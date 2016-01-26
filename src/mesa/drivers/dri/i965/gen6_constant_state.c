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

#include "brw_context.h"
#include "brw_state.h"
#include "brw_defines.h"
#include "intel_batchbuffer.h"
#include "intel_buffer_objects.h"
#include "program/prog_parameter.h"

#define F(RELOC, BATCH, buf, x) \
   if (buf) { \
      RELOC(buf, I915_GEM_DOMAIN_RENDER, 0, x); \
   } else { \
      BATCH(x); \
   }
#define OUT_PTR64(buf, x) F(OUT_RELOC64, OUT_BATCH64, buf, x)
#define OUT_PTR(buf, x)   F(OUT_RELOC,   OUT_BATCH,   buf, x)

static inline void
emit_3dstate_constant(struct brw_context *brw,
                      uint32_t opcode,
                      uint32_t mocs,
                      drm_intel_bo *bufs[4],
                      uint16_t read_lengths[4],
                      uint64_t offsets[4])
{
   /* Buffer 0 is relative to Dynamic State Base Address, which we program
    * to the start of the batch buffer.  All others are graphics virtual
    * addresses regardless of the INSTPM settings.
    */
   assert(bufs[0] == NULL || bufs[0] == brw->batch.bo);

   assert(read_lengths[0] == 0 || bufs[0] != NULL);
   assert(read_lengths[1] == 0 || bufs[1] != NULL);
   assert(read_lengths[2] == 0 || bufs[2] != NULL);
   assert(read_lengths[3] == 0 || bufs[3] != NULL);

   if (brw->gen >= 8) {
      BEGIN_BATCH(11);
      OUT_BATCH(opcode << 16 | (11 - 2));
      OUT_BATCH(read_lengths[0] | read_lengths[1] << 16);
      OUT_BATCH(read_lengths[2] | read_lengths[3] << 16);
      OUT_BATCH64(offsets[0]);
      OUT_PTR64(bufs[1], offsets[1]);
      OUT_PTR64(bufs[2], offsets[2]);
      OUT_PTR64(bufs[3], offsets[3]);
      ADVANCE_BATCH();
   } else if (brw->gen == 7) {
      /* From the Ivybridge PRM, Volume 2, Part 1, Page 112:
       * "Constant buffers must be enabled in order from Constant Buffer 0 to
       *  Constant Buffer 3 within this command.  For example, it is not
       *  allowed to enable Constant Buffer 1 by programming a non-zero value
       *  in the VS Constant Buffer 1 Read Length without a non-zero value in
       *  VS Constant Buffer 0 Read Length."
       *
       * Haswell removes this restriction.
       */
      if (!brw->is_haswell) {
         assert(read_lengths[3] == 0 || (read_lengths[2] > 0 &&
                                         read_lengths[1] > 0 &&
                                         read_lengths[0] > 0));
         assert(read_lengths[2] == 0 || (read_lengths[1] > 0 &&
                                         read_lengths[0] > 0));
         assert(read_lengths[1] == 0 || read_lengths[0] > 0);
      }

      BEGIN_BATCH(7);
      OUT_BATCH(opcode << 16 | (7 - 2));
      OUT_BATCH(read_lengths[0] | read_lengths[1] << 16);
      OUT_BATCH(read_lengths[2] | read_lengths[3] << 16);
      OUT_BATCH(offsets[0]);
      OUT_PTR(bufs[1], offsets[1]);
      OUT_PTR(bufs[2], offsets[2]);
      OUT_PTR(bufs[3], offsets[3]);
      ADVANCE_BATCH();
   } else if (brw->gen == 6) {
      /* From the Sandybridge PRM, Volume 2, Part 1, Page 138:
       * "The sum of all four read length fields (each incremented to
       *  represent the actual read length) must be less than or equal
       *  to 32."
       */
      assert(read_lengths[0] + read_lengths[1] +
             read_lengths[2] + read_lengths[3] < 32);

      BEGIN_BATCH(5);
      OUT_BATCH(opcode << 16 | (5 - 2) |
                (read_lengths[0] ? GEN6_CONSTANT_BUFFER_0_ENABLE : 0) |
                (read_lengths[1] ? GEN6_CONSTANT_BUFFER_1_ENABLE : 0) |
                (read_lengths[2] ? GEN6_CONSTANT_BUFFER_2_ENABLE : 0) |
                (read_lengths[3] ? GEN6_CONSTANT_BUFFER_3_ENABLE : 0));
      OUT_BATCH(offsets[0] | (read_lengths[0] - 1));
      OUT_PTR(bufs[1], offsets[1] | (read_lengths[1] - 1));
      OUT_PTR(bufs[2], offsets[2] | (read_lengths[2] - 1));
      OUT_PTR(bufs[3], offsets[3] | (read_lengths[3] - 1));
      ADVANCE_BATCH();
   } else {
      unreachable("unhandled gen in emit_3dstate_constant");
   }
}

static void
gen7_upload_constant_state(struct brw_context *brw,
                           const struct brw_stage_state *stage_state,
                           bool active, unsigned opcode)
{
   struct gl_context *ctx = &brw->ctx;
   const struct gl_shader_program *shader_prog =
      ctx->_Shader->CurrentProgram[stage_state->stage];
   const struct gl_linked_shader *shader = shader_prog ?
      shader_prog->_LinkedShaders[stage_state->stage] : NULL;

   uint32_t mocs = brw->gen < 8 ? GEN7_MOCS_L3 : 0;

   drm_intel_bo *bufs[4] = { NULL, NULL, NULL, NULL };
   uint16_t read_lengths[4] = { 0, 0, 0, 0 };
   uint64_t offsets[4] = { 0, 0, 0, 0 };

   if (active) {
      /* XXX: Have to start at 1 because of INSTPM rubbish.
       * XXX: Won't work for IVB
       */
      int start_ubo = 1;

      if (stage_state->push_const_size > 0) {
         bufs[0] = brw->batch.bo;
         offsets[0] = stage_state->push_const_offset;
         read_lengths[0] = stage_state->push_const_size;
      }

      if (shader_prog) {
         for (int i = 0; i < 4; i++) {
            const struct brw_ubo_range *range =
               &stage_state->prog_data->ubo_ranges[i];

            if (range->length == 0)
               break;

            const struct gl_uniform_block *block =
               shader->UniformBlocks[range->block];
            const struct gl_uniform_buffer_binding *binding =
               &ctx->UniformBufferBindings[block->Binding];

            if (binding->BufferObject != ctx->Shared->NullBufferObj) {
               assert(binding->Offset % 32 == 0);
               offsets[start_ubo+i] = range->start * 32 + binding->Offset;
               read_lengths[start_ubo+i] = range->length;
               bufs[start_ubo+i] = intel_bufferobj_buffer(brw,
                  intel_buffer_object(binding->BufferObject),
                  binding->Offset, range->length * 32);
            } else {
               assert(!"XXX: UBO not bound?  What do we do?");
            }
         }
      }
   }

   /* XXX: SKL workaround */

   emit_3dstate_constant(brw, opcode, mocs, bufs, read_lengths, offsets);

   /* On SKL+ the new constants don't take effect until the next corresponding
    * 3DSTATE_BINDING_TABLE_POINTER_* command is parsed so we need to ensure
    * that is sent
    */
   if (brw->gen >= 9)
      brw->ctx.NewDriverState |= BRW_NEW_SURFACES;
}

static void
gen7_upload_push_constant_packets(struct brw_context *brw)
{
   const struct gen_device_info *devinfo = &brw->screen->devinfo;

   struct brw_stage_state *stage_states[] = {
      &brw->vs.base,
      &brw->tcs.base,
      &brw->tes.base,
      &brw->gs.base,
      &brw->wm.base,
   };

   bool active[] = {
      true,
      brw->tess_eval_program != NULL,
      brw->tess_eval_program != NULL,
      brw->geometry_program != NULL,
      true,
   };

   unsigned cmds[] = {
      _3DSTATE_CONSTANT_VS,
      _3DSTATE_CONSTANT_HS,
      _3DSTATE_CONSTANT_DS,
      _3DSTATE_CONSTANT_GS,
      _3DSTATE_CONSTANT_PS,
   };

   if (devinfo->is_ivybridge &&
       stage_states[MESA_SHADER_VERTEX]->push_constants_dirty) {
      gen7_emit_vs_workaround_flush(brw);
   }

   for (int i = 0; i <= MESA_SHADER_FRAGMENT; i++) {
      if (stage_states[i]->push_constants_dirty) {
         gen7_upload_constant_state(brw, stage_states[i], active[i], cmds[i]);
         stage_states[i]->push_constants_dirty = false;
      }
   }
}

const struct brw_tracked_state gen7_push_constant_packets = {
   .dirty = {
      .mesa  = 0,
      .brw   = BRW_NEW_DRAW_CALL,
   },
   .emit = gen7_upload_push_constant_packets,
};

/**
 * Creates a streamed BO containing the push constants for the VS or GS on
 * gen6+.
 *
 * Push constants are constant values (such as GLSL uniforms) that are
 * pre-loaded into a shader stage's register space at thread spawn time.
 *
 * Not all GLSL uniforms will be uploaded as push constants: The hardware has
 * a limitation of 32 or 64 EU registers (256 or 512 floats) per stage to be
 * uploaded as push constants, while GL 4.4 requires at least 1024 components
 * to be usable for the VS.  Plus, currently we always use pull constants
 * instead of push constants when doing variable-index array access.
 *
 * See brw_curbe.c for the equivalent gen4/5 code.
 */
void
gen6_upload_push_constants(struct brw_context *brw,
                           const struct gl_program *prog,
                           const struct brw_stage_prog_data *prog_data,
                           struct brw_stage_state *stage_state,
                           enum aub_state_struct_type type)
{
   struct gl_context *ctx = &brw->ctx;

   if (prog_data->nr_params == 0) {
      stage_state->push_const_size = 0;
   } else {
      /* Updates the ParamaterValues[i] pointers for all parameters of the
       * basic type of PROGRAM_STATE_VAR.
       */
      /* XXX: Should this happen somewhere before to get our state flag set? */
      if (prog)
         _mesa_load_state_parameters(ctx, prog->Parameters);

      gl_constant_value *param;
      int i;

      param = brw_state_batch(brw, type,
                              prog_data->nr_params * sizeof(gl_constant_value),
                              32, &stage_state->push_const_offset);

      STATIC_ASSERT(sizeof(gl_constant_value) == sizeof(float));

      /* _NEW_PROGRAM_CONSTANTS
       *
       * Also _NEW_TRANSFORM -- we may reference clip planes other than as a
       * side effect of dereferencing uniforms, so _NEW_PROGRAM_CONSTANTS
       * wouldn't be set for them.
       */
      for (i = 0; i < prog_data->nr_params; i++) {
         param[i] = *prog_data->param[i];
      }

      if (0) {
         fprintf(stderr, "%s constants:\n",
                 _mesa_shader_stage_to_string(stage_state->stage));
         for (i = 0; i < prog_data->nr_params; i++) {
            if ((i & 7) == 0)
               fprintf(stderr, "g%d: ",
                       prog_data->dispatch_grf_start_reg + i / 8);
            fprintf(stderr, "%8f ", param[i].f);
            if ((i & 7) == 7)
               fprintf(stderr, "\n");
         }
         if ((i & 7) != 0)
            fprintf(stderr, "\n");
         fprintf(stderr, "\n");
      }

      stage_state->push_const_size = ALIGN(prog_data->nr_params, 8) / 8;
      /* We can only push 32 registers of constants at a time. */

      /* From the SNB PRM (vol2, part 1, section 3.2.1.4: 3DSTATE_CONSTANT_VS:
       *
       *     "The sum of all four read length fields (each incremented to
       *      represent the actual read length) must be less than or equal to
       *      32"
       *
       * From the IVB PRM (vol2, part 1, section 3.2.1.3: 3DSTATE_CONSTANT_VS:
       *
       *     "The sum of all four read length fields must be less than or
       *      equal to the size of 64"
       *
       * The other shader stages all match the VS's limits.
       */
      assert(stage_state->push_const_size <= 32);
   }
}
