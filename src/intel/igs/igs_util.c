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

#include <gio/gio.h>

#include "igs_util.h"

static const GDBusErrorEntry dbus_error_entries[] = {
   {IGS_ERROR_FAILED,        "com.intel.GPU.Error.Failed"        },
   {IGS_ERROR_BUSY,          "com.intel.GPU.Error.Busy"          },
   {IGS_ERROR_AUTHORIZATION, "com.intel.GPU.Error.Authorization" },
   {IGS_ERROR_IN_USE,        "com.intel.GPU.Error.InUse" },
};

GQuark
igs_error_quark (void)
{
   static volatile gsize quark_volatile = 0;
   g_dbus_error_register_error_domain("um-error-quark",
                                      &quark_volatile,
                                      dbus_error_entries,
                                      G_N_ELEMENTS(dbus_error_entries));
   return (GQuark) quark_volatile;
}

#define IGS_POLKIT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), UM_TYPE_POLKIT, IgsPolkitPrivate))

struct IgsPolkitPrivate
{
   PolkitAuthority *authority;
};

G_DEFINE_TYPE_WITH_CODE(IgsPolkit, igs_polkit, G_TYPE_OBJECT,
                        G_ADD_PRIVATE(IgsPolkit))

PolkitSubject *
igs_polkit_get_subject(IgsPolkit *polkit, GDBusMethodInvocation *context)
{
   GError *error;
   const gchar *sender;
   PolkitSubject *subject;

   sender = g_dbus_method_invocation_get_sender(context);
   subject = polkit_system_bus_name_new(sender);

   if (subject == NULL) {
      error = g_error_new(IGS_ERROR,
                          IGS_ERROR_AUTHORIZATION,
                          "failed to get PolicyKit subject");
      g_dbus_method_invocation_take_error(context, error);
   }

   return subject;
}

typedef struct {
   GMainLoop *mainloop;
   GAsyncResult *result;
} AsyncAuth;

static void
_auth_result(GObject *source_object, GAsyncResult *res, AsyncAuth *auth)
{
   auth->result = g_object_ref(res);
   g_main_loop_quit(auth->mainloop);
}

gboolean
igs_polkit_check_auth_from_subject(IgsPolkit *polkit,
                                  PolkitSubject *subject,
                                  const gchar *action_id,
                                  GDBusMethodInvocation *context)
{
   IgsPolkitPrivate *priv = igs_polkit_get_instance_private(polkit);
   gboolean ret = FALSE;
   GError *error_local = NULL;
   PolkitAuthorizationResult *result;
   AsyncAuth auth;

   g_return_val_if_fail(IGS_IS_POLKIT(polkit), FALSE);
   g_return_val_if_fail(POLKIT_IS_SUBJECT(subject), FALSE);
   g_return_val_if_fail(action_id != NULL, FALSE);
   g_return_val_if_fail(G_IS_DBUS_METHOD_INVOCATION(context), FALSE);

   auth.mainloop = g_main_loop_new(NULL, FALSE);

   /* check auth */
   polkit_authority_check_authorization(priv->authority,
                                        subject,
                                        action_id,
                                        NULL,
                                        POLKIT_CHECK_AUTHORIZATION_FLAGS_ALLOW_USER_INTERACTION,
                                        NULL,
                                        (GAsyncReadyCallback) _auth_result,
                                        &auth);

   g_main_loop_run(auth.mainloop);
   g_main_loop_unref(auth.mainloop);

   result = polkit_authority_check_authorization_finish(priv->authority,
                                                        auth.result,
                                                        &error_local);
   g_object_unref(auth.result);

   if (result == NULL) {
      g_dbus_method_invocation_return_error(context,
                                            IGS_ERROR,
                                            IGS_ERROR_AUTHORIZATION,
                                            "failed to check authorization: %s",
                                            error_local->message);
      g_error_free(error_local);
      goto out;
   }

   /* okay? */
   if (polkit_authorization_result_get_is_authorized(result))
      ret = TRUE;
   else
      g_dbus_method_invocation_return_error(context,
                                            IGS_ERROR,
                                            IGS_ERROR_AUTHORIZATION,
                                            "Authorization failed");

 out:
   if (result != NULL)
      g_object_unref(result);

   return ret;
}

gboolean
igs_polkit_is_allowed(IgsPolkit *polkit,
                     PolkitSubject *subject,
                     const gchar *action_id,
                     GError **error)
{
   IgsPolkitPrivate *priv = igs_polkit_get_instance_private(polkit);
   gboolean ret = FALSE;
   GError *error_local = NULL;
   PolkitAuthorizationResult *result;

   g_return_val_if_fail(IGS_IS_POLKIT(polkit), FALSE);
   g_return_val_if_fail(POLKIT_IS_SUBJECT(subject), FALSE);
   g_return_val_if_fail(action_id != NULL, FALSE);

   /* check auth */
   result = polkit_authority_check_authorization_sync(priv->authority,
                                                      subject, action_id, NULL,
                                                      POLKIT_CHECK_AUTHORIZATION_FLAGS_NONE,
                                                      NULL, &error_local);
   if (result == NULL) {
      g_set_error(error,
                  IGS_ERROR,
                  IGS_ERROR_AUTHORIZATION,
                  "failed to check authorization: %s",
                  error_local->message);
      g_error_free(error_local);
      goto out;
   }

   ret = polkit_authorization_result_get_is_authorized(result) ||
      polkit_authorization_result_get_is_challenge(result);

 out:
   if (result != NULL)
      g_object_unref(result);

   return ret;
}

gboolean
igs_polkit_check_auth(IgsPolkit *polkit,
                     const gchar *action_id,
                     GDBusMethodInvocation *context)
{
   PolkitSubject *subject;
   gboolean ret;

   g_return_val_if_fail(IGS_IS_POLKIT(polkit), FALSE);
   g_return_val_if_fail(action_id != NULL, FALSE);
   g_return_val_if_fail(G_IS_DBUS_METHOD_INVOCATION(context), FALSE);

   subject = igs_polkit_get_subject(polkit, context);
   if (subject == NULL) {
      g_dbus_method_invocation_return_error(context,
                                            IGS_ERROR,
                                            IGS_ERROR_AUTHORIZATION,
                                            "failed to get subject");
      return FALSE;
   }

   ret = igs_polkit_check_auth_from_subject(polkit, subject, action_id, context);

   g_object_unref(subject);

   return ret;
}

static void
igs_polkit_dispose(GObject *object)
{
   IgsPolkitPrivate *priv = igs_polkit_get_instance_private(IGS_POLKIT(object));

   g_clear_object(&priv->authority);

   G_OBJECT_CLASS(igs_polkit_parent_class)->dispose(object);
}

static void
igs_polkit_class_init(IgsPolkitClass *klass)
{
   GObjectClass *object_class = G_OBJECT_CLASS(klass);

   object_class->dispose = igs_polkit_dispose;
}

static void
igs_polkit_init(IgsPolkit *polkit)
{
   IgsPolkitPrivate *priv = igs_polkit_get_instance_private(polkit);
   GError *error = NULL;

   priv->authority = polkit_authority_get_sync(NULL, &error);
   if (priv->authority == NULL)
   {
      g_warning("failed to get pokit authority: %s", error->message);
      g_error_free(error);
   }
}

IgsPolkit *
igs_polkit_get_default(void)
{
   static IgsPolkit *polkit = NULL;

   if (G_UNLIKELY(polkit == NULL))
      polkit = g_object_new(IGS_TYPE_POLKIT, NULL);

   return polkit;
}
