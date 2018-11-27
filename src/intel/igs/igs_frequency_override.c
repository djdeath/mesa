/*
 * Copyright © 2019 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <glib/gi18n.h>
#include <gio/gio.h>

#include <polkit/polkit.h>

#include "igs_frequency_override.h"
#include "igs_server.h"
#include "igs_util.h"

static void frequency_iface_init(IgsComIntelGPUFrequencyIface *iface);

typedef struct {
   int device;

   char dbus_path[100];

   IgsServer *server;
   GDBusConnection *dbus_connection;
   char *dbus_owner;

   uint64_t min_frequency;
   uint64_t max_frequency;
   uint64_t boost_frequency;

   uint64_t saved_min_frequency;
   uint64_t saved_max_frequency;
   uint64_t saved_boost_frequency;

   guint watch_id;
} IgsFrequencyOverridePrivate;

G_DEFINE_TYPE_WITH_CODE(IgsFrequencyOverride, igs_frequency_override, IGS_TYPE_COM_INTEL_GPUFREQUENCY_SKELETON,
                        G_ADD_PRIVATE(IgsFrequencyOverride)
                        G_IMPLEMENT_INTERFACE(IGS_TYPE_COM_INTEL_GPUFREQUENCY, frequency_iface_init))

#define FREQUENCY_OVERRIDE_PRIVATE(o)                                   \
   (G_TYPE_INSTANCE_GET_PRIVATE((o), IGS_TYPE_FREQUENCY_OVERRIDE, IgsFrequencyOverridePrivate))


enum
{
   PROP_0,
   PROP_SERVER,
   PROP_DBUS_CONNECTION,
   PROP_DBUS_OWNER,
};

/**/

static bool
read_file_uint64(int device, const char *device_file, uint64_t *val)
{
   char path[100];
   char buf[32];
   int fd, n;

   snprintf(path, sizeof(path), "/sys/class/drm/card%i/%s", device, device_file);

   fd = open(path, O_RDONLY);
   if (fd < 0)
      return false;
   while ((n = read(fd, buf, sizeof (buf) - 1)) < 0 &&
          errno == EINTR);
   close(fd);
   if (n < 0)
      return false;

   buf[n] = '\0';
   *val = strtoull(buf, NULL, 0);

   return true;
}

static bool
write_file_uint64(int device, const char *device_file, uint64_t val)
{
   char path[100];
   char buf[32];
   int fd, n;

   snprintf(path, sizeof(path), "/sys/class/drm/card%i/%s", device, device_file);
   n = snprintf(buf, sizeof(buf), "%lu", val);

   fd = open(path, O_WRONLY);
   if (fd < 0)
      return false;

   while ((n = write(fd, buf, strlen(buf) + 1)) < 0 &&
          errno == EINTR);
   close(fd);
   if (n < 0)
      return false;

   return true;
}

static void
_igs_frequency_override_release(IgsFrequencyOverride *override)
{
   IgsFrequencyOverridePrivate *priv =
      igs_frequency_override_get_instance_private(override);

   write_file_uint64(priv->device, "gt_min_freq_mhz", priv->saved_min_frequency);
   write_file_uint64(priv->device, "gt_max_freq_mhz", priv->saved_max_frequency);
   write_file_uint64(priv->device, "gt_boost_freq_mhz", priv->saved_boost_frequency);
   igs_server_release_frequency_override(priv->server, priv->device);
}

static gboolean
igs_frequency_override_release(IgsComIntelGPUFrequency *object,
                               GDBusMethodInvocation *invocation)
{
   IgsFrequencyOverridePrivate *priv =
      igs_frequency_override_get_instance_private(IGS_FREQUENCY_OVERRIDE(object));

   if (strcmp(g_dbus_method_invocation_get_sender(invocation), priv->dbus_owner)) {
      g_info("Sender %s is not allowed to release frequency override, owned by %s",
             g_dbus_method_invocation_get_sender(invocation), priv->dbus_owner);
      g_dbus_method_invocation_return_error(invocation,
                                            IGS_ERROR,
                                            IGS_ERROR_AUTHORIZATION,
                                            "Sender is not allowed to release frequency override");
      return TRUE;
   }

   igs_com_intel_gpufrequency_complete_release(object, invocation);

   _igs_frequency_override_release(IGS_FREQUENCY_OVERRIDE(object));

   return TRUE;
}

static gboolean
igs_frequency_override_set_frequency(IgsComIntelGPUFrequency *object,
                                     GDBusMethodInvocation *invocation,
                                     guint arg_min_frequency,
                                     guint arg_max_frequency,
                                     guint arg_boost_frequency)
{
   IgsFrequencyOverridePrivate *priv =
      igs_frequency_override_get_instance_private(IGS_FREQUENCY_OVERRIDE(object));

   if (strcmp(g_dbus_method_invocation_get_sender(invocation), priv->dbus_owner)) {
      g_info("Sender %s is not allowed to set frequency, owned by %s",
             g_dbus_method_invocation_get_sender(invocation), priv->dbus_owner);
      g_dbus_method_invocation_return_error(invocation,
                                            IGS_ERROR,
                                            IGS_ERROR_AUTHORIZATION,
                                            "Sender is not allowed to set frequency");
      return TRUE;
   }

   g_info("Override device frequency to min/max/boot: %u/%u/%u",
          arg_min_frequency, arg_max_frequency, arg_boost_frequency);

   if (priv->min_frequency != arg_min_frequency) {
      priv->min_frequency = arg_min_frequency;
      if (!write_file_uint64(priv->device, "gt_min_freq_mhz", priv->min_frequency)) {
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Failed to set min frequency");
         return TRUE;
      }
      g_object_set(G_OBJECT(object), "min-frequency", priv->min_frequency, NULL);
   }
   if (priv->max_frequency != arg_max_frequency) {
      priv->max_frequency = arg_max_frequency;
      if (!write_file_uint64(priv->device, "gt_max_freq_mhz", priv->max_frequency)) {
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Failed to set max frequency");
         return TRUE;
      }
      g_object_set(G_OBJECT(object), "max-frequency", priv->max_frequency, NULL);
   }
   if (priv->boost_frequency != arg_boost_frequency) {
      priv->boost_frequency = arg_boost_frequency;
      if (!write_file_uint64(priv->device, "gt_boost_freq_mhz", priv->boost_frequency)) {
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Failed to set boost frequency");
         return TRUE;
      }
      g_object_set(G_OBJECT(object), "boost-frequency", priv->boost_frequency, NULL);
   }

   igs_com_intel_gpufrequency_complete_set_frequency(object, invocation);

   return TRUE;
}

static void frequency_iface_init(IgsComIntelGPUFrequencyIface *iface)
{
   iface->handle_release = igs_frequency_override_release;
   iface->handle_set_frequency = igs_frequency_override_set_frequency;
}

static void
igs_frequency_override_owner_gone(GDBusConnection *connection,
                                   const gchar *name,
                                   gpointer user_data)
{
   g_info("Owner %s is gone, resetting GPU frequency", name);
   _igs_frequency_override_release(IGS_FREQUENCY_OVERRIDE(user_data));
}

/**/

static void
igs_frequency_override_get_property(GObject    *object,
                                    guint       property_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
   IgsFrequencyOverridePrivate *priv =
      igs_frequency_override_get_instance_private(IGS_FREQUENCY_OVERRIDE(object));

   switch (property_id)
   {
   case PROP_SERVER:
      g_value_set_object(value, priv->server);
      break;
   case PROP_DBUS_CONNECTION:
      g_value_set_object(value, priv->dbus_connection);
      break;
   case PROP_DBUS_OWNER:
      g_value_set_string(value, priv->dbus_owner);
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
   }
}

static void
igs_frequency_override_set_property(GObject      *object,
                                    guint         property_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
   IgsFrequencyOverridePrivate *priv =
      igs_frequency_override_get_instance_private(IGS_FREQUENCY_OVERRIDE(object));

   switch (property_id) {
   case PROP_SERVER:
      g_clear_object(&priv->server);
      priv->server = g_value_dup_object(value);
      break;
   case PROP_DBUS_CONNECTION:
      g_clear_object(&priv->dbus_connection);
      priv->dbus_connection = g_value_dup_object(value);
      break;
   case PROP_DBUS_OWNER:
      g_clear_pointer(&priv->dbus_owner, g_free);
      priv->dbus_owner = g_value_dup_string(value);
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
   }
}

static void
igs_frequency_override_dispose(GObject *object)
{
   IgsFrequencyOverridePrivate *priv =
      igs_frequency_override_get_instance_private(IGS_FREQUENCY_OVERRIDE(object));

   g_clear_object(&priv->server);
   g_clear_object(&priv->dbus_connection);
   g_clear_pointer(&priv->dbus_owner, g_free);

   if (priv->watch_id) {
      g_bus_unwatch_name(priv->watch_id);
      priv->watch_id = 0;
   }

   G_OBJECT_CLASS(igs_frequency_override_parent_class)->dispose(object);
}

static void
igs_frequency_override_finalize(GObject *object)
{
   G_OBJECT_CLASS(igs_frequency_override_parent_class)->finalize(object);
}

static void
igs_frequency_override_constructed(GObject *object)
{
   IgsFrequencyOverridePrivate *priv =
      igs_frequency_override_get_instance_private(IGS_FREQUENCY_OVERRIDE(object));

   G_OBJECT_CLASS(igs_frequency_override_parent_class)->constructed(object);

   if (!read_file_uint64(priv->device, "gt_min_freq_mhz", &priv->saved_min_frequency))
      g_warning("Cannot read min GT frequency");
   if (!read_file_uint64(priv->device, "gt_max_freq_mhz", &priv->saved_max_frequency))
      g_warning("Cannot read max GT frequency");
   if (!read_file_uint64(priv->device, "gt_boost_freq_mhz", &priv->saved_boost_frequency))
      g_warning("Cannot read boost GT frequency");

   priv->min_frequency = priv->saved_min_frequency;
   priv->max_frequency = priv->saved_max_frequency;
   priv->boost_frequency = priv->saved_boost_frequency;
   g_object_set(object,
                "min-frequency", priv->min_frequency,
                "max-frequency", priv->max_frequency,
                "boost-frequency", priv->boost_frequency,
                NULL);

   priv->watch_id = g_bus_watch_name_on_connection(priv->dbus_connection,
                                                   priv->dbus_owner,
                                                   G_BUS_NAME_WATCHER_FLAGS_NONE,
                                                   NULL,
                                                   igs_frequency_override_owner_gone,
                                                   object,
                                                   NULL);

   snprintf(priv->dbus_path, sizeof(priv->dbus_path),
            "/com/intel/GPU/Frequency/%d", priv->device);
   g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(object),
                                    priv->dbus_connection,
                                    priv->dbus_path, NULL);
}

static void
igs_frequency_override_class_init(IgsFrequencyOverrideClass *klass)
{
   GObjectClass *object_class = G_OBJECT_CLASS(klass);

   object_class->get_property = igs_frequency_override_get_property;
   object_class->set_property = igs_frequency_override_set_property;
   object_class->dispose = igs_frequency_override_dispose;
   object_class->finalize = igs_frequency_override_finalize;
   object_class->constructed = igs_frequency_override_constructed;

   g_object_class_install_property(object_class,
                                   PROP_SERVER,
                                   g_param_spec_object("server",
                                                       "Server",
                                                       "Server",
                                                       IGS_TYPE_SERVER,
                                                       G_PARAM_READWRITE |
                                                       G_PARAM_STATIC_STRINGS |
                                                       G_PARAM_CONSTRUCT_ONLY));
   g_object_class_install_property(object_class,
                                   PROP_DBUS_CONNECTION,
                                   g_param_spec_object("dbus-connection",
                                                       "DBus Connection",
                                                       "DBus Connection",
                                                       G_TYPE_DBUS_CONNECTION,
                                                       G_PARAM_READWRITE |
                                                       G_PARAM_STATIC_STRINGS |
                                                       G_PARAM_CONSTRUCT_ONLY));
   g_object_class_install_property(object_class,
                                   PROP_DBUS_OWNER,
                                   g_param_spec_string("dbus-owner",
                                                       "DBus Owner",
                                                       "DBus Owner",
                                                       "",
                                                       G_PARAM_READWRITE |
                                                       G_PARAM_STATIC_STRINGS |
                                                       G_PARAM_CONSTRUCT_ONLY));
}

static void
igs_frequency_override_init(IgsFrequencyOverride *self)
{
}

IgsFrequencyOverride *
igs_frequency_override_new(IgsServer *server,
                           int device,
                           GDBusConnection *dbus_connection,
                           const char *dbus_owner)
{
   return g_object_new(IGS_TYPE_FREQUENCY_OVERRIDE,
                       "server", server,
                       "device", device,
                       "dbus-connection", dbus_connection,
                       "dbus-owner", dbus_owner,
                       NULL);
}

const char *
igs_frequency_override_get_dbus_path(IgsFrequencyOverride *override)
{
   IgsFrequencyOverridePrivate *priv =
      igs_frequency_override_get_instance_private(override);

   return priv->dbus_path;
}

const char *
igs_frequency_override_get_dbus_owner(IgsFrequencyOverride *override)
{
   IgsFrequencyOverridePrivate *priv =
      igs_frequency_override_get_instance_private(override);

   return priv->dbus_owner;
}
