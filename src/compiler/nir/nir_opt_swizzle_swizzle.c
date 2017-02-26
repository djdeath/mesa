/*
 * Copyright © 2017 Intel Corporation
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
 *
 * Authors:
 *    Lionel Landwerlin (lionel.g.landwerlin@intel.com)
 *
 */

#include "nir.h"

/*
 * Turn:
 *
 * ssa_1 = ssa_0.zyx
 * ssa_2 = iadd ssa_1.yz, 42
 *
 * into:
 *
 * ssa_2 = iadd ssa_0.yx, 42
 */

bool
nir_opt_swizzle_swizzle(nir_shader *shader)
{
   bool progress = false;

   nir_foreach_function(function, shader) {
      if (function->impl == NULL)
         continue;

      bool local_progress = false;
      nir_foreach_block(block, function->impl) {
         nir_foreach_instr_safe(instr, block) {
            if (instr->type != nir_instr_type_alu)
               continue;

            nir_alu_instr *alu_instr = nir_instr_as_alu(instr);

            if (alu_instr->op != nir_op_fmov &&
                alu_instr->op != nir_op_imov)
               continue;

            if (alu_instr->src[0].negate ||
                alu_instr->src[0].abs)
               continue;

            nir_foreach_use_safe(child_src, &alu_instr->dest.dest.ssa) {
               if (!child_src->is_ssa)
                  continue;

               if (child_src->parent_instr->type != nir_instr_type_alu)
                  continue;

               nir_alu_instr *child_alu_instr =
                  nir_instr_as_alu(child_src->parent_instr);

               nir_alu_src *child_alu_src =
                  exec_node_data(nir_alu_src, child_src, src);

               if (child_alu_src->negate || child_alu_src->abs)
                  continue;

               nir_instr_rewrite_src(&child_alu_instr->instr,
                                     &child_alu_src->src,
                                     alu_instr->src[0].src);

               for (unsigned i = 0; i < ARRAY_SIZE(child_alu_src->swizzle); i++) {
                  child_alu_src->swizzle[i] =
                     alu_instr->src[0].swizzle[child_alu_src->swizzle[i]];
               }

               local_progress = true;
            }
         }
      }

      if (local_progress)
         nir_metadata_preserve(function->impl, nir_metadata_none);
      progress |= local_progress;
   }

   return progress;
}
