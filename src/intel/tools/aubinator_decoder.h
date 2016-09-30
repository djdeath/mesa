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

#ifndef AUBINATOR_DECODER_H
#define AUBINATOR_DECODER_H

#include <stdint.h>
#include <stdbool.h>

#include "common/gen_device_info.h"

struct aub_spec;
struct aub_group;
struct aub_field;

static inline uint32_t aub_make_gen(uint32_t major, uint32_t minor)
{
   return (major << 8) | minor;
}

struct aub_group *aub_spec_find_struct(struct aub_spec *spec, const char *name);
struct aub_spec *aub_spec_load(const struct gen_device_info *devinfo);
uint32_t aub_spec_get_gen(struct aub_spec *spec);
struct aub_group *aub_spec_find_instruction(struct aub_spec *spec, const uint32_t *p);
struct aub_group *aub_spec_find_register(struct aub_spec *spec, uint32_t offset);
int aub_group_get_length(struct aub_group *group, const uint32_t *p);
const char *aub_group_get_name(struct aub_group *group);
uint32_t aub_group_get_opcode(struct aub_group *group);

struct aub_field_iterator {
   struct aub_group *group;
   const char *name;
   char value[128];
   const uint32_t *p;
   int i;
};

struct aub_group {
   char *name;
   int nfields;
   struct aub_field **fields;
   uint32_t group_offset, group_count;

   uint32_t opcode_mask;
   uint32_t opcode;

   /* Register specific */
   uint32_t register_offset;
};

struct aub_type {
   enum {
      AUB_TYPE_UNKNOWN,
      AUB_TYPE_INT,
      AUB_TYPE_UINT,
      AUB_TYPE_BOOL,
      AUB_TYPE_FLOAT,
      AUB_TYPE_ADDRESS,
      AUB_TYPE_OFFSET,
      AUB_TYPE_STRUCT,
      AUB_TYPE_UFIXED,
      AUB_TYPE_SFIXED,
      AUB_TYPE_MBO
   } kind;

   /* Struct definition for  AUB_TYPE_STRUCT */
   struct aub_group *aub_struct;

   /* Integer and fractional sizes for AUB_TYPE_UFIXED and AUB_TYPE_SFIXED */
   int i, f;
};

struct aub_field {
   char *name;
   int start, end;
   struct aub_type type;
   bool has_default;
   uint32_t default_value;
};

void aub_field_iterator_init(struct aub_field_iterator *iter,
                             struct aub_group *group, const uint32_t *p);

bool aub_field_iterator_next(struct aub_field_iterator *iter);

#endif /* AUBINATOR_DECODER_H */
