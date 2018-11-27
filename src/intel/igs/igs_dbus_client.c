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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef MAJOR_IN_MKDEV
#include <sys/mkdev.h>
#endif
#ifdef MAJOR_IN_SYSMACROS
#include <sys/sysmacros.h>
#endif

#include <dbus/dbus.h>

#include "igs_client.h"
#include "common/gen_debug.h"

#define FILE_DEBUG_FLAG DEBUG_PERFMON

static int debug_dbus_error(DBusError *error, int value)
{
   DBG("DBus error: %s\n", error->message);
   dbus_error_free(error);
   return value;
}

int
igs_client_acquire_perf(int drm_fd,
                        int metrics_set_id,
                        int report_format,
                        int period_exponent,
                        uint32_t ctx_id,
                        uint64_t timeout_ms)
{
   DBusError error = DBUS_ERROR_INIT;
   DBusConnection *connection =
      dbus_connection_open_private(
         "unix:path=/var/run/dbus/system_bus_socket",
         &error);
   if (!connection)
      return debug_dbus_error(&error, -1);

   if (!connection)
      return debug_dbus_error(&error, -1);

   if (!dbus_bus_register(connection, &error)) {
      dbus_connection_close(connection);
      return debug_dbus_error(&error, -1);
   }

   DBusMessage *call_message = dbus_message_new_method_call("com.intel.GPU",
                                                            "/com/intel/GPU",
                                                            "com.intel.GPU",
                                                            "AcquirePerformanceDeviceForContext");
   dbus_bool_t sample_oa = true;
   if (!dbus_message_append_args(call_message,
                                 DBUS_TYPE_UNIX_FD, &drm_fd,
                                 DBUS_TYPE_UINT32, &ctx_id,
                                 DBUS_TYPE_BOOLEAN, &sample_oa,
                                 DBUS_TYPE_INT32, &metrics_set_id,
                                 DBUS_TYPE_INT32, &report_format,
                                 DBUS_TYPE_INT32, &period_exponent,
                                 DBUS_TYPE_INVALID)) {
      dbus_clear_message(&call_message);
      dbus_connection_close(connection);
      return -1;
   }

   DBusMessage *reply_message =
      dbus_connection_send_with_reply_and_block(connection, call_message,
                                                timeout_ms, &error);
   if (!reply_message) {
      dbus_clear_message(&call_message);
      dbus_connection_close(connection);
      return debug_dbus_error(&error, -1);
   }

   int perf_fd = -1;
   if (!dbus_message_get_args(reply_message, &error,
                              DBUS_TYPE_UNIX_FD, &perf_fd,
                              DBUS_TYPE_INVALID)) {
      dbus_clear_message(&call_message);
      dbus_clear_message(&reply_message);
      dbus_connection_close(connection);
      return debug_dbus_error(&error, -1);
   }

   DBG("Got perf_fd=%i from Intel GPU Service\n", perf_fd);

   dbus_clear_message(&call_message);
   dbus_clear_message(&reply_message);

   dbus_connection_close(connection);

   return perf_fd;
}

static int
get_device_number(int drm_fd)
{
   struct stat sb;
   if (fstat(drm_fd, &sb))
      return -1;

   int min = minor(sb.st_rdev);
   return min >= 128 ? (min - 128) : min;
}

bool
igs_client_can_register_configurations(int drm_fd)
{
   DBusError error = DBUS_ERROR_INIT;
   DBusConnection *connection =
      dbus_connection_open_private(
         "unix:path=/var/run/dbus/system_bus_socket",
         &error);
   if (!connection)
      return debug_dbus_error(&error, false);

   if (!connection)
      return debug_dbus_error(&error, false);

   if (!dbus_bus_register(connection, &error)) {
      dbus_connection_close(connection);
      return debug_dbus_error(&error, false);
   }

   const char *obj_property = "CanRegisterConfigurations";
   DBusMessage *call_message = dbus_message_new_method_call("com.intel.GPU",
                                                            "/com/intel/GPU",
                                                            "org.freedesktop.DBus.Properties",
                                                            "Get");
   if (!dbus_message_append_args(call_message,
                                 DBUS_TYPE_STRING, &obj_property,
                                 DBUS_TYPE_INVALID)) {
      dbus_clear_message(&call_message);
      dbus_connection_close(connection);
      return false;
   }

   DBusMessage *reply_message =
      dbus_connection_send_with_reply_and_block(connection, call_message,
                                                -1 /* forever */, &error);
   if (!reply_message) {
      dbus_clear_message(&call_message);
      dbus_connection_close(connection);
      return debug_dbus_error(&error, false);
   }

   dbus_bool_t can_register = 0;
   if (!dbus_message_get_args(reply_message, &error,
                              DBUS_TYPE_BOOLEAN, &can_register,
                              DBUS_TYPE_INVALID)) {
      dbus_clear_message(&call_message);
      dbus_clear_message(&reply_message);
      dbus_connection_close(connection);
      return debug_dbus_error(&error, false);
   }

   DBG("Can register performance configurations: %i\n", can_register);

   dbus_clear_message(&call_message);
   dbus_clear_message(&reply_message);

   dbus_connection_close(connection);

   return can_register;

}

uint32_t
igs_client_register_configuration(int drm_fd,
                                  const char *uuid,
                                  uint32_t n_mux_regs,
                                  const uint32_t *mux_regs,
                                  uint32_t n_boolean_regs,
                                  const uint32_t *boolean_regs,
                                  uint32_t n_flex_regs,
                                  const uint32_t *flex_regs)
{
   int device = get_device_number(drm_fd);
   if (device < 0)
      return 0;

   DBusError error = DBUS_ERROR_INIT;
   DBusConnection *connection =
      dbus_connection_open_private(
         "unix:path=/var/run/dbus/system_bus_socket",
         &error);
   if (!connection)
      return debug_dbus_error(&error, 0);

   if (!connection)
      return debug_dbus_error(&error, 0);

   if (!dbus_bus_register(connection, &error)) {
      dbus_connection_close(connection);
      return debug_dbus_error(&error, 0);
   }

   DBusMessage *call_message = dbus_message_new_method_call("com.intel.GPU",
                                                            "/com/intel/GPU",
                                                            "com.intel.GPU",
                                                            "RegisterPerformanceConfiguration");
   if (!dbus_message_append_args(call_message,
                                 DBUS_TYPE_UINT32, &device,
                                 DBUS_TYPE_STRING, &uuid,
                                 DBUS_TYPE_ARRAY, DBUS_TYPE_UINT32,
                                 &mux_regs, &n_mux_regs,
                                 DBUS_TYPE_ARRAY, DBUS_TYPE_UINT32,
                                 &boolean_regs, &n_boolean_regs,
                                 DBUS_TYPE_ARRAY, DBUS_TYPE_UINT32,
                                 &flex_regs, &n_flex_regs,
                                 DBUS_TYPE_INVALID)) {
      dbus_clear_message(&call_message);
      dbus_connection_close(connection);
      return 0;
   }

   DBusMessage *reply_message =
      dbus_connection_send_with_reply_and_block(connection, call_message,
                                                -1 /* forever */, &error);
   if (!reply_message) {
      dbus_clear_message(&call_message);
      dbus_connection_close(connection);
      return debug_dbus_error(&error, 0);
   }

   uint32_t metric_id = 0;
   if (!dbus_message_get_args(reply_message, &error,
                              DBUS_TYPE_UINT32, &metric_id,
                              DBUS_TYPE_INVALID)) {
      dbus_clear_message(&call_message);
      dbus_clear_message(&reply_message);
      dbus_connection_close(connection);
      return debug_dbus_error(&error, 0);
   }

   DBG("Got metric_id=%i from uuid=%s\n", metric_id, uuid);

   dbus_clear_message(&call_message);
   dbus_clear_message(&reply_message);

   dbus_connection_close(connection);

   return metric_id;
}
