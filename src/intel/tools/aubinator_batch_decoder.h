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

#ifndef AUBINATOR_BATCH_DECODER_H
#define AUBINATOR_BATCH_DECODER_H

#include <stdio.h>
#include <stdint.h>

#include "aubinator_decoder.h"
#include "aubinator_disasm.h"

struct aub_batch_decoder {
   /* configuration: */
   bool full_decode;
   bool print_offsets;

   enum {
      AUB_PRINT_COLOR_AUTO,
      AUB_PRINT_COLOR_ALWAYS,
      AUB_PRINT_COLOR_NEVER
   } print_color;

   /* internal fields: */
   struct aub_spec *spec;
   struct aub_disasm *disasm;

   FILE *out;

   uint32_t *start, *end, *cursor;

   uint64_t gtt_size, gtt_end;
   void *gtt;

   uint64_t general_state_base;
   uint64_t surface_state_base;
   uint64_t dynamic_state_base;
   uint64_t instruction_base;
   uint64_t instruction_bound;
};

bool aub_batch_decoder_init(struct aub_batch_decoder *decoder,
                            const struct gen_device_info *devinfo);
void aub_batch_decoder_fini(struct aub_batch_decoder *decoder);

void aub_batch_decoder_write_data(struct aub_batch_decoder *decoder,
                                  void *data, size_t length,
                                  uint64_t offset);

void aub_batch_decoder_decode_aub(struct aub_batch_decoder *decoder,
                                  void *batch, size_t length,
                                  FILE *out);

#endif /* AUBINATOR_BATCH_DECODER_H */
