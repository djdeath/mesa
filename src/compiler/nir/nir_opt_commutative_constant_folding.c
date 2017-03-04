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

#include "nir_builder.h"

#define DEBUG_PRINT (false)

typedef struct {
   nir_ssa_def *ssa;

   bool negate;
   bool abs;
   uint8_t swizzle[4];

   bool alive;
} local_alu_src;

struct collect_context {
   void *mem_ctx;
   uint32_t n_allocated_srcs;

   bool is_fragment;

   nir_op op;
   nir_block *block;
   uint8_t num_components;
   uint8_t bit_size;

   local_alu_src *srcs;
   uint32_t write;
   uint32_t read;
};

static void
update_alu_src(nir_alu_src *dest, local_alu_src *src)
{
   dest->negate = src->negate;
   dest->abs = src->abs;
   for (unsigned i = 0; i < 4; i++)
      dest->swizzle[i] = src->swizzle[i];
}

static void
save_alu_src(local_alu_src *dest, nir_alu_src *src)
{
   dest->ssa = src->src.ssa;
   dest->negate = src->negate;
   dest->abs = src->abs;
   for (unsigned i = 0; i < 4; i++)
      dest->swizzle[i] = src->swizzle[i];
   dest->alive = true;
}

static void
print_saved_alu_src(local_alu_src *dest)
{
   fprintf(stderr, "\tsave ssa_%u neg=%u abs=%u swiz=%hhu/%hhu/%hhu/%hhu ",
           dest->ssa->index, dest->negate, dest->abs,
           dest->swizzle[0],dest->swizzle[1],dest->swizzle[2],dest->swizzle[3]);
   nir_print_instr(dest->ssa->parent_instr, stderr);
   fprintf(stderr, "\n");
}

static void
collect_source(struct collect_context *ctx,
               local_alu_src *parent,
               nir_alu_src *src,
               unsigned src_index)
{
   if (ctx->write >= ctx->n_allocated_srcs) {
      ctx->n_allocated_srcs *= 2;
      ctx->srcs = reralloc_size(ctx->mem_ctx, ctx->srcs,
                                sizeof(local_alu_src) * ctx->n_allocated_srcs);
   }

   save_alu_src(&ctx->srcs[ctx->write], src);
   ctx->srcs[ctx->write].negate ^= parent->negate;
   ctx->srcs[ctx->write].abs = parent->abs;
   for (unsigned i = 0; i < 4; i++) {
      if (nir_alu_instr_channel_used(nir_instr_as_alu(src->src.parent_instr),
                                     src_index, i))
         ctx->srcs[ctx->write].swizzle[i] = src->swizzle[parent->swizzle[i]];
   }
   if (ctx->is_fragment) print_saved_alu_src(&ctx->srcs[ctx->write]);

   ctx->write++;
}

static void
collect_sources(struct collect_context *ctx)
{
   nir_instr *instr = ctx->srcs[ctx->read].ssa->parent_instr;

   /* Do not cross blocks. */
   if (instr->type != nir_instr_type_alu ||
       instr->block != ctx->block)
      return;

   nir_alu_instr *alu_instr = nir_instr_as_alu(instr);
   if (alu_instr->op != ctx->op ||
       alu_instr->exact)
      return;

   if ((list_length(&alu_instr->dest.dest.ssa.uses) +
        list_length(&alu_instr->dest.dest.ssa.if_uses)) > 1)
      return;

   if (alu_instr->dest.dest.ssa.num_components != ctx->num_components ||
       alu_instr->dest.dest.ssa.bit_size != ctx->bit_size)
      return;

   if (ctx->is_fragment) {
      fprintf(stderr, "depile ");
      nir_print_instr(instr, stderr);
      fprintf(stderr, "\n");
   }

   ctx->srcs[ctx->read].alive = false;

   for (unsigned i = 0; i < nir_op_infos[ctx->op].num_inputs; i++) {
      assert(alu_instr->src[i].src.is_ssa);
      collect_source(ctx, &ctx->srcs[ctx->read], &alu_instr->src[i], i);
   }
}

static bool
constant_fold_alu_instr(nir_alu_instr *instr,
                        nir_builder *builder,
                        struct collect_context *ctx)
{
   const nir_op_algebraic_property required_properties =
      NIR_OP_IS_COMMUTATIVE | NIR_OP_IS_ASSOCIATIVE;

   if (!instr->dest.dest.is_ssa)
      return false;
   if ((nir_op_infos[instr->op].algebraic_properties & required_properties) != required_properties)
      return false;
   if (instr->exact)
      return false;

   /* If a variable is used more than once, we might generate more
    * instructions than we could save. */
   if ((list_length(&instr->dest.dest.ssa.uses) +
        list_length(&instr->dest.dest.ssa.if_uses)) > 1)
      return false;

   if (ctx->is_fragment) {
      fprintf(stderr, "depile ");
      nir_print_instr(&instr->instr, stderr);
      fprintf(stderr, "\n");
   }

   const unsigned num_inputs = nir_op_infos[instr->op].num_inputs;

   ctx->op = instr->op;
   ctx->block = instr->instr.block;
   ctx->num_components = instr->dest.dest.ssa.num_components;
   ctx->bit_size = instr->dest.dest.ssa.bit_size;
   for (unsigned i = 0; i < num_inputs; i++) {
      save_alu_src(&ctx->srcs[i], &instr->src[i]);
      if (ctx->is_fragment) print_saved_alu_src(&ctx->srcs[i]);
   }
   ctx->read = 0;
   ctx->write = num_inputs;

   while (ctx->read < ctx->write) {
      collect_sources(ctx);
      ctx->read++;
   }

   /* We might have holes in the array, let's pack everthing. */
   unsigned write = 0, constants = 0;
   for (unsigned i = 0; i < ctx->write; i++) {
      if (!ctx->srcs[i].alive)
         continue;

      memmove(&ctx->srcs[write], &ctx->srcs[i], sizeof(ctx->srcs[i]));
      if (nir_src_as_const_value(nir_src_for_ssa(ctx->srcs[i].ssa))) {
         /* Ensure we put constants first. */
         local_alu_src tmp;
         memcpy(&tmp, &ctx->srcs[constants], sizeof(tmp));
         memmove(&ctx->srcs[constants], &ctx->srcs[write], sizeof(tmp));
         memcpy(&ctx->srcs[write], &tmp, sizeof(tmp));
         constants++;
      }
      write++;
   }

   /* We can only apply the optimization with at least 2 constants. */
   if (constants < 2) {
      if (ctx->is_fragment) fprintf(stderr, "fail.\n");
      return false;
   }

   /* debug */
   if (ctx->is_fragment) fprintf(stderr, "write=%u constants=%u\n", write, constants);

   /* Rebuild the expression out of the extracted terms. */
   builder->cursor = nir_before_instr(&instr->instr);

   nir_ssa_def *srcs[4] = { NULL, };
   for (unsigned i = 0; i < num_inputs; i++)
      srcs[i] = ctx->srcs[i].ssa;

   nir_ssa_def *last_def = nir_build_alu(builder, ctx->op,
                                         srcs[0], srcs[1], srcs[2], srcs[3]);
   last_def->num_components = ctx->num_components;
   last_def->bit_size = ctx->bit_size;
   nir_alu_instr *alu_instr = nir_instr_as_alu(last_def->parent_instr);
   alu_instr->dest.write_mask = instr->dest.write_mask;
   for (unsigned i = 0; i < num_inputs; i++)
      update_alu_src(&alu_instr->src[i], &ctx->srcs[i]);

   for (unsigned i = num_inputs; i < write; i += (num_inputs - 1)) {
      for (unsigned j = 0; j < (num_inputs - 1); j++)
         srcs[j] = ctx->srcs[i + j].ssa;
      srcs[num_inputs - 1] = last_def;
      last_def = nir_build_alu(builder, ctx->op,
                               srcs[0], srcs[1], srcs[2], srcs[3]);
      last_def->num_components = ctx->num_components;
      last_def->bit_size = ctx->bit_size;
      alu_instr = nir_instr_as_alu(last_def->parent_instr);
      alu_instr->dest.write_mask = instr->dest.write_mask;
      for (unsigned j = 0; j < (num_inputs - 1); j++)
         update_alu_src(&alu_instr->src[j], &ctx->srcs[i + j]);
   }

   nir_ssa_def_rewrite_uses(&instr->dest.dest.ssa,
                            nir_src_for_ssa(last_def));
   nir_instr_remove(&instr->instr);

   /* Remove all unused variables that we know are only used once. */
   for (unsigned i = 0; i < write; i++) {
      if (ctx->srcs[i].alive)
         continue;

      nir_instr_remove(ctx->srcs[i].ssa->parent_instr);
   }

   return true;
}

static bool
constant_fold_block(nir_block *block,
                    nir_builder *builder,
                    struct collect_context *ctx)
{
   bool progress = false;

   nir_foreach_instr_reverse_safe(instr, block) {
      if (instr->type != nir_instr_type_alu)
         continue;

      progress |= constant_fold_alu_instr(nir_instr_as_alu(instr), builder, ctx);
   }

   return progress;
}

static bool
nir_opt_constant_folding_impl(nir_function_impl *impl,
                              struct collect_context *ctx)
{
   bool progress = false;
   nir_builder builder;

   nir_index_instrs(impl);

   nir_builder_init(&builder, impl);
   nir_foreach_block(block, impl) {
      progress |= constant_fold_block(block, &builder, ctx);
   }

   if (progress) {
      nir_metadata_preserve(impl, nir_metadata_none);/* block_index | */
                                  /* nir_metadata_dominance); */
      nir_index_instrs(impl);
   }

   return progress;
}

bool
nir_opt_commutative_constant_folding(nir_shader *shader)
{
   bool progress = false;
   void *mem_ctx = ralloc_context(NULL);
   struct collect_context ctx = {
      .mem_ctx = mem_ctx,
      .n_allocated_srcs = 4,
      .srcs = ralloc_size(mem_ctx, sizeof(local_alu_src) * 4),

      .is_fragment = DEBUG_PRINT && shader->stage == MESA_SHADER_FRAGMENT,
   };

   if (ctx.is_fragment)
      nir_print_shader(shader, stderr);

   nir_foreach_function(function, shader) {
      if (function->impl)
         progress |= nir_opt_constant_folding_impl(function->impl, &ctx);
   }

   if (ctx.is_fragment)
      nir_print_shader(shader, stderr);

   ralloc_free(ctx.srcs);

   return progress;
}
