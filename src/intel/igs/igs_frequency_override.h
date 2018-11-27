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

#ifndef __IGS_FREQUENCY_OVERRIDE_H__
#define __IGS_FREQUENCY_OVERRIDE_H__

#include <glib-object.h>
#include <gio/gio.h>

#include "com_intel_gpu.h"

G_BEGIN_DECLS

#define IGS_TYPE_FREQUENCY_OVERRIDE igs_frequency_override_get_type()

#define IGS_FREQUENCY_OVERRIDE(obj)                                     \
   (G_TYPE_CHECK_INSTANCE_CAST((obj),                                   \
                               IGS_TYPE_FREQUENCY_OVERRIDE,             \
                               IgsFrequencyOverride))

#define IGS_FREQUENCY_OVERRIDE_CLASS(klass)                             \
   (G_TYPE_CHECK_CLASS_CAST((klass),                                    \
                            IGS_TYPE_FREQUENCY_OVERRIDE,                \
                            IgsFrequencyOverrideClass))

#define IGS_IS_FREQUENCY_OVERRIDE(obj)                              \
   (G_TYPE_CHECK_INSTANCE_TYPE((obj),                               \
                               IGS_TYPE_FREQUENCY_OVERRIDE))

#define IGS_IS_FREQUENCY_OVERRIDE_CLASS(klass)                  \
   (G_TYPE_CHECK_CLASS_TYPE((klass),                            \
                            IGS_TYPE_FREQUENCY_OVERRIDE))

#define IGS_FREQUENCY_OVERRIDE_GET_CLASS(obj)                           \
   (G_TYPE_INSTANCE_GET_CLASS((obj),                                    \
                              IGS_TYPE_FREQUENCY_OVERRIDE,              \
                              IgsFrequencyOverrideClass))

typedef struct _IgsServer IgsServer;
typedef struct _IgsFrequencyOverride IgsFrequencyOverride;
typedef struct _IgsFrequencyOverrideClass IgsFrequencyOverrideClass;

struct _IgsFrequencyOverride
{
  IgsComIntelGPUSkeleton parent;
};

struct _IgsFrequencyOverrideClass
{
  IgsComIntelGPUSkeletonClass parent_class;
};

GType igs_frequency_override_get_type(void) G_GNUC_CONST;

IgsFrequencyOverride *igs_frequency_override_new(IgsServer *server,
                                                 int device_id,
                                                 GDBusConnection *dbus_connection,
                                                 const char *dbus_sender);

const char *igs_frequency_override_get_dbus_path(IgsFrequencyOverride *override);
const char *igs_frequency_override_get_dbus_owner(IgsFrequencyOverride *override);

G_END_DECLS

#endif /* __IGS_FREQUENCY_OVERRIDE_H__ */
