/*
 * Copyright 2017 Intel Corporation
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice (including the next
 *  paragraph) shall be included in all copies or substantial portions of the
 *  Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include <assert.h>
#include <stdlib.h>

#include "drm-uapi/drm_fourcc.h"
#include "drm-uapi/i915_drm.h"

#include "isl.h"
#include "isl_gen4.h"
#include "isl_gen7.h"
#include "dev/gen_debug.h"
#include "dev/gen_device_info.h"
#include "util/u_math.h"

uint32_t
isl_tiling_to_i915_tiling(enum isl_tiling tiling)
{
   switch (tiling) {
   case ISL_TILING_LINEAR:
      return I915_TILING_NONE;

   case ISL_TILING_X:
      return I915_TILING_X;

   case ISL_TILING_Y0:
   case ISL_TILING_HIZ:
   case ISL_TILING_CCS:
      return I915_TILING_Y;

   case ISL_TILING_W:
   case ISL_TILING_Yf:
   case ISL_TILING_Ys:
   case ISL_TILING_GEN12_CCS:
      return I915_TILING_NONE;
   }

   unreachable("Invalid ISL tiling");
}

enum isl_tiling
isl_tiling_from_i915_tiling(uint32_t tiling)
{
   switch (tiling) {
   case I915_TILING_NONE:
      return ISL_TILING_LINEAR;

   case I915_TILING_X:
      return ISL_TILING_X;

   case I915_TILING_Y:
      return ISL_TILING_Y0;
   }

   unreachable("Invalid i915 tiling");
}

static const struct isl_drm_modifier_info modifier_info[] = {
   {
      .modifier = DRM_FORMAT_MOD_NONE,
      .name = "DRM_FORMAT_MOD_NONE",
      .tiling = ISL_TILING_LINEAR,
   },
   {
      .modifier = I915_FORMAT_MOD_X_TILED,
      .name = "I915_FORMAT_MOD_X_TILED",
      .tiling = ISL_TILING_X,
   },
   {
      .modifier = I915_FORMAT_MOD_Y_TILED,
      .name = "I915_FORMAT_MOD_Y_TILED",
      .tiling = ISL_TILING_Y0,
   },
   {
      .modifier = I915_FORMAT_MOD_Y_TILED_CCS,
      .name = "I915_FORMAT_MOD_Y_TILED_CCS",
      .tiling = ISL_TILING_Y0,
      .aux_usage = ISL_AUX_USAGE_CCS_E,
      .supports_clear_color = false,
   },
};

const struct isl_drm_modifier_info *
isl_drm_modifier_get_info(uint64_t modifier)
{
   for (unsigned i = 0; i < ARRAY_SIZE(modifier_info); i++) {
      if (modifier_info[i].modifier == modifier)
         return &modifier_info[i];
   }

   return NULL;
}

bool
isl_format_supports_drm_modifier_s(const struct isl_device *dev,
                                   const struct isl_surf_init_info *restrict info,
                                   uint64_t modifier)
{
   switch (modifier) {
   case DRM_FORMAT_MOD_LINEAR:
   case I915_FORMAT_MOD_X_TILED:
      return true;
   case I915_FORMAT_MOD_Y_TILED:
      return ISL_DEV_GEN(dev) >= 6;
   case I915_FORMAT_MOD_Y_TILED_CCS: {
      if (unlikely(INTEL_DEBUG & DEBUG_NO_RBC))
         return false;

      /* No CCS on ASTC, ETC, BC, etc... */
      if (isl_format_is_compressed(info->format))
         return false;

      /* No CCSS for depth formats. */
      if (info->usage & (ISL_SURF_USAGE_DEPTH_BIT |
                         ISL_SURF_USAGE_STENCIL_BIT))
         return false;

      /* No non power of 2 formats. */
      const struct isl_format_layout *fmtl = isl_format_get_layout(info->format);
      uint32_t bits = 0;
      for (uint32_t c = 0; c < ARRAY_SIZE(fmtl->channels_array); c++)
         bits += fmtl->channels_array[c].bits;

      if (!util_next_power_of_two(bits))
         return false;

      enum isl_format linear_format = isl_format_srgb_to_linear(info->format);
      if (linear_format == ISL_FORMAT_UNSUPPORTED ||
          !isl_format_supports_ccs_e(dev->info, linear_format))
         return false;

      /* The format of the CCS has changed on Gen12. */
      return ISL_DEV_GEN(dev) >= 9 && ISL_DEV_GEN(dev) <= 11;
   }
   default:
      return false;
   }
}

enum modifier_priority {
   MODIFIER_PRIORITY_INVALID = 0,
   MODIFIER_PRIORITY_LINEAR,
   MODIFIER_PRIORITY_X,
   MODIFIER_PRIORITY_Y,
   MODIFIER_PRIORITY_Y_CCS,
};

static const uint64_t priority_to_modifier[] = {
   [MODIFIER_PRIORITY_INVALID] = DRM_FORMAT_MOD_INVALID,
   [MODIFIER_PRIORITY_LINEAR] = DRM_FORMAT_MOD_LINEAR,
   [MODIFIER_PRIORITY_X] = I915_FORMAT_MOD_X_TILED,
   [MODIFIER_PRIORITY_Y] = I915_FORMAT_MOD_Y_TILED,
   [MODIFIER_PRIORITY_Y_CCS] = I915_FORMAT_MOD_Y_TILED_CCS,
};

uint64_t
isl_drm_modifier_select_s(const struct isl_device *dev,
                          const struct isl_surf_init_info *restrict info,
                          const uint64_t *modifiers,
                          uint32_t n_modifiers)
{
   enum modifier_priority prio = MODIFIER_PRIORITY_INVALID;

   for (int i = 0; i < n_modifiers; i++) {
      if (!isl_format_supports_drm_modifier_s(dev, info, modifiers[i]))
         continue;

      const struct isl_drm_modifier_info *mod_info =
         isl_drm_modifier_get_info(modifiers[i]);
      assert(mod_info);

      isl_tiling_flags_t tiling_flags = 1u << mod_info->tiling;
      if (ISL_DEV_GEN(dev) >= 6) {
         isl_gen6_filter_tiling(dev, info, &tiling_flags);
      } else {
         isl_gen4_filter_tiling(dev, info, &tiling_flags);
      }

      /* It's possible that given a set of usages we do not support a
       * particular modifier. For instance TILING_Y is not available on
       * Gen8 for display usage.
       */
      if (!tiling_flags)
         continue;

      switch (modifiers[i]) {
      case I915_FORMAT_MOD_Y_TILED_CCS:
         prio = MAX2(prio, MODIFIER_PRIORITY_Y_CCS);
         break;
      case I915_FORMAT_MOD_Y_TILED:
         prio = MAX2(prio, MODIFIER_PRIORITY_Y);
         break;
      case I915_FORMAT_MOD_X_TILED:
         prio = MAX2(prio, MODIFIER_PRIORITY_X);
         break;
      case DRM_FORMAT_MOD_LINEAR:
         prio = MAX2(prio, MODIFIER_PRIORITY_LINEAR);
         break;
      case DRM_FORMAT_MOD_INVALID:
      default:
         break;
      }
   }

   return priority_to_modifier[prio];
}
