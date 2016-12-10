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

#include "anv_nir.h"
#include "nir_array.h"

struct ubo_range_entry
{
   struct anv_push_ubo_range range;
   int benefit;
};

static int
score(const struct ubo_range_entry *entry)
{
   return 2 * entry->benefit - entry->range.length;
}

/**
 * Compares score for two UBO range entries.
 *
 * For a descending qsort().
 */
static int
cmp_ubo_range_entry(const void *va, const void *vb)
{
   const struct ubo_range_entry *a = va;
   const struct ubo_range_entry *b = vb;

   return score(b) - score(a);
}

#define MAX_SET     (8)   /* maxBoundDescriptorSets */
#define MAX_UBO_SET (256) /* maxDescriptorSetUniformBuffers */

#define MAX_UBO (MAX_SET * MAX_UBO_SET)

struct ubo_analysis_state
{
   uint64_t offsets[MAX_UBO];
   uint8_t uses[MAX_UBO][64];
};

static int16_t
nir_vulkan_resource_index_to_block(const nir_src *src)
{
   if (src->ssa->parent_instr->type != nir_instr_type_intrinsic)
      return -1;

   nir_intrinsic_instr *intrin =
      nir_instr_as_intrinsic(src->ssa->parent_instr);
   if (intrin->intrinsic != nir_intrinsic_vulkan_resource_index)
      return -1;

   unsigned set = nir_intrinsic_desc_set(intrin);
   nir_const_value *binding_const = nir_src_as_const_value(intrin->src[0]);

   if (binding_const == NULL)
      return -1;

   uint32_t binding = binding_const->u32[0];

   return set * MAX_UBO_SET + binding;
}

static bool
analyze_ubos_block(struct ubo_analysis_state *state, nir_block *block)
{
   nir_foreach_instr(instr, block) {
      if (instr->type != nir_instr_type_intrinsic)
         continue;

      nir_intrinsic_instr *intrin = nir_instr_as_intrinsic(instr);
      if (intrin->intrinsic != nir_intrinsic_load_ubo)
         continue;

      int block_id = nir_vulkan_resource_index_to_block(&intrin->src[0]);
      nir_const_value *offset_const = nir_src_as_const_value(intrin->src[1]);

      if (block_id != -1 && offset_const) {
         const int offset = offset_const->u32[0] / (8 * 4);

         /* Won't fit in our bitfield */
         if (offset >= 64)
            continue;

         state->offsets[block_id] |= (1ull << offset);
         state->uses[block_id][offset]++;
      }
   }

   return true;
}

static void
print_ubo_entry(FILE *file,
                const struct ubo_range_entry *entry,
                const struct ubo_analysis_state *state)
{
   fprintf(file,
           "block %2d, start %2d, length %2d, bits = %zx, "
           "benefit %2d, cost %2d, score = %2d\n",
           entry->range.block, entry->range.start, entry->range.length,
           state->offsets[entry->range.block],
           entry->benefit, entry->range.length, score(entry));
}

void
anv_nir_analyze_ubo_ranges(nir_shader *nir,
                           struct anv_push_ubo_range out_ranges[3])
{
   void *mem_ctx = ralloc_context(NULL);
   struct ubo_analysis_state *state = rzalloc(mem_ctx,
                                              struct ubo_analysis_state);

   /* Walk the IR, recording how many times each UBO block/offset is used. */
   nir_foreach_function(function, nir) {
      if (function->impl) {
         nir_foreach_block(block, function->impl) {
            analyze_ubos_block(state, block);
         }
      }
   }

   /* Find ranges. */
   nir_array ranges;
   nir_array_init(&ranges, mem_ctx);

   for (int b = 0; b < ARRAY_SIZE(state->offsets); b++) {
      uint64_t offsets = state->offsets[b];

      while (offsets != 0) {
         int first_bit = ffsll(offsets) - 1;
         int first_hole = ffsll(~offsets & ~((1ull << first_bit) - 1)) - 1;
         if (first_hole == -1) {
            first_hole = 64;
            offsets = 0;
         } else {
            offsets &= ~((1ull << first_hole) - 1);
         }

         struct ubo_range_entry *entry =
            nir_array_grow(&ranges, sizeof(struct ubo_range_entry));

         entry->range.block = b;
         entry->range.start = first_bit;
         entry->range.length = first_hole - first_bit;

         for (int i = 0; i < entry->range.length; i++)
            entry->benefit += state->uses[b][first_bit + i];
      }
   }

   int nr_entries = ranges.size / sizeof(struct ubo_range_entry);

   if (getenv("ANV_PRINT_UBO")) {
      if (ranges.size > 0)
         nir_print_shader(nir, stderr);
      nir_array_foreach(&ranges, struct ubo_range_entry, entry) {
         print_ubo_entry(stderr, entry, state);
      }
   }

   /* TODO: Consider combining ranges.
    *
    * We can only push 3-4 ranges via 3DSTATE_CONSTANT_XS.  If there are
    * more ranges, and two are close by with only a small hole, it may be
    * worth combining them.  The holes will waste register space, but the
    * benefit of removing pulls may outweigh that cost.
    */

   /* Sort the list so the most beneficial ranges are at the front. */
   qsort(ranges.data, nr_entries, sizeof(struct ubo_range_entry),
         cmp_ubo_range_entry);

   struct ubo_range_entry *entries = ranges.data;

   /* Return the top 3.
    *
    * TODO: Return the top 4 if regular uniforms aren't in use.  This requires
    *       setting INSTPM/CS_DEBUG_MODE2 bits to make the first constant
    *       buffer use an absolute graphics address rather than a relative
    *       offset from dynamic state base address.  Beware kernel bugs.
    */
   nr_entries = MIN2(nr_entries, 3);

   for (int i = 0; i < nr_entries; i++) {
      out_ranges[i] = entries[i].range;
   }
   for (int i = nr_entries; i < 3; i++) {
      out_ranges[i].block = 0;
      out_ranges[i].start = 0;
      out_ranges[i].length = 0;
   }

   ralloc_free(ranges.mem_ctx);
}

/**/

static void
push_ubo_ranges(nir_block *block,
                struct anv_push_ubo_range *ubo_ranges,
                uint32_t n_ranges)
{
   nir_foreach_instr(instr, block) {
      if (instr->type != nir_instr_type_intrinsic)
         continue;

      nir_intrinsic_instr *intrin = nir_instr_as_intrinsic(instr);
      if (intrin->intrinsic != nir_intrinsic_load_ubo)
         continue;

      int block_id = nir_vulkan_resource_index_to_block(&intrin->src[0]);
      nir_const_value *offset_const = nir_src_as_const_value(intrin->src[1]);
      if (block_id == -1 && offset_const == NULL)
         continue;

      const int offset = offset_const->u32[0] / (8 * 4);
      for (uint32_t r = 0; r < n_ranges; r++) {
         if (block_id == ubo_ranges[r].block &&
             offset >= ubo_ranges[r].start &&
             offset < (ubo_ranges[r].start + ubo_ranges[r].length)) {
            if (getenv("ANV_PRINT_UBO"))
               fprintf(stderr, "replacing %i -> %u!\n",
                       offset, ubo_ranges[r].push_start);
            break;
         }
      }
   }
}

void
anv_nir_push_ubo_ranges(nir_shader *nir,
                        struct anv_push_ubo_range *ubo_ranges,
                        uint32_t n_ranges)
{
   nir_foreach_function(function, nir) {
      if (!function->impl)
         continue;

      nir_foreach_block(block, function->impl) {
         push_ubo_ranges(block, ubo_ranges, n_ranges);
      }
   }
}
