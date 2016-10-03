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

#include "aubinator_batch_decoder.h"

#include "intel_aub.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

/* Below is the only command missing from intel_aub.h in libdrm
 * So, reuse intel_aub.h from libdrm and #define the
 * AUB_MI_BATCH_BUFFER_END as below
 */
#define AUB_MI_BATCH_BUFFER_END (0x0500 << 16)

#define CSI "\e["
#define HEADER CSI "37;44m"
#define NORMAL CSI "0m"
#define CLEAR_TO_EOL CSI "0K"

static inline uint32_t
field(uint32_t value, int start, int end)
{
   uint32_t mask;

   mask = ~0U >> (31 - end + start);

   return (value >> start) & mask;
}

struct brw_instruction;

static inline int
valid_offset(struct aub_batch_decoder *decoder, uint32_t offset)
{
   return offset < decoder->gtt_end;
}

static void
print_dword_val(struct aub_batch_decoder *decoder,
                struct aub_field_iterator *iter, uint64_t offset,
                int *dword_num)
{
   struct aub_field *f;

   f = iter->group->fields[iter->i - 1];
   const int dword = f->start / 32;

   if (*dword_num != dword) {
      fprintf(decoder->out,
              "0x%08lx:  0x%08x : Dword %d\n",
              offset + 4 * dword,  iter->p[dword], dword);
      *dword_num = dword;
   }
}

static char *
print_iterator_values(struct aub_batch_decoder *decoder,
                      struct aub_field_iterator *iter, int *idx)
{
    char *token = NULL;
    if (strstr(iter->value, "struct") == NULL) {
       fprintf(decoder->out, "    %s: %s\n", iter->name, iter->value);
    } else {
        token = strtok(iter->value, " ");
        if (token != NULL) {
            token = strtok(NULL, " ");
            *idx = atoi(strtok(NULL, ">"));
        } else {
            token = NULL;
        }
        fprintf(decoder->out, "    %s:<struct %s>\n", iter->name, token);
    }
    return token;
}

static void
decode_structure(struct aub_batch_decoder *decoder, struct aub_group *strct,
                 const uint32_t *p)
{
   struct aub_field_iterator iter;
   char *token = NULL;
   int idx = 0, dword_num = 0;
   uint64_t offset = 0;

   if (decoder->print_offsets)
      offset = (void *) p - decoder->gtt;
   else
      offset = 0;

   aub_field_iterator_init(&iter, strct, p);
   while (aub_field_iterator_next(&iter)) {
      idx = 0;
      print_dword_val(decoder, &iter, offset, &dword_num);
      token = print_iterator_values(decoder, &iter, &idx);
      if (token != NULL) {
         struct aub_group *struct_val = aub_spec_find_struct(decoder->spec, token);
         decode_structure(decoder, struct_val, &p[idx]);
         token = NULL;
      }
   }
}

static void
handle_struct_decode(struct aub_batch_decoder *decoder, char *struct_name, uint32_t *p)
{
   if (struct_name == NULL)
      return;
   struct aub_group *struct_val = aub_spec_find_struct(decoder->spec, struct_name);
   decode_structure(decoder, struct_val, p);
}

static void
dump_binding_table(struct aub_batch_decoder *decoder, uint32_t offset)
{
   uint32_t *pointers, i;
   uint64_t start;
   struct aub_group *surface_state;

   surface_state = aub_spec_find_struct(decoder->spec, "RENDER_SURFACE_STATE");
   if (surface_state == NULL) {
      fprintf(decoder->out, "did not find RENDER_SURFACE_STATE info\n");
      return;
   }

   start = decoder->surface_state_base + offset;
   pointers = decoder->gtt + start;
   for (i = 0; i < 16; i++) {
      if (pointers[i] == 0)
         continue;
      start = pointers[i] + decoder->surface_state_base;
      if (!valid_offset(decoder, start)) {
         fprintf(decoder->out, "pointer %u: %08x <not valid>\n",
                 i, pointers[i]);
         continue;
      } else {
         fprintf(decoder->out,
                        "pointer %u: %08x\n", i, pointers[i]);
      }

      decode_structure(decoder, surface_state, decoder->gtt + start);
   }
}

static void
handle_3dstate_index_buffer(struct aub_batch_decoder *decoder, uint32_t *p)
{
   void *start;
   uint32_t length, i, type, size;

   start = decoder->gtt + p[2];
   type = (p[1] >> 8) & 3;
   size = 1 << type;
   length = p[4] / size;
   if (length > 10)
      length = 10;

   printf("\t");

   for (i = 0; i < length; i++) {
      switch (type) {
      case 0:
         printf("%3d ", ((uint8_t *)start)[i]);
         break;
      case 1:
         printf("%3d ", ((uint16_t *)start)[i]);
         break;
      case 2:
         printf("%3d ", ((uint32_t *)start)[i]);
         break;
      }
   }
   if (length < p[4] / size)
      printf("...\n");
   else
      printf("\n");
}

static inline uint64_t
get_qword(uint32_t *p)
{
   return ((uint64_t) p[1] << 32) | p[0];
}

static void
handle_state_base_address(struct aub_batch_decoder *decoder, uint32_t *p)
{
   uint64_t mask = ~((1 << 12) - 1);

   if (aub_spec_get_gen(decoder->spec) >= aub_make_gen(8,0)) {
      if (p[1] & 1)
         decoder->general_state_base = get_qword(&p[1]) & mask;
      if (p[4] & 1)
         decoder->surface_state_base = get_qword(&p[4]) & mask;
      if (p[6] & 1)
         decoder->dynamic_state_base = get_qword(&p[6]) & mask;
      if (p[10] & 1)
         decoder->instruction_base = get_qword(&p[10]) & mask;
      if (p[15] & 1)
         decoder->instruction_bound = p[15] & mask;
   } else {
      if (p[2] & 1)
         decoder->surface_state_base = p[2] & mask;
      if (p[3] & 1)
         decoder->dynamic_state_base = p[3] & mask;
      if (p[5] & 1)
         decoder->instruction_base = p[5] & mask;
      if (p[9] & 1)
         decoder->instruction_bound = p[9] & mask;
   }
}

static void
dump_samplers(struct aub_batch_decoder *decoder, uint32_t offset)
{
   uint32_t i;
   uint64_t start;
   struct aub_group *sampler_state;

   sampler_state = aub_spec_find_struct(decoder->spec, "SAMPLER_STATE");

   start = decoder->dynamic_state_base + offset;
   for (i = 0; i < 4; i++) {
      printf("sampler state %d\n", i);
      decode_structure(decoder, sampler_state, decoder->gtt + start + i * 16);
   }
}

static void
handle_media_interface_descriptor_load(struct aub_batch_decoder *decoder,
                                       uint32_t *p)
{
   int i, length = p[2] / 32;
   struct aub_group *descriptor_structure;
   uint32_t *descriptors;
   uint64_t start;
   struct brw_instruction *insns;

   descriptor_structure =
      aub_spec_find_struct(decoder->spec, "INTERFACE_DESCRIPTOR_DATA");
   if (descriptor_structure == NULL) {
      printf("did not find INTERFACE_DESCRIPTOR_DATA info\n");
      return;
   }

   start = decoder->dynamic_state_base + p[3];
   descriptors = decoder->gtt + start;
   for (i = 0; i < length; i++, descriptors += 8) {
      printf("descriptor %u: %08x\n", i, *descriptors);
      decode_structure(decoder, descriptor_structure, descriptors);

      start = decoder->instruction_base + descriptors[0];
      if (!valid_offset(decoder, start)) {
         printf("kernel: %08lx <not valid>\n", start);
         continue;
      } else {
         printf("kernel: %08lx\n", start);
      }

      insns = (struct brw_instruction *) (decoder->gtt + start);
      aub_disasm_disassemble(decoder->disasm, insns, 0, stdout);

      dump_samplers(decoder, descriptors[3] & ~0x1f);
      dump_binding_table(decoder, descriptors[4] & ~0x1f);
   }
}

/* Heuristic to determine whether a uint32_t is probably actually a float
 * (http://stackoverflow.com/a/2953466)
 */

static bool
probably_float(uint32_t bits)
{
   int exp = ((bits & 0x7f800000U) >> 23) - 127;
   uint32_t mant = bits & 0x007fffff;

   /* +- 0.0 */
   if (exp == -127 && mant == 0)
      return true;

   /* +- 1 billionth to 1 billion */
   if (-30 <= exp && exp <= 30)
      return true;

   /* some value with only a few binary digits */
   if ((mant & 0x0000ffff) == 0)
      return true;

   return false;
}

static void
handle_3dstate_vertex_buffers(struct aub_batch_decoder *decoder, uint32_t *p)
{
   uint32_t *end, *s, *dw, *dwend;
   uint64_t offset;
   int n, i, count, stride;

   end = (p[0] & 0xff) + p + 2;
   for (s = &p[1], n = 0; s < end; s += 4, n++) {
      if (aub_spec_get_gen(decoder->spec) >= aub_make_gen(8, 0)) {
         offset = *(uint64_t *) &s[1];
         dwend = decoder->gtt + offset + s[3];
      } else {
         offset = s[1];
         dwend = decoder->gtt + s[2] + 1;
      }

      stride = field(s[0], 0, 11);
      count = 0;
      printf("vertex buffer %d, size %d\n", n, s[3]);
      for (dw = decoder->gtt + offset, i = 0; dw < dwend && i < 256; dw++) {
         if (count == 0 && count % (8 * 4) == 0)
            printf("  ");

         if (probably_float(*dw))
            printf("  %8.2f", *(float *) dw);
         else
            printf("  0x%08x", *dw);

         i++;
         count += 4;

         if (count == stride) {
            printf("\n");
            count = 0;
         } else if (count % (8 * 4) == 0) {
            printf("\n");
         } else {
            printf(" ");
         }
      }
      if (count > 0 && count % (8 * 4) != 0)
         printf("\n");
   }
}

static void
handle_3dstate_vs(struct aub_batch_decoder *decoder, uint32_t *p)
{
   uint64_t start;
   struct brw_instruction *insns;
   int vs_enable;

   if (aub_spec_get_gen(decoder->spec) >= aub_make_gen(8, 0)) {
      start = get_qword(&p[1]);
      vs_enable = p[7] & 1;
   } else {
      start = p[1];
      vs_enable = p[5] & 1;
   }

   if (vs_enable) {
      printf("instruction_base %08lx, start %08lx\n",
             decoder->instruction_base, start);

      insns = (struct brw_instruction *) (decoder->gtt +
                                          decoder->instruction_base +
                                          start);
      aub_disasm_disassemble(decoder->disasm, insns, 0, stdout);
   }
}

static void
handle_3dstate_hs(struct aub_batch_decoder *decoder, uint32_t *p)
{
   uint64_t start;
   struct brw_instruction *insns;
   int hs_enable;

   if (aub_spec_get_gen(decoder->spec) >= aub_make_gen(8, 0)) {
      start = get_qword(&p[4]);
   } else {
      start = p[4];
   }

   hs_enable = p[2] & 0x80000000;

   if (hs_enable) {
      printf("instruction_base %08lx, start %08lx\n",
             decoder->instruction_base, start);

      insns = (struct brw_instruction *) (decoder->gtt +
                                          decoder->instruction_base +
                                          start);
      aub_disasm_disassemble(decoder->disasm, insns, 0, stdout);
   }
}

static void
handle_3dstate_constant(struct aub_batch_decoder *decoder, uint32_t *p)
{
   int i, j, length;
   uint32_t *dw;
   float *f;

   for (i = 0; i < 4; i++) {
      length = (p[1 + i / 2] >> (i & 1) * 16) & 0xffff;
      f = (float *) (decoder->gtt + p[3 + i * 2] + decoder->dynamic_state_base);
      dw = (uint32_t *) f;
      for (j = 0; j < length * 8; j++) {
         if (probably_float(dw[j]))
            printf("  %04.3f", f[j]);
         else
            printf("  0x%08x", dw[j]);

         if ((j & 7) == 7)
            printf("\n");
      }
   }
}

static void
handle_3dstate_ps(struct aub_batch_decoder *decoder, uint32_t *p)
{
   uint32_t mask = ~((1 << 6) - 1);
   uint64_t start;
   struct brw_instruction *insns;
   static const char unused[] = "unused";
   static const char *pixel_type[3] = {"8 pixel", "16 pixel", "32 pixel"};
   const char *k0, *k1, *k2;
   uint32_t k_mask, k1_offset, k2_offset;

   if (aub_spec_get_gen(decoder->spec) >= aub_make_gen(8, 0)) {
      k_mask = p[6] & 7;
      k1_offset = 8;
      k2_offset = 10;
   } else {
      k_mask = p[4] & 7;
      k1_offset = 6;
      k2_offset = 7;
   }

#define DISPATCH_8 1
#define DISPATCH_16 2
#define DISPATCH_32 4

   switch (k_mask) {
   case DISPATCH_8:
      k0 = pixel_type[0];
      k1 = unused;
      k2 = unused;
      break;
   case DISPATCH_16:
      k0 = pixel_type[1];
      k1 = unused;
      k2 = unused;
      break;
   case DISPATCH_8 | DISPATCH_16:
      k0 = pixel_type[0];
      k1 = unused;
      k2 = pixel_type[1];
      break;
   case DISPATCH_32:
      k0 = pixel_type[2];
      k1 = unused;
      k2 = unused;
      break;
   case DISPATCH_16 | DISPATCH_32:
      k0 = unused;
      k1 = pixel_type[2];
      k2 = pixel_type[1];
      break;
   case DISPATCH_8 | DISPATCH_16 | DISPATCH_32:
      k0 = pixel_type[0];
      k1 = pixel_type[2];
      k2 = pixel_type[1];
      break;
   default:
      k0 = unused;
      k1 = unused;
      k2 = unused;
      break;
   }

   start = decoder->instruction_base + (p[1] & mask);
   printf("  Kernel[0] %s\n", k0);
   if (k0 != unused) {
      insns = (struct brw_instruction *) (decoder->gtt + start);
      aub_disasm_disassemble(decoder->disasm, insns, 0, stdout);
   }

   start = decoder->instruction_base + (p[k1_offset] & mask);
   printf("  Kernel[1] %s\n", k1);
   if (k1 != unused) {
      insns = (struct brw_instruction *) (decoder->gtt + start);
      aub_disasm_disassemble(decoder->disasm, insns, 0, stdout);
   }

   start = decoder->instruction_base + (p[k2_offset] & mask);
   printf("  Kernel[2] %s\n", k2);
   if (k2 != unused) {
      insns = (struct brw_instruction *) (decoder->gtt + start);
      aub_disasm_disassemble(decoder->disasm, insns, 0, stdout);
   }
}

static void
handle_3dstate_binding_table_pointers(struct aub_batch_decoder *decoder,
                                      uint32_t *p)
{
   dump_binding_table(decoder, p[1]);
}

static void
handle_3dstate_sampler_state_pointers(struct aub_batch_decoder *decoder,
                                      uint32_t *p)
{
   dump_samplers(decoder, p[1]);
}

static void
handle_3dstate_viewport_state_pointers_cc(struct aub_batch_decoder *decoder,
                                          uint32_t *p)
{
   uint64_t start;
   struct aub_group *cc_viewport;

   cc_viewport = aub_spec_find_struct(decoder->spec, "CC_VIEWPORT");

   start = decoder->dynamic_state_base + (p[1] & ~0x1fu);
   for (uint32_t i = 0; i < 4; i++) {
      printf("viewport %d\n", i);
      decode_structure(decoder, cc_viewport, decoder->gtt + start + i * 8);
   }
}

static void
handle_3dstate_viewport_state_pointers_sf_clip(struct aub_batch_decoder *decoder,
                                               uint32_t *p)
{
   uint64_t start;
   struct aub_group *sf_clip_viewport;

   sf_clip_viewport = aub_spec_find_struct(decoder->spec, "SF_CLIP_VIEWPORT");

   start = decoder->dynamic_state_base + (p[1] & ~0x3fu);
   for (uint32_t i = 0; i < 4; i++) {
      printf("viewport %d\n", i);
      decode_structure(decoder, sf_clip_viewport, decoder->gtt + start + i * 64);
   }
}

static void
handle_3dstate_blend_state_pointers(struct aub_batch_decoder *decoder,
                                    uint32_t *p)
{
   uint64_t start;
   struct aub_group *blend_state;

   blend_state = aub_spec_find_struct(decoder->spec, "BLEND_STATE");

   start = decoder->dynamic_state_base + (p[1] & ~0x3fu);
   decode_structure(decoder, blend_state, decoder->gtt + start);
}

static void
handle_3dstate_cc_state_pointers(struct aub_batch_decoder *decoder, uint32_t *p)
{
   uint64_t start;
   struct aub_group *cc_state;

   cc_state = aub_spec_find_struct(decoder->spec, "COLOR_CALC_STATE");

   start = decoder->dynamic_state_base + (p[1] & ~0x3fu);
   decode_structure(decoder, cc_state, decoder->gtt + start);
}

static void
handle_3dstate_scissor_state_pointers(struct aub_batch_decoder *decoder,
                                      uint32_t *p)
{
   uint64_t start;
   struct aub_group *scissor_rect;

   scissor_rect = aub_spec_find_struct(decoder->spec, "SCISSOR_RECT");

   start = decoder->dynamic_state_base + (p[1] & ~0x1fu);
   decode_structure(decoder, scissor_rect, decoder->gtt + start);
}

static void
handle_load_register_imm(struct aub_batch_decoder *decoder, uint32_t *p)
{
   struct aub_group *reg = aub_spec_find_register(decoder->spec, p[1]);

   if (reg != NULL) {
      printf("register %s (0x%x): 0x%x\n",
             reg->name, reg->register_offset, p[2]);
      decode_structure(decoder, reg, &p[2]);
   }
}

#define ARRAY_LENGTH(a) (sizeof (a) / sizeof (a)[0])

#define STATE_BASE_ADDRESS                  0x61010000

#define MEDIA_INTERFACE_DESCRIPTOR_LOAD     0x70020000

#define _3DSTATE_INDEX_BUFFER               0x780a0000
#define _3DSTATE_VERTEX_BUFFERS             0x78080000

#define _3DSTATE_VS                         0x78100000
#define _3DSTATE_GS                         0x78110000
#define _3DSTATE_HS                         0x781b0000
#define _3DSTATE_DS                         0x781d0000

#define _3DSTATE_CONSTANT_VS                0x78150000
#define _3DSTATE_CONSTANT_GS                0x78160000
#define _3DSTATE_CONSTANT_PS                0x78170000
#define _3DSTATE_CONSTANT_HS                0x78190000
#define _3DSTATE_CONSTANT_DS                0x781A0000

#define _3DSTATE_PS                         0x78200000

#define _3DSTATE_BINDING_TABLE_POINTERS_VS  0x78260000
#define _3DSTATE_BINDING_TABLE_POINTERS_HS  0x78270000
#define _3DSTATE_BINDING_TABLE_POINTERS_DS  0x78280000
#define _3DSTATE_BINDING_TABLE_POINTERS_GS  0x78290000
#define _3DSTATE_BINDING_TABLE_POINTERS_PS  0x782a0000

#define _3DSTATE_SAMPLER_STATE_POINTERS_VS  0x782b0000
#define _3DSTATE_SAMPLER_STATE_POINTERS_GS  0x782e0000
#define _3DSTATE_SAMPLER_STATE_POINTERS_PS  0x782f0000

#define _3DSTATE_VIEWPORT_STATE_POINTERS_CC 0x78230000
#define _3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP 0x78210000
#define _3DSTATE_BLEND_STATE_POINTERS       0x78240000
#define _3DSTATE_CC_STATE_POINTERS          0x780e0000
#define _3DSTATE_SCISSOR_STATE_POINTERS     0x780f0000

#define _MI_LOAD_REGISTER_IMM               0x11000000

struct custom_handler {
   uint32_t opcode;
   void (*handle)(struct aub_batch_decoder *decoder, uint32_t *p);
} custom_handlers[] = {
   { STATE_BASE_ADDRESS, handle_state_base_address },
   { MEDIA_INTERFACE_DESCRIPTOR_LOAD, handle_media_interface_descriptor_load },
   { _3DSTATE_VERTEX_BUFFERS, handle_3dstate_vertex_buffers },
   { _3DSTATE_INDEX_BUFFER, handle_3dstate_index_buffer },
   { _3DSTATE_VS, handle_3dstate_vs },
   { _3DSTATE_GS, handle_3dstate_vs },
   { _3DSTATE_DS, handle_3dstate_vs },
   { _3DSTATE_HS, handle_3dstate_hs },
   { _3DSTATE_CONSTANT_VS, handle_3dstate_constant },
   { _3DSTATE_CONSTANT_GS, handle_3dstate_constant },
   { _3DSTATE_CONSTANT_PS, handle_3dstate_constant },
   { _3DSTATE_CONSTANT_HS, handle_3dstate_constant },
   { _3DSTATE_CONSTANT_DS, handle_3dstate_constant },
   { _3DSTATE_PS, handle_3dstate_ps },

   { _3DSTATE_BINDING_TABLE_POINTERS_VS, handle_3dstate_binding_table_pointers },
   { _3DSTATE_BINDING_TABLE_POINTERS_HS, handle_3dstate_binding_table_pointers },
   { _3DSTATE_BINDING_TABLE_POINTERS_DS, handle_3dstate_binding_table_pointers },
   { _3DSTATE_BINDING_TABLE_POINTERS_GS, handle_3dstate_binding_table_pointers },
   { _3DSTATE_BINDING_TABLE_POINTERS_PS, handle_3dstate_binding_table_pointers },

   { _3DSTATE_SAMPLER_STATE_POINTERS_VS, handle_3dstate_sampler_state_pointers },
   { _3DSTATE_SAMPLER_STATE_POINTERS_GS, handle_3dstate_sampler_state_pointers },
   { _3DSTATE_SAMPLER_STATE_POINTERS_PS, handle_3dstate_sampler_state_pointers },

   { _3DSTATE_VIEWPORT_STATE_POINTERS_CC, handle_3dstate_viewport_state_pointers_cc },
   { _3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP, handle_3dstate_viewport_state_pointers_sf_clip },
   { _3DSTATE_BLEND_STATE_POINTERS, handle_3dstate_blend_state_pointers },
   { _3DSTATE_CC_STATE_POINTERS, handle_3dstate_cc_state_pointers },
   { _3DSTATE_SCISSOR_STATE_POINTERS, handle_3dstate_scissor_state_pointers },
   { _MI_LOAD_REGISTER_IMM, handle_load_register_imm }
};

static void
parse_commands(struct aub_batch_decoder *decoder, uint32_t *cmds, int size, int engine)
{
   uint32_t *p, *end = cmds + size / 4;
   unsigned int length, i;
   struct aub_group *inst;

   for (p = cmds; p < end; p += length) {
      inst = aub_spec_find_instruction(decoder->spec, p);
      if (inst == NULL) {
         printf("unknown instruction %08x\n", p[0]);
         length = (p[0] & 0xff) + 2;
         continue;
      }
      length = aub_group_get_length(inst, p);

      const char *color, *reset_color = CLEAR_TO_EOL NORMAL;
      uint64_t offset;

      if (decoder->full_decode)
         color = HEADER;
      else
         color = NORMAL;

      if (decoder->print_color == AUB_PRINT_COLOR_NEVER) {
         color = "";
         reset_color = "";
      }

      if (decoder->print_offsets)
         offset = (void *) p - decoder->gtt;
      else
         offset = 0;

      printf("%s0x%08lx:  0x%08x:  %s%s\n",
             color, offset, p[0],
             aub_group_get_name(inst), reset_color);

      if (decoder->full_decode) {
         struct aub_field_iterator iter;
         char *token = NULL;
         int idx = 0, dword_num = 0;
         aub_field_iterator_init(&iter, inst, p);
         while (aub_field_iterator_next(&iter)) {
            idx = 0;
            print_dword_val(decoder, &iter, offset, &dword_num);
            if (dword_num > 0)
               token = print_iterator_values(decoder, &iter, &idx);
            if (token != NULL) {
                printf("0x%08lx:  0x%08x : Dword %d\n",
                       offset + 4 * idx, p[idx], idx);
                handle_struct_decode(decoder,token, &p[idx]);
                token = NULL;
            }
         }

         for (i = 0; i < ARRAY_LENGTH(custom_handlers); i++) {
            if (aub_group_get_opcode(inst) ==
                custom_handlers[i].opcode)
               custom_handlers[i].handle(decoder, p);
         }
      }

      if ((p[0] & 0xffff0000) == AUB_MI_BATCH_BUFFER_START) {
         uint64_t start;
         if (aub_spec_get_gen(decoder->spec) >= aub_make_gen(8,0))
            start = get_qword(&p[1]);
         else
            start = p[1];

         parse_commands(decoder, decoder->gtt + start, 1 << 20, engine);
      } else if ((p[0] & 0xffff0000) == AUB_MI_BATCH_BUFFER_END) {
         break;
      }
   }
}

#define AUB_ENGINE_RENDER 1
#define AUB_ENGINE_BLITTER 2

static void
handle_trace_block(struct aub_batch_decoder *decoder, uint32_t *p)
{
   int operation = p[1] & AUB_TRACE_OPERATION_MASK;
   int type = p[1] & AUB_TRACE_TYPE_MASK;
   int address_space = p[1] & AUB_TRACE_ADDRESS_SPACE_MASK;
   uint64_t offset = p[3];
   uint32_t size = p[4];
   int header_length = p[0] & 0xffff;
   uint32_t *data = p + header_length + 2;
   int engine = AUB_ENGINE_RENDER;

   if (aub_spec_get_gen(decoder->spec) >= aub_make_gen(8,0))
      offset += (uint64_t) p[5] << 32;

   switch (operation) {
   case AUB_TRACE_OP_DATA_WRITE:
      if (address_space != AUB_TRACE_MEMTYPE_GTT)
         break;
      aub_batch_decoder_write_data(decoder, data, size, offset);
      break;
   case AUB_TRACE_OP_COMMAND_WRITE:
      switch (type) {
      case AUB_TRACE_TYPE_RING_PRB0:
         engine = AUB_ENGINE_RENDER;
         break;
      case AUB_TRACE_TYPE_RING_PRB2:
         engine = AUB_ENGINE_BLITTER;
         break;
      default:
         printf("command write to unknown ring %d\n", type);
         break;
      }

      parse_commands(decoder, data, size, engine);
      decoder->gtt_end = 0;
      break;
   }
}

#define TYPE(dw)       (((dw) >> 29) & 7)
#define OPCODE(dw)     (((dw) >> 23) & 0x3f)
#define SUBOPCODE(dw)  (((dw) >> 16) & 0x7f)

#define MAKE_HEADER(type, opcode, subopcode) \
                   (((type) << 29) | ((opcode) << 23) | ((subopcode) << 16))

#define TYPE_AUB            0x7

/* Classic AUB opcodes */
#define OPCODE_AUB          0x01
#define SUBOPCODE_HEADER    0x05
#define SUBOPCODE_BLOCK     0x41
#define SUBOPCODE_BMP       0x1e

/* Newer version AUB opcode */
#define OPCODE_NEW_AUB      0x2e
#define SUBOPCODE_VERSION   0x00
#define SUBOPCODE_REG_WRITE 0x03
#define SUBOPCODE_MEM_POLL  0x05
#define SUBOPCODE_MEM_WRITE 0x06

#define MAKE_GEN(major, minor) ( ((major) << 8) | (minor) )

struct {
   const char *name;
   uint32_t gen;
} device_map[] = {
   { "bwr", MAKE_GEN(4, 0) },
   { "cln", MAKE_GEN(4, 0) },
   { "blc", MAKE_GEN(4, 0) },
   { "ctg", MAKE_GEN(4, 0) },
   { "el", MAKE_GEN(4, 0) },
   { "il", MAKE_GEN(4, 0) },
   { "sbr", MAKE_GEN(6, 0) },
   { "ivb", MAKE_GEN(7, 0) },
   { "lrb2", MAKE_GEN(0, 0) },
   { "hsw", MAKE_GEN(7, 5) },
   { "vlv", MAKE_GEN(7, 0) },
   { "bdw", MAKE_GEN(8, 0) },
   { "skl", MAKE_GEN(9, 0) },
   { "chv", MAKE_GEN(8, 0) },
   { "bxt", MAKE_GEN(9, 0) }
};

static void
aub_batch_decoder_decode_aub_item(struct aub_batch_decoder *decoder)
{
   uint32_t *p, h, device, data_type;
   int header_length, payload_size, bias;

   p = decoder->cursor;
   h = *p;
   header_length = h & 0xffff;

   switch (OPCODE(h)) {
   case OPCODE_AUB:
      bias = 2;
      break;
   case OPCODE_NEW_AUB:
      bias = 1;
      break;
   default:
      printf("unknown opcode %d at %ld/%ld\n",
             OPCODE(h), decoder->cursor - decoder->start,
             decoder->end - decoder->start);
      decoder->cursor = decoder->end;
      return;
   }

   payload_size = 0;
   switch (h & 0xffff0000) {
   case MAKE_HEADER(TYPE_AUB, OPCODE_AUB, SUBOPCODE_HEADER):
      payload_size = p[12];
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_AUB, SUBOPCODE_BLOCK):
      payload_size = p[4];
      handle_trace_block(decoder, p);
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_AUB, SUBOPCODE_BMP):
      break;

   case MAKE_HEADER(TYPE_AUB, OPCODE_NEW_AUB, SUBOPCODE_VERSION):
      printf("version block: dw1 %08x\n", p[1]);
      device = (p[1] >> 8) & 0xff;
      printf("  device %s\n", device_map[device].name);
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_NEW_AUB, SUBOPCODE_REG_WRITE):
      printf("register write block: (dwords %d)\n", h & 0xffff);
      printf("  reg 0x%x, data 0x%x\n", p[1], p[5]);
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_NEW_AUB, SUBOPCODE_MEM_WRITE):
      printf("memory write block (dwords %d):\n", h & 0xffff);
      printf("  address 0x%lx\n", *(uint64_t *) &p[1]);
      data_type = (p[3] >> 20) & 0xff;
      if (data_type != 0)
         printf("  data type 0x%x\n", data_type);
      printf("  address space 0x%x\n", (p[3] >> 28) & 0xf);
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_NEW_AUB, SUBOPCODE_MEM_POLL):
      printf("memory poll block (dwords %d):\n", h & 0xffff);
      break;
   default:
      printf("unknown block type=0x%x, opcode=0x%x, "
             "subopcode=0x%x (%08x)\n", TYPE(h), OPCODE(h), SUBOPCODE(h), h);
      break;
   }
   decoder->cursor = p + header_length + bias + payload_size / 4;
}

void aub_batch_decoder_write_data(struct aub_batch_decoder *decoder,
                                  void *data, size_t length,
                                  uint64_t offset)
{
   if (decoder->gtt_size < offset + length) {
      fprintf(stderr, "overflow gtt space: %s", strerror(errno));
      exit(EXIT_FAILURE);
   }

   memcpy((char *) decoder->gtt + offset, data, length);
   if (decoder->gtt_end < offset + length)
      decoder->gtt_end = offset + length;
}

void aub_batch_decoder_decode_aub(struct aub_batch_decoder *decoder,
                                  void *batch, size_t length,
                                  FILE *out)
{
   decoder->start = batch;
   decoder->end = batch + length;
   decoder->cursor = decoder->start;
   decoder->out = out;

   while (decoder->cursor < decoder->end)
      aub_batch_decoder_decode_aub_item(decoder);
}

bool aub_batch_decoder_init(struct aub_batch_decoder *decoder,
                            const struct gen_device_info *devinfo)
{
   decoder->spec = aub_spec_load(devinfo);
   decoder->disasm = aub_disasm_create(devinfo);

   /* mmap a terabyte for our gtt space. */
   decoder->gtt_size = 1ul << 40;
   decoder->gtt = mmap(NULL, decoder->gtt_size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS |  MAP_NORESERVE, -1, 0);
   if (decoder->gtt == MAP_FAILED) {
      fprintf(stderr, "failed to alloc gtt space: %s", strerror(errno));
      return false;
   }

   return true;
}

void aub_batch_decoder_fini(struct aub_batch_decoder *decoder)
{
   if (decoder->disasm)
      aub_disasm_destroy(decoder->disasm);
   if (decoder->gtt)
      munmap(decoder->gtt, decoder->gtt_size);
}
