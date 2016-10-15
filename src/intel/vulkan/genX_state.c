/*
 * Copyright © 2015 Intel Corporation
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

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "anv_private.h"
#include "vk_format_info.h"

#include "common/gen_sample_positions.h"
#include "genxml/gen_macros.h"
#include "genxml/genX_pack.h"

static uint32_t
border_color_index(VkBorderColor border_color, VkFormat format)
{
#if GEN_IS_HASWELL
   if (!vk_format_is_integer(format))
      return border_color;

   uint32_t max_bpc = vk_format_max_bpc(format);
   uint32_t index = 0;

   if (max_bpc <= 8)
      return border_color;

   if (max_bpc <= 16)
      index = VK_BORDER_COLOR_END_RANGE + 1;
   else
      index = VK_BORDER_COLOR_END_RANGE + 4;

   switch (border_color) {
   case VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
   case VK_BORDER_COLOR_INT_TRANSPARENT_BLACK:
      index += 0;
      break;

   case VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK:
   case VK_BORDER_COLOR_INT_OPAQUE_BLACK:
      index += 1;
      break;

   case VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE:
   case VK_BORDER_COLOR_INT_OPAQUE_WHITE:
      index += 2;
      break;

   default:
      unreachable("invalid border color");
   }

   return index;
#else
   return border_color;
#endif
}

#define BORDER_COLOR(name, r, g, b, a) {           \
      .BorderColor##name##Red   = r,               \
      .BorderColor##name##Green = g,               \
      .BorderColor##name##Blue  = b,               \
      .BorderColor##name##Alpha = a,               \
   }

void
genX(border_colors_setup)(struct anv_device *device)
{
#if GEN_IS_HASWELL
   static const struct GENX(SAMPLER_BORDER_COLOR_STATE) border_colors[] = {
      [VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK] =
         BORDER_COLOR(Float, 0.0, 0.0, 0.0, 0.0),
      [VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK] =
         BORDER_COLOR(Float, 0.0, 0.0, 0.0, 1.0),
      [VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE] =
         BORDER_COLOR(Float, 1.0, 1.0, 1.0, 1.0),
      [VK_BORDER_COLOR_INT_TRANSPARENT_BLACK] =
         BORDER_COLOR(8bit, 0, 0, 0, 0),
      [VK_BORDER_COLOR_INT_OPAQUE_BLACK] =
         BORDER_COLOR(8bit, 0, 0, 0, 1),
      [VK_BORDER_COLOR_INT_OPAQUE_WHITE] =
         BORDER_COLOR(8bit, 1, 1, 1, 1),
      [VK_BORDER_COLOR_END_RANGE + 1] =
         BORDER_COLOR(16bit, 0, 0, 0, 0),
      [VK_BORDER_COLOR_END_RANGE + 2] =
         BORDER_COLOR(16bit, 0, 0, 0, 1),
      [VK_BORDER_COLOR_END_RANGE + 3] =
         BORDER_COLOR(16bit, 1, 1, 1, 1),
      [VK_BORDER_COLOR_END_RANGE + 4] =
         BORDER_COLOR(32bit, 0, 0, 0, 0),
      [VK_BORDER_COLOR_END_RANGE + 5] =
         BORDER_COLOR(32bit, 0, 0, 0, 1),
      [VK_BORDER_COLOR_END_RANGE + 6] =
         BORDER_COLOR(32bit, 1, 1, 1, 1)
   };
   device->border_color_align = 512;
#elif GEN_GEN == 7
   static const struct GENX(SAMPLER_BORDER_COLOR_STATE) border_colors[] = {
      [VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK] =
         BORDER_COLOR(Float, 0.0, 0.0, 0.0, 0.0),
      [VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK] =
         BORDER_COLOR(Float, 0.0, 0.0, 0.0, 1.0),
      [VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE] =
         BORDER_COLOR(Float, 1.0, 1.0, 1.0, 1.0),
      [VK_BORDER_COLOR_INT_TRANSPARENT_BLACK] =
         BORDER_COLOR(Float, 0.0, 0.0, 0.0, 0.0),
      [VK_BORDER_COLOR_INT_OPAQUE_BLACK] =
         BORDER_COLOR(Float, 0.0, 0.0, 0.0, 1.0),
      [VK_BORDER_COLOR_INT_OPAQUE_WHITE] =
         BORDER_COLOR(Float, 1.0, 1.0, 1.0, 1.0)
   };
   device->border_color_align = 64;
#else /* GEN_GEN >= 8 */
   static const struct GENX(SAMPLER_BORDER_COLOR_STATE) border_colors[] = {
      [VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK] =
         BORDER_COLOR(Float, 0.0, 0.0, 0.0, 0.0),
      [VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK] =
         BORDER_COLOR(Float, 0.0, 0.0, 0.0, 1.0),
      [VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE] =
         BORDER_COLOR(Float, 1.0, 1.0, 1.0, 1.0),
      [VK_BORDER_COLOR_INT_TRANSPARENT_BLACK] =
         BORDER_COLOR(32bit, 0, 0, 0, 0),
      [VK_BORDER_COLOR_INT_OPAQUE_BLACK] =
         BORDER_COLOR(32bit, 0, 0, 0, 1),
      [VK_BORDER_COLOR_INT_OPAQUE_WHITE] =
         BORDER_COLOR(32bit, 1, 1, 1, 1)
   };
   device->border_color_align = 64;
#endif

   device->border_colors =
      anv_state_pool_alloc(&device->dynamic_state_pool,
                           ARRAY_SIZE(border_colors) * device->border_color_align,
                           device->border_color_align);

   for (uint32_t i = 0; i < ARRAY_SIZE(border_colors); i++) {
      GENX(SAMPLER_BORDER_COLOR_STATE_pack)(
         NULL,
         device->border_colors.map + i * device->border_color_align,
         &border_colors[i]);
   }

   if (!device->info.has_llc)
      anv_state_clflush(device->border_colors);
}

VkResult
genX(init_device_state)(struct anv_device *device)
{
   GENX(MEMORY_OBJECT_CONTROL_STATE_pack)(NULL, &device->default_mocs,
                                          &GENX(MOCS));

   struct anv_batch batch;

   uint32_t cmds[64];
   batch.start = batch.next = cmds;
   batch.end = (void *) cmds + sizeof(cmds);

   anv_batch_emit(&batch, GENX(PIPELINE_SELECT), ps) {
#if GEN_GEN >= 9
      ps.MaskBits = 3;
#endif
      ps.PipelineSelection = _3D;
   }

   anv_batch_emit(&batch, GENX(3DSTATE_VF_STATISTICS), vfs)
      vfs.StatisticsEnable = true;

   anv_batch_emit(&batch, GENX(3DSTATE_HS), hs);
   anv_batch_emit(&batch, GENX(3DSTATE_TE), ts);
   anv_batch_emit(&batch, GENX(3DSTATE_DS), ds);

   anv_batch_emit(&batch, GENX(3DSTATE_AA_LINE_PARAMETERS), aa);

   anv_batch_emit(&batch, GENX(3DSTATE_DRAWING_RECTANGLE), rect) {
      rect.ClippedDrawingRectangleYMin = 0;
      rect.ClippedDrawingRectangleXMin = 0;
      rect.ClippedDrawingRectangleYMax = UINT16_MAX;
      rect.ClippedDrawingRectangleXMax = UINT16_MAX;
      rect.DrawingRectangleOriginY = 0;
      rect.DrawingRectangleOriginX = 0;
   }

#if GEN_GEN >= 8
   anv_batch_emit(&batch, GENX(3DSTATE_WM_CHROMAKEY), ck);

   /* See the Vulkan 1.0 spec Table 24.1 "Standard sample locations" and
    * VkPhysicalDeviceFeatures::standardSampleLocations.
    */
   anv_batch_emit(&batch, GENX(3DSTATE_SAMPLE_PATTERN), sp) {
      GEN_SAMPLE_POS_1X(sp._1xSample);
      GEN_SAMPLE_POS_2X(sp._2xSample);
      GEN_SAMPLE_POS_4X(sp._4xSample);
      GEN_SAMPLE_POS_8X(sp._8xSample);
#if GEN_GEN >= 9
      GEN_SAMPLE_POS_16X(sp._16xSample);
#endif
   }
#endif

   anv_batch_emit(&batch, GENX(MI_BATCH_BUFFER_END), bbe);

   assert(batch.next <= batch.end);

   return anv_device_submit_simple_batch(device, &batch);
}

static inline uint32_t
vk_to_gen_tex_filter(VkFilter filter, bool anisotropyEnable)
{
   switch (filter) {
   default:
      assert(!"Invalid filter");
   case VK_FILTER_NEAREST:
      return MAPFILTER_NEAREST;
   case VK_FILTER_LINEAR:
      return anisotropyEnable ? MAPFILTER_ANISOTROPIC : MAPFILTER_LINEAR;
   }
}

static inline uint32_t
vk_to_gen_max_anisotropy(float ratio)
{
   return (anv_clamp_f(ratio, 2, 16) - 2) / 2;
}

static const uint32_t vk_to_gen_mipmap_mode[] = {
   [VK_SAMPLER_MIPMAP_MODE_NEAREST]          = MIPFILTER_NEAREST,
   [VK_SAMPLER_MIPMAP_MODE_LINEAR]           = MIPFILTER_LINEAR
};

static const uint32_t vk_to_gen_tex_address[] = {
   [VK_SAMPLER_ADDRESS_MODE_REPEAT]          = TCM_WRAP,
   [VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT] = TCM_MIRROR,
   [VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE]   = TCM_CLAMP,
   [VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE] = TCM_MIRROR_ONCE,
   [VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER] = TCM_CLAMP_BORDER,
};

/* Vulkan specifies the result of shadow comparisons as:
 *     1     if   ref <op> texel,
 *     0     otherwise.
 *
 * The hardware does:
 *     0     if texel <op> ref,
 *     1     otherwise.
 *
 * So, these look a bit strange because there's both a negation
 * and swapping of the arguments involved.
 */
static const uint32_t vk_to_gen_shadow_compare_op[] = {
   [VK_COMPARE_OP_NEVER]                        = PREFILTEROPALWAYS,
   [VK_COMPARE_OP_LESS]                         = PREFILTEROPLEQUAL,
   [VK_COMPARE_OP_EQUAL]                        = PREFILTEROPNOTEQUAL,
   [VK_COMPARE_OP_LESS_OR_EQUAL]                = PREFILTEROPLESS,
   [VK_COMPARE_OP_GREATER]                      = PREFILTEROPGEQUAL,
   [VK_COMPARE_OP_NOT_EQUAL]                    = PREFILTEROPEQUAL,
   [VK_COMPARE_OP_GREATER_OR_EQUAL]             = PREFILTEROPGREATER,
   [VK_COMPARE_OP_ALWAYS]                       = PREFILTEROPNEVER,
};

#if GEN_IS_HASWELL
void
#else
static void
#endif
genX(pack_sampler_state)(
   struct anv_device *                          device,
   struct anv_sampler *                         sampler,
   VkFormat                                     format)
{
   uint32_t color_index =
      border_color_index(sampler->info.borderColor, format);
   uint32_t color_offset =
      device->border_colors.offset +
      color_index * device->border_color_align;

   struct GENX(SAMPLER_STATE) sampler_state = {
      .SamplerDisable = false,
      .TextureBorderColorMode = DX10OGL,

#if GEN_GEN >= 8
      .LODPreClampMode = CLAMP_MODE_OGL,
#else
      .LODPreClampEnable = CLAMP_ENABLE_OGL,
#endif

#if GEN_GEN == 8
      .BaseMipLevel = 0.0,
#endif
      .MipModeFilter = vk_to_gen_mipmap_mode[sampler->info.mipmapMode],
      .MagModeFilter = vk_to_gen_tex_filter(sampler->info.magFilter,
                                            sampler->info.anisotropyEnable),
      .MinModeFilter = vk_to_gen_tex_filter(sampler->info.minFilter,
                                            sampler->info.anisotropyEnable),
      .TextureLODBias = anv_clamp_f(sampler->info.mipLodBias, -16, 15.996),
      .AnisotropicAlgorithm = EWAApproximation,
      .MinLOD = anv_clamp_f(sampler->info.minLod, 0, 14),
      .MaxLOD = anv_clamp_f(sampler->info.maxLod, 0, 14),
      .ChromaKeyEnable = 0,
      .ChromaKeyIndex = 0,
      .ChromaKeyMode = 0,
      .ShadowFunction = vk_to_gen_shadow_compare_op[sampler->info.compareOp],
      .CubeSurfaceControlMode = OVERRIDE,

      .BorderColorPointer = color_offset,

#if GEN_GEN >= 8
      .LODClampMagnificationMode = MIPNONE,
#endif

      .MaximumAnisotropy = vk_to_gen_max_anisotropy(sampler->info.maxAnisotropy),
      .RAddressMinFilterRoundingEnable = 0,
      .RAddressMagFilterRoundingEnable = 0,
      .VAddressMinFilterRoundingEnable = 0,
      .VAddressMagFilterRoundingEnable = 0,
      .UAddressMinFilterRoundingEnable = 0,
      .UAddressMagFilterRoundingEnable = 0,
      .TrilinearFilterQuality = 0,
      .NonnormalizedCoordinateEnable = sampler->info.unnormalizedCoordinates,
      .TCXAddressControlMode = vk_to_gen_tex_address[sampler->info.addressModeU],
      .TCYAddressControlMode = vk_to_gen_tex_address[sampler->info.addressModeV],
      .TCZAddressControlMode = vk_to_gen_tex_address[sampler->info.addressModeW],
   };

   GENX(SAMPLER_STATE_pack)(NULL, sampler->state, &sampler_state);
}


VkResult genX(CreateSampler)(
    VkDevice                                    _device,
    const VkSamplerCreateInfo*                  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSampler*                                  pSampler)
{
   ANV_FROM_HANDLE(anv_device, device, _device);
   struct anv_sampler *sampler;

   assert(pCreateInfo->sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);

   sampler = anv_alloc2(&device->alloc, pAllocator, sizeof(*sampler), 8,
                        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (!sampler)
      return vk_error(VK_ERROR_OUT_OF_HOST_MEMORY);

   sampler->info = *pCreateInfo;

   /* No need to pack the sampler state on HSW, as the packing will depend on
    * the format of the associated texture. */
#if ! GEN_IS_HASWELL
   genX(pack_sampler_state)(device, sampler, VK_FORMAT_UNDEFINED);
#endif

   *pSampler = anv_sampler_to_handle(sampler);

   return VK_SUCCESS;
}
