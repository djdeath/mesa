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

#include <xf86drm.h>
#include <i915_drm.h>

#include "util/macros.h"
#include "igs_client.h"

#define FILE_DEBUG_FLAG DEBUG_PERFMON

int
igs_client_acquire_perf(int drm_fd,
                        int metrics_set_id,
                        int report_format,
                        int period_exponent,
                        uint32_t ctx_id,
                        uint64_t timeout_ms)
{
   uint64_t properties[] = {
      /* Single context sampling */
      DRM_I915_PERF_PROP_CTX_HANDLE, ctx_id,

      /* Include OA reports in samples */
      DRM_I915_PERF_PROP_SAMPLE_OA, true,

      /* OA unit configuration */
      DRM_I915_PERF_PROP_OA_METRICS_SET, metrics_set_id,
      DRM_I915_PERF_PROP_OA_FORMAT, report_format,
      DRM_I915_PERF_PROP_OA_EXPONENT, period_exponent,
   };
   struct drm_i915_perf_open_param param = {
      .flags = I915_PERF_FLAG_FD_CLOEXEC |
               I915_PERF_FLAG_FD_NONBLOCK |
               I915_PERF_FLAG_DISABLED,
      .num_properties = ARRAY_SIZE(properties) / 2,
      .properties_ptr = (uintptr_t) properties,
   };

   return drmIoctl(drm_fd, DRM_IOCTL_I915_PERF_OPEN, &param);
}

bool
igs_client_can_register_configurations(int drm_fd)
{
   uint64_t config_id;

   return drmIoctl(drm_fd, DRM_IOCTL_I915_PERF_REMOVE_CONFIG, &config_id) < 0 &&
      errno == ENOENT;
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
   struct drm_i915_perf_oa_config config;

   memset(&config, 0, sizeof(config));

   memcpy(config.uuid, query->guid, sizeof(config.uuid));

   config.n_mux_regs = query->n_mux_regs;
   config.mux_regs_ptr = (uintptr_t) query->mux_regs;

   config.n_boolean_regs = query->n_b_counter_regs;
   config.boolean_regs_ptr = (uintptr_t) query->b_counter_regs;

   config.n_flex_regs = query->n_flex_regs;
   config.flex_regs_ptr = (uintptr_t) query->flex_regs;

   int ret = drmIoctl(drm_fd, DRM_IOCTL_I915_PERF_ADD_CONFIG, &config);
   return ret <= 0 : 0 : ret;
}
