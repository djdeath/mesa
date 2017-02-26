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
#include "nir_builder.h"
#include "nir_control_flow.h"

bool
nir_opt_flatten_nested_ifs(nir_shader *shader)
{
   bool progress = false;

   nir_foreach_function(function, shader) {
      if (function->impl == NULL)
         continue;

      nir_builder b;
      nir_builder_init(&b, function->impl);

      /* We don't want the safe flavor, because we might delete the next
       * block. */
      nir_foreach_block(block, function->impl) {
         if (block->cf_node.type != nir_cf_node_if)
            continue;

         nir_if *if_stmt = nir_cf_node_as_if(&block->cf_node);
         if (exec_list_length(&if_stmt->else_list) > 0)
            continue;

         /* Look at the next block if it's an if block, we can proceed.*/
         nir_block *next_block = nir_cf_node_cf_tree_next(&if_stmt->cf_node);
         if (next_block == NULL ||
             next_block->cf_node.parent != &block->cf_node ||
             next_block->cf_node.type != nir_cf_node_if)
            continue;

         nir_if *nested_if_stmt = nir_cf_node_as_if(&next_block->cf_node);
         if (exec_list_length(&nested_if_stmt->else_list) > 0)
            continue;

         nir_block *after =
            nir_cf_node_as_block(nir_cf_node_next(&nested_if_stmt->cf_node));

         nir_foreach_instr_safe(instr, after) {
            if (instr->type != nir_instr_type_phi)
               break;

            nir_phi_instr *phi = nir_instr_as_phi(instr);
            nir_ssa_def *def = NULL;
            nir_foreach_phi_src(phi_src, phi) {
               if (phi_src->pred != block)
                  continue;

               assert(phi_src->src.is_ssa);
               def = phi_src->src.ssa;
            }

            assert(def);
            assert(phi->dest.is_ssa);
            nir_ssa_def_rewrite_uses(&phi->dest.ssa, nir_src_for_ssa(def));
            nir_instr_remove(instr);
         }

         nir_cf_list list;
         nir_cf_list_extract(&list, &nested_if_stmt->then_list);
         nir_cf_reinsert(&list, nir_after_cf_node(&nested_if_stmt->cf_node));
         nir_cf_node_remove(&nested_if_stmt->cf_node);

         progress = true;
      }
   }

   if (progress)
      fprintf(stderr, "nir_opt_flatten_nested_ifs!\n");

   return progress;
}
