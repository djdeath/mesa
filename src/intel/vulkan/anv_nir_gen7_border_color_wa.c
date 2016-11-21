/*
 * Copyright © 2016 Intel Corporation
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

/*
 * Implements a pass replaces integer sampler accesses with some logic to work
 * around broken border colors with integer formats on Ivybridge and Haswell.
 */

#include "anv_nir.h"
#include "nir/nir_builder.h"
#include "nir/nir_opcodes.h"

static void
wrap_tex_instr(nir_builder *b,
               nir_tex_instr *tex_instr,
               nir_variable *sampler,
               const struct anv_descriptor_set_layout *set_layout)
{
   nir_ssa_def *zeroi = nir_imm_int(b, 0);
   nir_intrinsic_instr *vk_instr =
      nir_intrinsic_instr_create(b->shader,
                                 nir_intrinsic_vulkan_resource_index);
   vk_instr->num_components = 1;
   vk_instr->src[0] = nir_src_for_ssa(zeroi); /* array index */
   nir_intrinsic_set_desc_set(vk_instr, 0);
   nir_intrinsic_set_binding(vk_instr, set_layout->border_color_index);
   nir_ssa_dest_init(&vk_instr->instr, &vk_instr->dest, 1, 32, NULL);
   nir_builder_instr_insert(b, &vk_instr->instr);

   nir_intrinsic_instr *load_wrapping_instr =
      nir_intrinsic_instr_create(b->shader, nir_intrinsic_load_ubo);
   load_wrapping_instr->num_components = 1;
   load_wrapping_instr->src[0] = nir_src_for_ssa(&vk_instr->dest.ssa);
   load_wrapping_instr->src[1] = /* ubo offset */
      nir_src_for_ssa(nir_imm_int(b, offsetof(struct gen7_border_color,
                                              wrapping)));
   nir_ssa_dest_init(&load_wrapping_instr->instr, &load_wrapping_instr->dest,
                     load_wrapping_instr->num_components, 32, NULL);
   nir_builder_instr_insert(b, &load_wrapping_instr->instr);



   nir_ssa_def *tex_coords =
      tex_instr->src[nir_tex_instr_src_index(tex_instr,
                                             nir_tex_src_coord)].src.ssa,
      *cond = NULL,
      *zerof = nir_imm_float(b, 0.0f),
      *onef = nir_imm_float(b, 1.0f);
   uint32_t coord_components = tex_instr->coord_components;
   if (glsl_sampler_type_is_array(sampler->type))
      coord_components -= 1;
   for (unsigned i = 0; i < coord_components; ++i) {
      nir_ssa_def *lt_def = nir_flt(b, nir_channel(b, tex_coords, i), zerof),
         *gt_def = nir_flt(b, onef, nir_channel(b, tex_coords, i)),
         *bounds_cond = nir_ior(b, lt_def, gt_def),
         *border_cond =
            nir_inot(b, nir_ieq(b,
                                nir_iand(b, &load_wrapping_instr->dest.ssa,
                                         nir_imm_int(b, 1 << i)),
                                zeroi)),
         *inner_cond = nir_iand(b, border_cond,
                                bounds_cond);

      if (cond == NULL)
         cond = inner_cond;
      else
         cond = nir_ior(b, cond, inner_cond);
   }
   nir_if *if_stmt = nir_if_create(b->shader);
   if_stmt->condition = nir_src_for_ssa(nir_inot(b, cond));
   nir_builder_cf_insert(b, &if_stmt->cf_node);

   /* Fill out the new then-block */
   nir_instr_remove(&tex_instr->instr);
   nir_instr_insert_after_cf_list(&if_stmt->then_list, &tex_instr->instr);

   /* Fill out the new else-block */
   b->cursor = nir_after_cf_list(&if_stmt->else_list);
   const struct anv_descriptor_set_binding_layout *binding_layout =
      &set_layout->binding[tex_instr->sampler->var->data.binding];
   uint32_t binding_offset = sizeof(struct gen7_border_color) *
      binding_layout->border_color_array_index;

   /* Load border color. */
   nir_intrinsic_instr *load_color_instr =
      nir_intrinsic_instr_create(b->shader, nir_intrinsic_load_ubo);
   load_color_instr->num_components = 4;
   load_color_instr->src[0] = nir_src_for_ssa(&vk_instr->dest.ssa);
   load_color_instr->src[1] = /* ubo offset */
      nir_src_for_ssa(nir_imm_int(b, binding_offset));
   nir_ssa_dest_init(&load_color_instr->instr, &load_color_instr->dest,
                     load_color_instr->num_components, 32, NULL);
   nir_builder_instr_insert(b, &load_color_instr->instr);

   /* Load border color mask. */
   nir_intrinsic_instr *load_mask_instr =
      nir_intrinsic_instr_create(b->shader, nir_intrinsic_load_ubo);
   load_mask_instr->num_components = 4;
   load_mask_instr->src[0] = nir_src_for_ssa(&vk_instr->dest.ssa);
   load_mask_instr->src[1] = /* ubo offset */
      nir_src_for_ssa(nir_imm_int(b, binding_offset +
                                  offsetof(struct gen7_border_color,
                                           color_mask)));
   nir_ssa_dest_init(&load_mask_instr->instr, &load_mask_instr->dest,
                     load_mask_instr->num_components, 32, NULL);
   nir_builder_instr_insert(b, &load_mask_instr->instr);

   /* Load the alpha overwrite. */
   nir_intrinsic_instr *load_alpha_instr =
      nir_intrinsic_instr_create(b->shader, nir_intrinsic_load_ubo);
   load_alpha_instr->num_components = 1;
   load_alpha_instr->src[0] = nir_src_for_ssa(&vk_instr->dest.ssa);
   load_alpha_instr->src[1] = /* ubo offset */
      nir_src_for_ssa(nir_imm_int(b, binding_offset +
                                  offsetof(struct gen7_border_color,
                                           alpha_overwrite)));
   nir_ssa_dest_init(&load_alpha_instr->instr, &load_alpha_instr->dest,
                     load_alpha_instr->num_components, 32, NULL);
   nir_builder_instr_insert(b, &load_alpha_instr->instr);

   nir_ssa_def *masked_color =
      nir_iand(b, &load_color_instr->dest.ssa, &load_mask_instr->dest.ssa);
   nir_ssa_def *color_components[4] = {
      nir_channel(b, masked_color, 0),
      nir_channel(b, masked_color, 1),
      nir_channel(b, masked_color, 2),
      nir_ior(b, nir_channel(b, masked_color, 3), &load_alpha_instr->dest.ssa)
   };
   nir_ssa_def *color = nir_vec(b, color_components, 4);

   /* We need a phi node to gather either values. */
   nir_phi_instr *phi = nir_phi_instr_create(b->shader);
   nir_ssa_dest_init(&phi->instr, &phi->dest,
                     tex_instr->dest.ssa.num_components,
                     tex_instr->dest.ssa.bit_size, NULL);

   nir_phi_src *src1 = ralloc(phi, nir_phi_src);
   struct exec_node *tnode = exec_list_get_tail(&if_stmt->then_list);
   src1->pred = exec_node_data(nir_block, tnode, cf_node.node);
   src1->src = nir_src_for_ssa(&tex_instr->dest.ssa);
   exec_list_push_tail(&phi->srcs, &src1->node);

   nir_phi_src *src2 = ralloc(phi, nir_phi_src);
   struct exec_node *enode = exec_list_get_tail(&if_stmt->else_list);
   src2->pred = exec_node_data(nir_block, enode, cf_node.node);
   src2->src = nir_src_for_ssa(color);
   exec_list_push_tail(&phi->srcs, &src2->node);

   nir_ssa_def_rewrite_uses(&tex_instr->dest.ssa,
                            nir_src_for_ssa(&phi->dest.ssa));
   nir_instr_insert_after_cf(&if_stmt->cf_node, &phi->instr);
}

void
anv_nir_apply_gen7_border_color(struct anv_pipeline *pipeline,
                                nir_shader *shader,
                                struct brw_stage_prog_data *prog_data)
{
   const struct anv_pipeline_layout *layout = pipeline->layout;
   const struct anv_descriptor_set_layout *set_layout = layout->set[0].layout;

   nir_foreach_function(function, shader) {
      if (!function->impl)
         continue;

      nir_builder builder;
      nir_builder_init(&builder, function->impl);

      nir_foreach_block(block, function->impl) {
         nir_foreach_instr_safe(instr, block) {
            if (instr->type != nir_instr_type_tex)
               continue;

            nir_tex_instr *tex_instr = nir_instr_as_tex(instr);
            if (!tex_instr->sampler ||
                nir_tex_instr_src_index(tex_instr, nir_tex_src_coord) == -1)
               continue;

            nir_variable *sampler = tex_instr->sampler->var;
            if (glsl_get_sampler_result_type(sampler->type) == GLSL_TYPE_FLOAT) {
               continue;
            }

            builder.cursor = nir_before_instr(instr);
            wrap_tex_instr(&builder, tex_instr, sampler, set_layout);
         }
      }
   }
}
