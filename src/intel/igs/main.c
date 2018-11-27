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

#include <glib/gi18n.h>
#include <gio/gio.h>

#include "igs_server.h"

/**/

static GMainLoop *loop = NULL;
static gboolean opt_replace = FALSE;
static gboolean opt_debug = FALSE;
static gboolean opt_always_grant = FALSE;
static GOptionEntry opt_entries[] =
{
   {"replace",      'r', 0, G_OPTION_ARG_NONE, &opt_replace,      "Replace existing daemon", NULL},
   {"debug",        'd', 0, G_OPTION_ARG_NONE, &opt_debug,        "Print debug information", NULL},
   {"always-grant", 'a', 0, G_OPTION_ARG_NONE, &opt_always_grant, "Always grant permission (for debug)", NULL},
   {NULL }
};

static IgsServer *server = NULL;

static void
on_bus_acquired(GDBusConnection *connection,
                const gchar     *name,
                gpointer         user_data)
{
   g_info("Connected to the system bus");
}

static void
on_name_lost(GDBusConnection *connection,
             const gchar     *name,
             gpointer         user_data)
{
   if (server == NULL)
      g_warning("Failed to connect to the system message bus");
   else
      g_info("Lost the name %s on the system message bus", name);

   g_main_loop_quit(loop);
}

static void
on_name_acquired(GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
   g_debug("Acquired the name %s on the system message bus (capable=%i)", name,
           g_dbus_connection_get_capabilities(connection) & G_DBUS_CAPABILITY_FLAGS_UNIX_FD_PASSING);
   server = igs_server_new(connection);
}

static int log_levels =
   G_LOG_LEVEL_CRITICAL |
   G_LOG_LEVEL_ERROR    |
   G_LOG_LEVEL_WARNING  |
   G_LOG_LEVEL_MESSAGE  |
   G_LOG_LEVEL_INFO;

static void
log_handler (const gchar    *log_domain,
             GLogLevelFlags  log_level,
             const gchar    *message,
             gpointer        unused_data)
{
   if ((log_level & log_levels) == 0)
      return;

   g_log_default_handler(log_domain, log_level, message, unused_data);
}
int
main(int    argc,
     char **argv)
{
   GError *error;
   GOptionContext *opt_context;
   gint ret;
   guint name_owner_id;

   ret = 1;
   loop = NULL;
   opt_context = NULL;
   name_owner_id = 0;

   opt_context = g_option_context_new("Intel GPU service");
   g_option_context_add_main_entries(opt_context, opt_entries, NULL);
   error = NULL;
   if (!g_option_context_parse(opt_context, &argc, &argv, &error))
   {
      g_printerr("Error parsing options: %s\n", error->message);
      g_error_free(error);
      goto out;
   }

   g_log_set_default_handler(log_handler, NULL);

   if (opt_debug)
      log_levels |= G_LOG_LEVEL_DEBUG;

   g_debug("Intel GPU daemon starting");

   loop = g_main_loop_new(NULL, FALSE);

   name_owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
                                  "com.intel.GPU",
                                  G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT |
                                  (opt_replace ? G_BUS_NAME_OWNER_FLAGS_REPLACE : 0),
                                  on_bus_acquired,
                                  on_name_acquired,
                                  on_name_lost,
                                  NULL,
                                  NULL);

   g_debug("Entering main event loop");

   g_main_loop_run(loop);

   if (server)
      g_object_unref(server);

   ret = 0;

 out:
   if (name_owner_id != 0)
      g_bus_unown_name(name_owner_id);
   if (loop != NULL)
      g_main_loop_unref(loop);
   if (opt_context != NULL)
      g_option_context_free(opt_context);

   g_debug("updatemanager daemon version %s exiting", PACKAGE_VERSION);

   return ret;
}
