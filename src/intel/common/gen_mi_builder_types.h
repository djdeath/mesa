/*
 * Copyright © 2020 Intel Corporation
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

#ifndef GEN_MI_BUILDER_TYPES_H
#define GEN_MI_BUILDER_TYPES_H

#ifndef GEN_MI_BUILDER_NUM_ALLOC_GPRS
/** The number of GPRs the MI builder is allowed to allocate
 *
 * This may be set by a user of this API so that it can reserve some GPRs at
 * the top end for its own use.
 */
#define GEN_MI_BUILDER_NUM_ALLOC_GPRS 16
#endif

/** These must be defined by the user of the builder
 *
 * void *__gen_get_batch_dwords(__gen_user_data *user_data,
 *                              unsigned num_dwords);
 *
 * __gen_address_type
 * __gen_address_offset(__gen_address_type addr, uint64_t offset);
 *
 */

enum gen_mi_value_type {
   GEN_MI_VALUE_TYPE_IMM,
   GEN_MI_VALUE_TYPE_MEM32,
   GEN_MI_VALUE_TYPE_MEM64,
   GEN_MI_VALUE_TYPE_REG32,
   GEN_MI_VALUE_TYPE_REG64,
};

struct gen_mi_value {
   enum gen_mi_value_type type;

   union {
      uint64_t imm;
      __gen_address_type addr;
      uint32_t reg;
   };

   bool invert;
};

struct gen_mi_builder {
   __gen_user_data *user_data;

   uint32_t gprs;
   uint8_t gpr_refs[GEN_MI_BUILDER_NUM_ALLOC_GPRS];

   unsigned num_math_dwords;
   uint32_t math_dwords[256 /* Gen9 max */];
};

static inline void
gen_mi_builder_init(struct gen_mi_builder *b,
                     __gen_user_data *user_data)
{
   memset(b, 0, sizeof(*b));
   b->user_data = user_data;

   b->gprs = 0;
   b->num_math_dwords = 0;
}

#endif /* GEN_MI_BUILDER_TYPES_H */
