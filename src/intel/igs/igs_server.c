/*
 * Copyright © 2018 Intel Corporation
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib/gi18n.h>
#include <gio/gunixfdlist.h>
#include <gio/gio.h>

#include <polkit/polkit.h>

#include "igs_frequency_override.h"
#include "igs_server.h"
#include "igs_util.h"

#include <xf86drm.h>
#include <i915_drm.h>

static void service_iface_init(IgsComIntelGPUIface *iface);

typedef struct {
   GDBusConnection *dbus_connection;

   GHashTable *frequency_overrides;
} IgsServerPrivate;

G_DEFINE_TYPE_WITH_CODE(IgsServer, igs_server, IGS_TYPE_COM_INTEL_GPU_SKELETON,
                        G_ADD_PRIVATE(IgsServer)
                        G_IMPLEMENT_INTERFACE(IGS_TYPE_COM_INTEL_GPU, service_iface_init))

#define SERVER_PRIVATE(o)                                               \
   (G_TYPE_INSTANCE_GET_PRIVATE((o), IGS_TYPE_SERVER, IgsServerPrivate))


enum
{
   PROP_0,
   PROP_DBUS_CONNECTION,
};

/**/

static gboolean
igs_server_acquire_frequency_override(IgsComIntelGPU *object,
                                      GDBusMethodInvocation *invocation,
                                      guint arg_device)
{
   IgsServerPrivate *priv = igs_server_get_instance_private(IGS_SERVER(object));
   IgsFrequencyOverride *override = g_hash_table_lookup(priv->frequency_overrides,
                                                        GINT_TO_POINTER(arg_device));

   /* TODO: maybe verify that the device exist and is Intel/i915 branded. */

   if (override) {
      g_info("Frequency override for device%i already in use by %s",
             arg_device, igs_frequency_override_get_dbus_owner(override));
      g_dbus_method_invocation_return_error(invocation,
                                            IGS_ERROR,
                                            IGS_ERROR_BUSY,
                                            "Frequency override alread in use");
      return TRUE;
   }

   if (!igs_polkit_check_auth(igs_polkit_get_default(),
                              "com.intel.GPU.frequency-override",
                              invocation)) {
      g_info("Authorization refused.");
      g_dbus_method_invocation_return_error(invocation,
                                            IGS_ERROR,
                                            IGS_ERROR_AUTHORIZATION,
                                            "Frequency override alread in use");
      return TRUE;
   }

   override = igs_frequency_override_new(IGS_SERVER(object),
                                         arg_device,
                                         priv->dbus_connection,
                                         g_dbus_method_invocation_get_sender(invocation));
   g_hash_table_insert(priv->frequency_overrides, GINT_TO_POINTER(arg_device), override);

   igs_com_intel_gpu_complete_acquire_frequency_override(object, invocation,
                                                         igs_frequency_override_get_dbus_path(override));

   g_info("Device%i frequency override owned by %s", arg_device,
          g_dbus_method_invocation_get_sender(invocation));

   return TRUE;
}

static int
open_i915_perf_oa_stream(int drm_fd,
                         bool sample_oa,
                         int metrics_set_id,
                         int report_format,
                         int period_exponent,
                         bool use_context,
                         int context_id)
{
   uint64_t properties[] = {
      /* Include OA reports in samples */
      DRM_I915_PERF_PROP_SAMPLE_OA, sample_oa,

      /* OA unit configuration */
      DRM_I915_PERF_PROP_OA_METRICS_SET, metrics_set_id,
      DRM_I915_PERF_PROP_OA_FORMAT, report_format,
      DRM_I915_PERF_PROP_OA_EXPONENT, period_exponent,

      /* Single context sampling */
      DRM_I915_PERF_PROP_CTX_HANDLE, context_id,
   };
   uint32_t num_properties = use_context ?
      (G_N_ELEMENTS(properties) / 2) :
      ((G_N_ELEMENTS(properties) / 2) - 1);
   struct drm_i915_perf_open_param param = {
      .flags = I915_PERF_FLAG_FD_CLOEXEC |
               I915_PERF_FLAG_FD_NONBLOCK |
               I915_PERF_FLAG_DISABLED,
      .num_properties = num_properties,
      .properties_ptr = (uintptr_t) properties,
   };

   return drmIoctl(drm_fd, DRM_IOCTL_I915_PERF_OPEN, &param);
}

static gboolean
igs_server_acquire_performance_device_for_context(IgsComIntelGPU *object,
                                                  GDBusMethodInvocation *invocation,
                                                  GUnixFDList *fd_list,
                                                  GVariant *arg_drm_fd,
                                                  guint arg_context,
                                                  gboolean arg_sample_oa,
                                                  gint arg_metric_set,
                                                  gint arg_oa_format,
                                                  gint arg_oa_exponent)
{
   GError *drm_fd_error = NULL;
   int drm_fd = g_unix_fd_list_get(fd_list,
                                   g_variant_get_handle(arg_drm_fd),
                                   &drm_fd_error);
   int perf_fd;

   if (drm_fd_error) {
      g_info("Unable to get drm file descriptor : %s", drm_fd_error->message);
      g_error_free(drm_fd_error);
      g_dbus_method_invocation_return_error(invocation,
                                            IGS_ERROR,
                                            IGS_ERROR_FAILED,
                                            "Unable to retrieve context file descriptor");
      return TRUE;
   }

   if (!igs_polkit_check_auth(igs_polkit_get_default(),
                              "com.intel.GPU.monitor",
                              invocation)) {
      close(drm_fd);
      g_info("Authorization refused.");
      g_dbus_method_invocation_return_error(invocation,
                                            IGS_ERROR,
                                            IGS_ERROR_AUTHORIZATION,
                                            "Frequency override alread in use");
      return TRUE;
   }

   perf_fd = open_i915_perf_oa_stream(drm_fd, arg_sample_oa,
                                      arg_metric_set, arg_oa_format, arg_oa_exponent,
                                      true, arg_context);
   close(drm_fd);

   if (perf_fd < 0) {
      switch (errno) {
      case EBUSY:
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Performance monitoring device in use");
         break;
      case EINVAL:
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Invalid opening parameter");
         break;
      default:
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Unable to open performance monitoring device");
         break;
      }
      return TRUE;
   }

   g_info("Performance HW owned by %s", g_dbus_method_invocation_get_sender(invocation));

   GUnixFDList *reply_fd_list = g_unix_fd_list_new_from_array(&perf_fd, 1);
   igs_com_intel_gpu_complete_acquire_performance_device_for_context(object, invocation,
                                                                     reply_fd_list,
                                                                     g_variant_new_handle(0));

   g_object_unref(G_OBJECT(reply_fd_list));

   return TRUE;
}

static gboolean
igs_server_acquire_performance_device(IgsComIntelGPU *object,
                                      GDBusMethodInvocation *invocation,
                                      GUnixFDList *fd_list,
                                      GVariant *arg_drm_fd,
                                      gboolean arg_sample_oa,
                                      gint arg_metric_set,
                                      gint arg_oa_format,
                                      gint arg_oa_exponent)
{
   GError *drm_fd_error = NULL;
   int drm_fd = g_unix_fd_list_get(fd_list,
                                   g_variant_get_handle(arg_drm_fd),
                                   &drm_fd_error);
   int perf_fd;

   if (drm_fd_error) {
      g_info("Unable to get drm file descriptor : %s", drm_fd_error->message);
      g_error_free(drm_fd_error);
      g_dbus_method_invocation_return_error(invocation,
                                            IGS_ERROR,
                                            IGS_ERROR_FAILED,
                                            "Unable to retrieve context file descriptor");
      return TRUE;
   }

   if (!igs_polkit_check_auth(igs_polkit_get_default(),
                              "com.intel.GPU.monitor",
                              invocation)) {
      close(drm_fd);
      g_info("Authorization refused.");
      g_dbus_method_invocation_return_error(invocation,
                                            IGS_ERROR,
                                            IGS_ERROR_AUTHORIZATION,
                                            "Frequency override alread in use");
      return TRUE;
   }

   perf_fd = open_i915_perf_oa_stream(drm_fd, arg_sample_oa,
                                      arg_metric_set, arg_oa_format, arg_oa_exponent,
                                      false, 0);
   close(drm_fd);

   if (perf_fd < 0) {
      switch (errno) {
      case EBUSY:
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Performance monitoring device in use");
         break;
      case EINVAL:
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Invalid opening parameter");
         break;
      default:
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Unable to open performance monitoring device");
         break;
      }
      return TRUE;
   }

   g_info("Performance HW owned by %s", g_dbus_method_invocation_get_sender(invocation));

   GUnixFDList *reply_fd_list = g_unix_fd_list_new_from_array(&perf_fd, 1);
   igs_com_intel_gpu_complete_acquire_performance_device_for_context(object, invocation,
                                                                     reply_fd_list,
                                                                     g_variant_new_handle(0));

   g_object_unref(G_OBJECT(reply_fd_list));

   return TRUE;
}

static uint32_t *
alloc_and_fill_registers(GVariant *registers)
{
   GVariantIter iter;
   g_variant_iter_init(&iter, registers);

   if (g_variant_iter_n_children(&iter) == 0)
      return NULL;

   uint32_t *regs = malloc(sizeof(uint32_t) * g_variant_iter_n_children(&iter));

   GVariant *item;
   uint32_t i = 0;
   while ((item = g_variant_iter_next_value(&iter)))
      regs[i++] = g_variant_get_uint32(item);

   return regs;
}

static gboolean
igs_server_register_performance_configuration(IgsComIntelGPU *object,
                                              GDBusMethodInvocation *invocation,
                                              guint arg_device,
                                              const gchar *arg_uuid,
                                              GVariant *arg_mux_registers,
                                              GVariant *arg_boolean_registers,
                                              GVariant *arg_flex_registers)
{
   struct drm_i915_perf_oa_config config;

   if ((g_variant_n_children(arg_mux_registers) % 2) != 0 ||
       (g_variant_n_children(arg_boolean_registers) % 2) != 0 ||
       (g_variant_n_children(arg_flex_registers) % 2) != 0 ||
       strlen(arg_uuid) < G_N_ELEMENTS(config.uuid)) {
      g_dbus_method_invocation_return_error_literal(invocation,
                                                    IGS_ERROR,
                                                    IGS_ERROR_FAILED,
                                                    "Invalid configuration data");
      return TRUE;
   }

   char drm_path[100];
   snprintf(drm_path, sizeof(drm_path),
            "/dev/dri/renderD%u", 128 + arg_device);

   int drm_fd = open(drm_path, 0);
   if (drm_fd < 0) {
      g_dbus_method_invocation_return_error_literal(invocation,
                                                    IGS_ERROR,
                                                    IGS_ERROR_FAILED,
                                                    "Cannot open DRM device");
      return TRUE;
   }

   uint32_t *mux_regs = alloc_and_fill_registers(arg_mux_registers);
   uint32_t *boolean_regs = alloc_and_fill_registers(arg_boolean_registers);
   uint32_t *flex_regs = alloc_and_fill_registers(arg_flex_registers);

   memset(&config, 0, sizeof(config));
   config.n_mux_regs = g_variant_n_children(arg_mux_registers) / 2;
   config.mux_regs_ptr = (uintptr_t) mux_regs;
   config.n_boolean_regs = g_variant_n_children(arg_boolean_registers) / 2;
   config.boolean_regs_ptr = (uintptr_t) boolean_regs;
   config.n_flex_regs = g_variant_n_children(arg_flex_registers) / 2;
   config.flex_regs_ptr = (uintptr_t) flex_regs;

   int ret = drmIoctl(drm_fd, DRM_IOCTL_I915_PERF_ADD_CONFIG, &config);
   if (ret < 0) {
      if (errno == EADDRINUSE) {
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_IN_USE,
                                               "Performance configuration already registered");
      } else {
         g_dbus_method_invocation_return_error(invocation,
                                               IGS_ERROR,
                                               IGS_ERROR_FAILED,
                                               "Unable to register performance configuration");
      }
      return TRUE;
   }

   free(mux_regs);
   free(boolean_regs);
   free(flex_regs);
   close(drm_fd);

   igs_com_intel_gpu_complete_register_performance_configuration(object, invocation, ret);

   return TRUE;
}

static gboolean
igs_server_unregister_performance_configuration(IgsComIntelGPU *object,
                                                GDBusMethodInvocation *invocation,
                                                guint arg_device,
                                                guint arg_metric_set)
{
   char drm_path[100];
   snprintf(drm_path, sizeof(drm_path),
            "/dev/dri/renderD%u", 128 + arg_device);

   int drm_fd = open(drm_path, 0);
   if (drm_fd < 0) {
      g_dbus_method_invocation_return_error_literal(invocation,
                                                    IGS_ERROR,
                                                    IGS_ERROR_FAILED,
                                                    "Cannot open DRM device");
      return TRUE;
   }

   uint64_t metric_id = arg_metric_set;
   int ret = drmIoctl(drm_fd, DRM_IOCTL_I915_PERF_REMOVE_CONFIG, &metric_id);
   if (ret < 0) {
      g_dbus_method_invocation_return_error(invocation,
                                            IGS_ERROR,
                                            IGS_ERROR_FAILED,
                                            "Unable to unregister performance configuration");
      return TRUE;
   }

   igs_com_intel_gpu_complete_unregister_performance_configuration(object, invocation);

   return TRUE;
}

static void service_iface_init(IgsComIntelGPUIface *iface)
{
   iface->handle_acquire_frequency_override = igs_server_acquire_frequency_override;
   iface->handle_acquire_performance_device_for_context = igs_server_acquire_performance_device_for_context;
   iface->handle_acquire_performance_device = igs_server_acquire_performance_device;
   iface->handle_register_performance_configuration = igs_server_register_performance_configuration;
   iface->handle_unregister_performance_configuration = igs_server_unregister_performance_configuration;
}

/**/

static void
igs_server_get_property(GObject    *object,
                        guint       property_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
   IgsServerPrivate *priv = igs_server_get_instance_private(IGS_SERVER(object));

   switch (property_id)
   {
   case PROP_DBUS_CONNECTION:
      g_value_set_object(value, priv->dbus_connection);
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
   }
}

static void
igs_server_set_property(GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
   IgsServerPrivate *priv = igs_server_get_instance_private(IGS_SERVER(object));

   switch (property_id) {
   case PROP_DBUS_CONNECTION:
      g_clear_object(&priv->dbus_connection);
      priv->dbus_connection = g_value_dup_object(value);
      break;
   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
   }
}

static void
igs_server_dispose(GObject *object)
{
   IgsServerPrivate *priv = igs_server_get_instance_private(IGS_SERVER(object));

   g_clear_pointer(&priv->frequency_overrides, g_hash_table_unref);
   g_clear_object(&priv->dbus_connection);

   G_OBJECT_CLASS(igs_server_parent_class)->dispose(object);
}

static void
igs_server_finalize(GObject *object)
{
   G_OBJECT_CLASS(igs_server_parent_class)->finalize(object);
}

static void
igs_server_constructed(GObject *object)
{
   IgsServerPrivate *priv = igs_server_get_instance_private(IGS_SERVER(object));

   G_OBJECT_CLASS(igs_server_parent_class)->constructed(object);

   g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(object),
                                    priv->dbus_connection,
                                    "/com/intel/GPU",
                                    NULL);
}

static void
igs_server_class_init(IgsServerClass *klass)
{
   GObjectClass *object_class = G_OBJECT_CLASS(klass);

   object_class->get_property = igs_server_get_property;
   object_class->set_property = igs_server_set_property;
   object_class->dispose = igs_server_dispose;
   object_class->finalize = igs_server_finalize;
   object_class->constructed = igs_server_constructed;

   g_object_class_install_property(object_class,
                                   PROP_DBUS_CONNECTION,
                                   g_param_spec_object("dbus-connection",
                                                       "DBus Connection",
                                                       "DBus Connection",
                                                       G_TYPE_DBUS_CONNECTION,
                                                       G_PARAM_READWRITE |
                                                       G_PARAM_STATIC_STRINGS |
                                                       G_PARAM_CONSTRUCT_ONLY));

}

static void
igs_server_init(IgsServer *self)
{
   IgsServerPrivate *priv = igs_server_get_instance_private(self);

   priv->frequency_overrides = g_hash_table_new_full(g_direct_hash,
                                                     g_direct_equal,
                                                     NULL,
                                                     g_object_unref);
}

IgsServer *
igs_server_new(GDBusConnection *dbus_connection)
{
   return g_object_new(IGS_TYPE_SERVER,
                       "dbus-connection", dbus_connection,
                       NULL);
}

void
igs_server_release_frequency_override(IgsServer *server,
                                      int device)
{
   IgsServerPrivate *priv = igs_server_get_instance_private(server);

   g_info("Device%i frequency override released", device);

   g_hash_table_remove(priv->frequency_overrides, GINT_TO_POINTER(device));
}
