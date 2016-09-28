/*
 * Copyright © 2011 Intel Corporation
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

#include "intel_batchbuffer.h"
#include "intel_mipmap_tree.h"

#include "brw_context.h"
#include "brw_state.h"
#include "brw_util.h"

#include "main/enums.h"
#include "main/glformats.h"

#include "genX_util.h"
#include "genxml/gen_macros.h"
#include "genxml/genX_pack.h"

static void
upload_blend_state(struct brw_context *brw)
{
   bool is_buffer_zero_integer_format = false;
   struct gl_context *ctx = &brw->ctx;
   int b;
   int nr_draw_buffers = ctx->DrawBuffer->_NumColorDrawBuffers;
   struct GENX(BLEND_STATE) blend_state = {
      /* Make sure it gets zeroed */
      .Entry = { { 0, }, },
   };

#if GEN_GEN <= 7
   /* We need at least one BLEND_STATE written, because we might do
    * thread dispatch even if _NumColorDrawBuffers is 0 (for example
    * for computed depth or alpha test), which will do an FB write
    * with render target 0, which will reference BLEND_STATE[0] for
    * alpha test enable.
    */
   if (nr_draw_buffers == 0)
      nr_draw_buffers = 1;
#else
   if (nr_draw_buffers == 0 && ctx->Color.AlphaEnabled)
      nr_draw_buffers = 1;
#endif /* GEN_GEN <= 7 */

   /* Default everything to disabled */
   for (b = 0; b < ARRAY_SIZE(blend_state.Entry); b++) {
      blend_state.Entry[b].WriteDisableAlpha = true;
      blend_state.Entry[b].WriteDisableRed = true;
      blend_state.Entry[b].WriteDisableGreen = true;
      blend_state.Entry[b].WriteDisableBlue = true;
   }

#if GEN_GEN >= 8
   if (_mesa_is_multisample_enabled(ctx)) {
      blend_state.AlphaToCoverageEnable = ctx->Multisample.SampleAlphaToCoverage;
      blend_state.AlphaToCoverageDitherEnable = ctx->Multisample.SampleAlphaToCoverage;
      blend_state.AlphaToOneEnable = ctx->Multisample.SampleAlphaToOne;
   }

   /* OpenGL specification 3.3 (page 196), section 4.1.3 says:
    * "If drawbuffer zero is not NONE and the buffer it references has an
    * integer format, the SAMPLE_ALPHA_TO_COVERAGE and SAMPLE_ALPHA_TO_ONE
    * operations are skipped."
    */
   struct gl_renderbuffer *rb0 = ctx->DrawBuffer->_ColorDrawBuffers[0];
   GLenum rb_zero_type =
      rb0 ? _mesa_get_format_datatype(rb0->Format) : GL_UNSIGNED_NORMALIZED;

   if (rb_zero_type != GL_INT && rb_zero_type != GL_UNSIGNED_INT) {
      /* _NEW_MULTISAMPLE */
      if (_mesa_is_multisample_enabled(ctx)) {
         blend_state.AlphaToCoverageEnable =
            blend_state.AlphaToCoverageDitherEnable =
            ctx->Multisample.SampleAlphaToCoverage;
         blend_state.AlphaToOneEnable = ctx->Multisample.SampleAlphaToOne;
      }

      /* _NEW_COLOR */
      if (ctx->Color.AlphaEnabled) {
         blend_state.AlphaTestEnable = true;
         blend_state.AlphaTestFunction =
            intel_translate_compare_func(ctx->Color.AlphaFunc);
      }

      if (ctx->Color.DitherFlag) {
         blend_state.ColorDitherEnable = true;
      }
   }
#endif /* GEN_GEN >= 8 */

   for (b = 0; b < nr_draw_buffers; b++) {
      struct GENX(BLEND_STATE_ENTRY) *bse = &blend_state.Entry[b];

      /* _NEW_BUFFERS */
      struct gl_renderbuffer *rb = ctx->DrawBuffer->_ColorDrawBuffers[b];
      GLenum rb_type;
      bool integer;

      if (rb)
	 rb_type = _mesa_get_format_datatype(rb->Format);
      else
	 rb_type = GL_UNSIGNED_NORMALIZED;

      /* Used for implementing the following bit of GL_EXT_texture_integer:
       *     "Per-fragment operations that require floating-point color
       *      components, including multisample alpha operations, alpha test,
       *      blending, and dithering, have no effect when the corresponding
       *      colors are written to an integer color buffer."
      */
      integer = (rb_type == GL_INT || rb_type == GL_UNSIGNED_INT);

      if(b == 0 && integer)
         is_buffer_zero_integer_format = true;

      /* _NEW_COLOR */
      if (ctx->Color.ColorLogicOpEnabled) {
	 /* Floating point RTs should have no effect from LogicOp,
	  * except for disabling of blending, but other types should.
	  *
	  * However, from the Sandy Bridge PRM, Vol 2 Par 1, Section 8.1.11,
	  * "Logic Ops",
	  *
	  *     "Logic Ops are only supported on *_UNORM surfaces (excluding
	  *      _SRGB variants), otherwise Logic Ops must be DISABLED."
	  */
         WARN_ONCE(ctx->Color.LogicOp != GL_COPY &&
                   rb_type != GL_UNSIGNED_NORMALIZED &&
                   rb_type != GL_FLOAT, "Ignoring %s logic op on %s "
                   "renderbuffer\n",
                   _mesa_enum_to_string(ctx->Color.LogicOp),
                   _mesa_enum_to_string(rb_type));
	 if (rb_type == GL_UNSIGNED_NORMALIZED) {
	    bse->LogicOpEnable = true;
	    bse->LogicOpFunction =
	       intel_translate_logic_op(ctx->Color.LogicOp);
	 }
      } else if (ctx->Color.BlendEnabled & (1 << b) && !integer &&
                 !ctx->Color._AdvancedBlendMode) {
	 GLenum eqRGB = ctx->Color.Blend[b].EquationRGB;
	 GLenum eqA = ctx->Color.Blend[b].EquationA;
	 GLenum srcRGB = ctx->Color.Blend[b].SrcRGB;
	 GLenum dstRGB = ctx->Color.Blend[b].DstRGB;
	 GLenum srcA = ctx->Color.Blend[b].SrcA;
	 GLenum dstA = ctx->Color.Blend[b].DstA;

	 if (eqRGB == GL_MIN || eqRGB == GL_MAX) {
	    srcRGB = dstRGB = GL_ONE;
	 }

	 if (eqA == GL_MIN || eqA == GL_MAX) {
	    srcA = dstA = GL_ONE;
	 }

         /* Due to hardware limitations, the destination may have information
          * in an alpha channel even when the format specifies no alpha
          * channel. In order to avoid getting any incorrect blending due to
          * that alpha channel, coerce the blend factors to values that will
          * not read the alpha channel, but will instead use the correct
          * implicit value for alpha.
          */
         if (rb && !_mesa_base_format_has_channel(rb->_BaseFormat, GL_TEXTURE_ALPHA_TYPE))
         {
            srcRGB = brw_fix_xRGB_alpha(srcRGB);
            srcA = brw_fix_xRGB_alpha(srcA);
            dstRGB = brw_fix_xRGB_alpha(dstRGB);
            dstA = brw_fix_xRGB_alpha(dstA);
         }

	 bse->DestinationBlendFactor = brw_translate_blend_factor(dstRGB);
	 bse->SourceBlendFactor = brw_translate_blend_factor(srcRGB);
         bse->ColorBlendFunction = brw_translate_blend_equation(eqRGB);

         bse->DestinationAlphaBlendFactor = brw_translate_blend_factor(dstA);
	 bse->SourceAlphaBlendFactor = brw_translate_blend_factor(srcA);
	 bse->AlphaBlendFunction = brw_translate_blend_equation(eqA);

         bse->ColorBufferBlendEnable = true;
#if GEN_GEN <= 7
         bse->IndependentAlphaBlendEnable = (srcA != srcRGB ||
                                             dstA != dstRGB ||
                                             eqA != eqRGB);
#else
         blend_state.IndependentAlphaBlendEnable |= (srcA != srcRGB ||
                                                     dstA != dstRGB ||
                                                     eqA != eqRGB);
#endif /* GEN_GEN <= 7 */
      }

      /* See section 8.1.6 "Pre-Blend Color Clamping" of the
       * SandyBridge PRM Volume 2 Part 1 for HW requirements.
       *
       * We do our ARB_color_buffer_float CLAMP_FRAGMENT_COLOR
       * clamping in the fragment shader.  For its clamping of
       * blending, the spec says:
       *
       *     "RESOLVED: For fixed-point color buffers, the inputs and
       *      the result of the blending equation are clamped.  For
       *      floating-point color buffers, no clamping occurs."
       *
       * So, generally, we want clamping to the render target's range.
       * And, good news, the hardware tables for both pre- and
       * post-blend color clamping are either ignored, or any are
       * allowed, or clamping is required but RT range clamping is a
       * valid option.
       */
      bse->PreBlendColorClampEnable = true;
      bse->PostBlendColorClampEnable = true;
      bse->ColorClampRange = COLORCLAMP_RTFORMAT;

#if GEN_GEN <= 7
      /* _NEW_COLOR */
      if (ctx->Color.AlphaEnabled && !integer) {
         bse->AlphaTestEnable = true;
         bse->AlphaBlendFunction =
	    intel_translate_compare_func(ctx->Color.AlphaFunc);
      }

      /* _NEW_COLOR */
      if (ctx->Color.DitherFlag && !integer) {
         bse->ColorDitherEnable = true;
	 bse->XDitherOffset = 0;
         bse->YDitherOffset = 0;
      }

      bse->WriteDisableRed   = !ctx->Color.ColorMask[b][0];
      bse->WriteDisableGreen = !ctx->Color.ColorMask[b][1];
      bse->WriteDisableBlue  = !ctx->Color.ColorMask[b][2];
      bse->WriteDisableAlpha = !ctx->Color.ColorMask[b][3];

      /* OpenGL specification 3.3 (page 196), section 4.1.3 says:
       * "If drawbuffer zero is not NONE and the buffer it references has an
       * integer format, the SAMPLE_ALPHA_TO_COVERAGE and SAMPLE_ALPHA_TO_ONE
       * operations are skipped."
       */
      if(!is_buffer_zero_integer_format) {
         /* _NEW_MULTISAMPLE */
         bse->AlphaToCoverageEnable =
            _mesa_is_multisample_enabled(ctx) && ctx->Multisample.SampleAlphaToCoverage;

	/* From SandyBridge PRM, volume 2 Part 1, section 8.2.3, BLEND_STATE:
	 * DWord 1, Bit 30 (AlphaToOne Enable):
	 * "If Dual Source Blending is enabled, this bit must be disabled"
	 */
         WARN_ONCE(ctx->Color.Blend[b]._UsesDualSrc &&
                   _mesa_is_multisample_enabled(ctx) &&
                   ctx->Multisample.SampleAlphaToOne,
                   "HW workaround: disabling alpha to one with dual src "
                   "blending\n");
	 if (ctx->Color.Blend[b]._UsesDualSrc)
            bse->AlphaToOneEnable = false;
	 else
            bse->AlphaToOneEnable =
	       _mesa_is_multisample_enabled(ctx) && ctx->Multisample.SampleAlphaToOne;

         bse->AlphaToCoverageDitherEnable = (brw->gen >= 7);
      }
      else {
         bse->AlphaToOneEnable = false;
         bse->AlphaToCoverageEnable = false;
      }
#endif /* GEN_GEN <= 7 */
   }

   uint32_t blend_state_offset = 0;
   void *map = brw_state_batch(brw, AUB_TRACE_BLEND_STATE,
                               GENX(BLEND_STATE_length), 64,
                               &blend_state_offset);

   GENX(BLEND_STATE_pack)(NULL, map, &blend_state);

   /* Point the GPU at the new indirect state. */
#if GEN_GEN == 6
   I965_BATCH_EMIT(GENX(3DSTATE_CC_STATE_POINTERS), ccp) {
      ccp.PointertoBLEND_STATE = blend_state_offset;
      ccp.BLEND_STATEChange = true;
   }
#else
   I965_BATCH_EMIT(GENX(3DSTATE_BLEND_STATE_POINTERS), bsp) {
      bsp.BlendStatePointer = blend_state_offset;
#if GEN_GEN >= 8
      bsp.BlendStatePointerValid = true;
#endif /* GEN_GEN >= 8 */
   }
#endif /* GEN_GEN == 6 */
}

const struct brw_tracked_state genX(blend_state) = {
   .dirty = {
      .mesa = _NEW_BUFFERS |
              _NEW_COLOR |
              _NEW_MULTISAMPLE,
      .brw = BRW_NEW_BATCH |
             BRW_NEW_BLORP |
             BRW_NEW_STATE_BASE_ADDRESS,
   },
   .emit = upload_blend_state,
};
