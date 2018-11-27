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

#ifndef __IGS_UTIL_H__
#define __IGS_UTIL_H__

#include <glib-object.h>

#include <polkit/polkit.h>

G_BEGIN_DECLS

/**
 * IgsError:
 * @IGS_ERROR_FAILED:        The operation failed.
 * @IGS_ERROR_BUSY:          The operation failed because another operation is running.
 *
 * Error codes for the #IGS_ERROR error domain and the corresponding D-Bus
 * error names.
 */
typedef enum
{
  IGS_ERROR_FAILED,              /* com.intel.GPU.Service.Error.Failed */
  IGS_ERROR_BUSY,                /* com.intel.GPU.Service.Error.Busy */
  IGS_ERROR_AUTHORIZATION,       /* com.intel.GPU.Service.Error.Authorization */
  IGS_ERROR_IN_USE,              /* com.intel.GPU.Service.Error.InUse */
} IgsError;

/**
 * IGS_ERROR:
 *
 * Error domain for IgsServer. Errors in this domain will be form the
 * #IgsError enumeration. See #GError for more information on error domains.
 */
#define IGS_ERROR (igs_error_quark ())

GQuark igs_error_quark(void);

#define IGS_TYPE_POLKIT            (igs_polkit_get_type())
#define IGS_POLKIT(o)              (G_TYPE_CHECK_INSTANCE_CAST((o), IGS_TYPE_POLKIT, IgsPolkit))
#define IGS_POLKIT_CLASS(k)        (G_TYPE_CHECK_CLASS_CAST((k), IGS_TYPE_POLKIT, IgsPolkitClass))
#define IGS_IS_POLKIT(o)           (G_TYPE_CHECK_INSTANCE_TYPE((o), IGS_TYPE_POLKIT))
#define IGS_IS_POLKIT_CLASS(k)     (G_TYPE_CHECK_CLASS_TYPE((k), IGS_TYPE_POLKIT))
#define IGS_POLKIT_GET_CLASS(o)    (G_TYPE_INSTANCE_GET_CLASS((o), IGS_TYPE_POLKIT, IgsPolkitClass))

typedef struct IgsPolkitPrivate IgsPolkitPrivate;

typedef struct
{
   GObject parent;
   IgsPolkitPrivate *priv;
} IgsPolkit;

typedef struct
{
   GObjectClass parent_class;
} IgsPolkitClass;

GType          igs_polkit_get_type               (void);
IgsPolkit     *igs_polkit_get_default            (void);

PolkitSubject *igs_polkit_get_subject            (IgsPolkit *polkit,
                                                  GDBusMethodInvocation *context);
gboolean       igs_polkit_check_auth_from_subject(IgsPolkit             *polkit,
                                                  PolkitSubject         *subject,
                                                  const gchar           *action_id,
                                                  GDBusMethodInvocation *context);
gboolean       igs_polkit_is_allowed             (IgsPolkit              *polkit,
                                                  PolkitSubject         *subject,
                                                  const gchar           *action_id,
                                                  GError               **error);

gboolean       igs_polkit_check_auth             (IgsPolkit              *polkit,
                                                  const gchar           *action_id,
                                                  GDBusMethodInvocation *context);


G_END_DECLS

#endif /* __IGS_UTIL_H__ */
