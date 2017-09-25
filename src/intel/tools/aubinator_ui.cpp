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
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "common/gen_decoder.h"
#include "intel_aub.h"
#include "isl/isl.h"
#include "gen_disasm.h"
#include "memory.h"
#include "util/list.h"
#include "util/ralloc.h"

#include "imgui.h"
#include "imgui_impl_gtk3_cogl.h"
#include "aubinator_imgui_widgets.h"

#define xtzalloc(__type) ((__type *) calloc(1, sizeof(__type)))
#define xtalloc(__type) ((__type *) malloc(sizeof(__type)))

enum shader_stage {
   STAGE_VS,
   STAGE_GS,
   STAGE_DS,
   STAGE_HS,
   STAGE_PS,
   STAGE_MEDIA,
   STAGE_COUNT
};

struct offset_at_base {
   const uint64_t *base;
   const char *base_name;
   char address[12];
   uint64_t offset;
};

struct window {
   struct list_head windows;

   char name[128];
   bool opened;

   ImVec2 position;
   ImVec2 size;

   void (*display)(struct window*);
   void (*destroy)(struct window*);

   void (*reset)(struct window*);
};

struct decode_options {
   struct ImGuiTextFilter field_filter;

   bool drop_filtered;
   bool show_dwords;
};

struct as_window {
   struct window base;

   struct gpu_state *state;
   struct offset_at_base offset;

   struct gen_group *as;

   struct decode_options options;
};

struct batch_window {
   struct window base;

   bool collapsed;

   uint32_t *ring_buffer;

   struct ImGuiTextFilter cmd_filter;
   struct decode_options options;
};

struct image_window {
   struct window base;

   char shader[1024];

   struct gpu_state *state;
   struct offset_at_base offset;

   uint64_t base_address;
   char base_address_buf[20];

   int width;
   int height;
   int stride;
   int plane_stride;
   enum isl_format format;
   uint32_t tiling;

   CoglPipeline *pipeline;
};

struct pattern_window {
   struct window base;

   ImVec2 points[31];
   int ms_offset;
};

struct shader_window {
   struct window base;

   char *shader;
   size_t shader_len;

   struct gpu_state *state;
   struct offset_at_base offset;
};

struct state_window {
   struct window base;

   struct gpu_state *state;

   bool collapsed;
   struct ImGuiTextFilter cmd_filter;
   struct decode_options options;
};

struct urb_window {
   struct window base;

   struct gpu_state *state;

   uint32_t max_allocation;

   struct {
      uint32_t offset;
      uint32_t length;
      ImColor color;
   } allocations[STAGE_COUNT];

   struct {
      uint32_t offset;
      uint32_t length;
      ImColor color;
   } constants[STAGE_COUNT];
};

struct gpu_state_item {
   uint32_t refcount;

   memory_t *gtt;
   uint64_t gtt_offset;
   struct gen_group *group;
};

struct gpu_state {
   uint64_t general_state_base;
   uint64_t surface_state_base;
   uint64_t dynamic_state_base;
   uint64_t instruction_base;
   uint64_t instruction_bound;

   struct hash_table *items;

   uint32_t batch;
   uint64_t gtt_offset;

   memory_t *gtt;
};

static struct {
   struct aub_file *file;
   char *input_file;
   char *xml_path;

   memory_t *gtt;

   /* Device state */
   uint16_t pci_id = 0;
   bool read_pci_id;
   char app_name[33];
   struct gen_device_info devinfo;
   struct gen_spec *spec;
   struct gen_disasm *disasm;

   /* Custom handlers */
   struct hash_table *accumulation_handlers;
   struct hash_table *display_handlers;

   /* Decoding state */
   int32_t command_index;
   uint32_t *command_block;

    /* Snapshots of GPU states at each point of interest (usually
     * 3DPRIMITIVEs, but we could add more later...), indexed by gtt offset.
     */
   struct hash_table_u64 *gpu_states;
   struct gpu_state *last_gpu_state;

   /* UI state*/
   bool show_commands_window;
   bool show_registers_window;

   ImVec4 clear_color;
   ImVec4 dwords_color;
   ImVec4 highlight_color;
   ImVec4 error_color;

   struct list_head windows;
   struct window *focused_window;

   struct window file_window;
   struct window commands_window;
   struct window registers_window;
} context;

/**/

static struct gpu_state_item *
gpu_state_item_ref(struct gpu_state_item *item)
{
   item->refcount++;
   return item;
}

static void
gpu_state_item_unref(struct gpu_state_item *item)
{
   if (--item->refcount == 0) {
      memory_unref(item->gtt);
      free(item);
   }
}

static struct gpu_state_item *
gpu_state_item_new(memory_t *gtt, uint64_t gtt_offset,
                   struct gen_group *group)
{
   struct gpu_state_item *item = xtzalloc(struct gpu_state_item);

   item->refcount = 1;
   item->gtt = memory_ref(gtt);
   item->gtt_offset = gtt_offset;
   item->group = group;

   return item;
}

static struct gpu_state *
gpu_state_new(void)
{
   struct gpu_state *state = xtzalloc(struct gpu_state);

   state->gtt = memory_new();
   state->items = _mesa_hash_table_create(NULL, _mesa_hash_string,
                                          _mesa_key_string_equal);

   return state;
}

static struct gpu_state *
gpu_state_copy(struct gpu_state *parent)
{
   struct gpu_state *state = xtalloc(struct gpu_state);
   struct hash_entry *entry;

   memcpy(state, parent, sizeof(*state));

   state->batch = parent->batch;
   state->gtt_offset = parent->gtt_offset;
   state->gtt = memory_ref(parent->gtt);
   state->items = _mesa_hash_table_create(NULL, _mesa_hash_string,
                                          _mesa_key_string_equal);
   hash_table_foreach(parent->items, entry) {
      _mesa_hash_table_insert(state->items,
                              entry->key,
                              gpu_state_item_ref((struct gpu_state_item *) entry->data));
   }

   return state;
}

static void
gpu_state_free(struct gpu_state *state)
{
   if (!state) return;

   struct hash_entry *entry;
   hash_table_foreach(state->items, entry)
      gpu_state_item_unref((struct gpu_state_item *) entry->data);
   _mesa_hash_table_destroy(state->items, NULL);
   memory_unref(state->gtt);
   free(state);
}

static void
gpu_state_add(struct gpu_state *state,
              memory_t *gtt, uint64_t gtt_offset,
              struct gen_group *group)
{
   struct hash_entry *previous_entry =
      _mesa_hash_table_search(state->items, group->name);
   if (previous_entry)
      gpu_state_item_unref((struct gpu_state_item *) previous_entry->data);
   _mesa_hash_table_insert(state->items,
                           group->name,
                           gpu_state_item_new(gtt, gtt_offset, group));
}

static struct gpu_state_item *
gpu_state_find(struct gpu_state *state, const char *name)
{
   struct hash_entry *entry =
      _mesa_hash_table_search(state->items, name);
   return entry ? ((struct gpu_state_item *) entry->data) : NULL;
}

static struct gpu_state *
gpu_state_at_offset(uint64_t gtt_offset)
{
   return (struct gpu_state *)
      _mesa_hash_table_u64_search(context.gpu_states, gtt_offset);
}

/**/

static void new_image_window(struct gpu_state *state, uint64_t offset);
static void new_pattern_window(const struct gen_dword_reader *reader);
static void new_pattern_window(struct gpu_state *state);
static void new_shader_window(struct gpu_state *state, uint64_t offset, const char *type);
static void new_state_window(struct gpu_state *state);
static void new_urb_window(struct gpu_state *state);

static bool
has_ctrl_key(char key)
{
   return ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_COUNT + key);
}

static bool
has_ctrl_imgui_key(ImGuiKey key)
{
   return ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(key);
}

static bool
window_has_ctrl_key(char key)
{
   return ImGui::IsRootWindowOrAnyChildFocused() && has_ctrl_key(key);
}

static void
build_window_title(struct window *win, const char *title, struct gpu_state *state)
{
   if (state == NULL)
      snprintf(win->name, sizeof(win->name), "%s (live)##%p", title, win);
   else
      snprintf(win->name, sizeof(win->name), "%s (batch %i, gtt %" PRIx64 ")##%p",
               title, state->batch, state->gtt_offset, win);
}

/**/

const struct {
   const char *name;
   int pci_id;
} supported_gens[] = {
   { "ilk", 0x0046 }, /* Intel(R) Ironlake Mobile */
   { "snb", 0x0126 }, /* Intel(R) Sandybridge Mobile GT2 */
   { "ivb", 0x0166 }, /* Intel(R) Ivybridge Mobile GT2 */
   { "hsw", 0x0416 }, /* Intel(R) Haswell Mobile GT2 */
   { "byt", 0x0155 }, /* Intel(R) Bay Trail */
   { "bdw", 0x1616 }, /* Intel(R) HD Graphics 5500 (Broadwell GT2) */
   { "chv", 0x22B3 }, /* Intel(R) HD Graphics (Cherryview) */
   { "skl", 0x1912 }, /* Intel(R) HD Graphics 530 (Skylake GT2) */
   { "kbl", 0x591D }, /* Intel(R) Kabylake GT2 */
   { "bxt", 0x0A84 }, /* Intel(R) HD Graphics (Broxton) */
   { "cnl", 0x5A52 }, /* Intel(R) HD Graphics (Cannonlake) */
};

/**/

// static inline uint32_t
// field(uint32_t value, int start, int end)
// {
//    uint32_t mask;

//    mask = ~0U >> (31 - end + start);

//    return (value >> start) & mask;
// }

struct brw_instruction;

static uint32_t reader_cb(void *user_data, uint32_t dword_offset);
static uint64_t reader_offset(const struct gen_dword_reader *_reader);

struct aubinator_reader {
   struct gen_dword_reader base;

   enum {
      MEMORY,
      OFFSET,
      POINTER
   } type;

   union {
      struct {
         memory_t *memory;
         uint64_t offset;
      };
      struct {
         const struct gen_dword_reader *parent;
         uint32_t dword_offset;
      };
      struct {
         uint32_t *ptr;
      };
   };

   aubinator_reader() {
      this->base.read = reader_cb;
      this->base.user_data = this;
   }

   aubinator_reader(memory_t *memory, uint64_t offset) : aubinator_reader() {
      this->type = MEMORY;
      this->memory = memory;
      this->offset = offset;
   }

   aubinator_reader(const struct gen_dword_reader *parent, uint32_t dword_offset)
      : aubinator_reader() {
      this->type = OFFSET;
      this->parent = parent;
      this->dword_offset = dword_offset;
   }

   aubinator_reader(uint32_t *ptr) : aubinator_reader() {
      this->type = POINTER;
      this->ptr = ptr;
   }

   uint32_t read_dword(uint32_t dword_offset) const {
      switch (this->type) {
      case MEMORY:
         return memory_read_dword(this->memory, this->offset + 4 * dword_offset);
      case OFFSET:
         return gen_read_dword(this->parent, this->dword_offset + dword_offset);
      case POINTER:
         return this->ptr[dword_offset];
      }
      unreachable("Invalid memory");
   }

   uint64_t get_offset() const {
      switch (this->type) {
      case MEMORY: return this->offset;
      case OFFSET: return reader_offset(this->parent) + 4 * this->dword_offset;
      case POINTER: return 0;
      }
      unreachable("Invalid memory");
   }

   const struct gen_dword_reader *gen() const { return &this->base; }
};

static uint64_t
reader_offset(const struct gen_dword_reader *_reader)
{
   const struct aubinator_reader *reader = (const struct aubinator_reader *) _reader;
   return reader->get_offset();
}

static uint32_t
reader_cb(void *user_data, uint32_t dword_offset)
{
   const struct aubinator_reader *reader = (const struct aubinator_reader *) user_data;
   return reader->read_dword(dword_offset);
}

/**/

static inline int
gtt_valid_offset(memory_t *mem, uint64_t offset)
{
   return (offset >= memory_start(mem) && offset < memory_end(mem));
}

static inline int
gtt_valid_range(memory_t *mem, uint64_t offset, uint32_t size)
{
   return (offset >= memory_start(mem) && offset < memory_end(mem) &&
           (offset + size) < memory_end(mem));
}

static void
decode_group(struct gen_group *strct,
             const struct gen_dword_reader *reader,
             const struct decode_options *options)
{
   uint64_t offset = reader_offset(reader);
   struct gen_field_iterator iter;
   int last_dword = -1;

   gen_field_iterator_init(&iter, strct, reader, false);
   do {
      // Display dword offset.
      if (options->show_dwords && last_dword != iter.dword) {
         for (int i = last_dword + 1; i <= iter.dword; i++) {
            ImGui::TextColored(context.dwords_color,
                               "0x%08" PRIx64 ":  0x%08x : Dword %d",
                               offset + 4 * i, gen_read_dword(reader, i), i);
         }
         last_dword = iter.dword;
      }

      // Skip header fields.
      if (gen_field_is_header(iter.field))
         continue;

      bool pass = (options->field_filter.PassFilter(iter.name) ||
                   options->field_filter.PassFilter(iter.value));
      bool highlight = options->field_filter.IsActive() && pass;
      if (highlight) ImGui::PushStyleColor(ImGuiCol_Text, context.highlight_color);
      if (!options->drop_filtered || pass) {
         if (!iter.struct_desc) {
            ImGui::Text("   %s: ", iter.name); ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, context.dwords_color);
            ImGui::Text("%s", iter.value);
            ImGui::PopStyleColor();
         } else if (ImGui::TreeNodeEx((void *) (offset + iter.dword * 4),
                                      ImGuiTreeNodeFlags_DefaultOpen,
                                      "   %s: %s", iter.name, iter.value)) {
            struct aubinator_reader child_reader(reader, iter.dword);
            decode_group(iter.struct_desc, child_reader.gen(), options);
            ImGui::TreePop();
         }
      } else {
         if (iter.struct_desc && ImGui::TreeNodeEx((void *) (offset + iter.dword * 4),
                                                   ImGuiTreeNodeFlags_DefaultOpen,
                                                   "   %s: %s", iter.name, iter.value)) {
            struct aubinator_reader child_reader(reader, iter.dword);
            decode_group(iter.struct_desc, child_reader.gen(), options);
            ImGui::TreePop();
         }
      }

      if (highlight) ImGui::PopStyleColor();
   } while (gen_field_iterator_next(&iter));
}

// static void
// handle_3dstate_index_buffer(struct gen_spec *spec, uint32_t *p)
// {
//    void *start;
//    uint32_t length, i, type, size;

//    start = gtt_at32(p[2]);
//    type = (p[1] >> 8) & 3;
//    size = 1 << type;
//    length = p[4] / size;
//    if (length > 10)
//       length = 10;

//    fprintf(stdout, "\t");

//    for (i = 0; i < length; i++) {
//       switch (type) {
//       case 0:
//          fprintf(stdout, "%3d ", ((uint8_t *)start)[i]);
//          break;
//       case 1:
//          fprintf(stdout, "%3d ", ((uint16_t *)start)[i]);
//          break;
//       case 2:
//          fprintf(stdout, "%3d ", ((uint32_t *)start)[i]);
//          break;
//       }
//    }
//    if (length < p[4] / size)
//       fprintf(stdout, "...\n");
//    else
//       fprintf(stdout, "\n");
// }

static inline uint64_t
read_address(const struct gen_dword_reader *reader,
             uint32_t offset)
{
   /* Addresses are always guaranteed to be page-aligned and sometimes
    * hardware packets have extra stuff stuffed in the bottom 12 bits.
    */
   uint64_t addr = gen_read_dword(reader, offset) & ~0xfffu;

   if (gen_spec_get_gen(context.spec) >= gen_make_gen(8,0)) {
      /* On Broadwell and above, we have 48-bit addresses which consume two
       * dwords.  Some packets require that these get stored in a "canonical
       * form" which means that bit 47 is sign-extended through the upper
       * bits. In order to correctly handle those aub dumps, we need to mask
       * off the top 16 bits.
       */
      addr |= ((uint64_t)gen_read_dword(reader, offset + 1) & 0xffff) << 32;
   }

   return addr;
}

static inline uint64_t
read_offset(const struct gen_dword_reader *reader,
            uint32_t dw_offset, uint32_t start, uint32_t end)
{
   assert(start <= end);
   assert(end < 64);

   uint64_t mask = (~0ull >> (64 - (end - start + 1))) << start;

   uint64_t offset = gen_read_dword(reader, dw_offset);
   if (end >= 32)
      offset |= (uint64_t) gen_read_dword(reader, dw_offset + 1) << 32;

   return offset & mask;
}

static void
address_maybe_update(struct gen_group *group,
                     const char *enable, const char *field_name,
                     const struct gen_dword_reader *reader,
                     uint64_t *address)
{
   struct gen_field *field = gen_group_find_field(group, enable);
   if (!field)
      return;

   union gen_field_value value = { .u64 = 0 };
   gen_field_decode(field, reader, &value);
   if (!value.b32)
      return;

   field = gen_group_find_field(group, field_name);
   assert(field);

   gen_field_decode(field, reader, &value);
   *address = value.u64;
}

static void
handle_state_base_address(struct gpu_state *state,
                          const struct gen_dword_reader *reader,
                          const struct decode_options *)
{
   struct gen_group *group =
      gen_spec_find_instruction_by_name(context.spec, "STATE_BASE_ADDRESS");

   address_maybe_update(group,
                        "General State Base Address Modify Enable",
                        "General State Base Address",
                        reader, &state->general_state_base);
   address_maybe_update(group,
                        "Surface State Base Address Modify Enable",
                        "Surface State Base Address",
                        reader, &state->surface_state_base);
   address_maybe_update(group,
                        "Dynamic State Base Address Modify Enable",
                        "Dynamic State Base Address",
                        reader, &state->dynamic_state_base);
   address_maybe_update(group,
                        "Instruction Base Address Modify Enable",
                        "Instruction Base Address",
                        reader, &state->instruction_base);
   address_maybe_update(group,
                        "Instruction Buffer size Modify Enable",
                        "Instruction Buffer Size",
                        reader, &state->instruction_bound);
}

static void
snapshot_gpu_state(struct gpu_state *,
                   const struct aubinator_reader& reader,
                   const struct decode_options *)
{
   context.last_gpu_state = gpu_state_copy(context.last_gpu_state);
   context.last_gpu_state->batch = context.command_index;
   context.last_gpu_state->gtt_offset = reader.get_offset();
   _mesa_hash_table_u64_insert(context.gpu_states,
                               context.last_gpu_state->gtt_offset,
                               context.last_gpu_state);
}

static void
dump_binding_table(struct gpu_state *state, uint64_t offset,
                   const struct decode_options *options)
{
   struct gen_group *surface_state =
      gen_spec_find_struct(context.spec, "RENDER_SURFACE_STATE");
   if (surface_state == NULL) {
      fprintf(stderr, "did not find RENDER_SURFACE_STATE info\n");
      return;
   }

   uint64_t surfaces_offset = state->surface_state_base + offset;
   for (uint32_t i = 0; i < 16; i++) {
      uint32_t surface_offset =
         memory_read_dword(state->gtt, surfaces_offset + i * 4);
      if (surface_offset == 0)
         continue;
      uint64_t surface_state_offset = state->surface_state_base + surface_offset;
      if (!gtt_valid_offset(state->gtt, surface_state_offset)) {
         ImGui::TextColored(context.error_color,
                            "pointer %u: %08lx <not valid>",
                            i, surface_state_offset);
         continue;
      }

      if (ImGui::TreeNodeEx((void *) surface_state_offset,
                            ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen,
                            "pointer %u: %08lx", i, surface_state_offset)) {
         char button_name[64];
         snprintf(button_name, sizeof(button_name),
                  "Show as image##%lx", surface_state_offset);
         if (ImGui::Button(button_name))
            new_image_window(state, surface_state_offset);

         struct aubinator_reader reader(state->gtt, surface_state_offset);
         decode_group(surface_state, reader.gen(), options);
         ImGui::TreePop();
      }
   }
}

static void
dump_samplers(struct gpu_state *state, uint64_t offset,
              const struct decode_options *options)
{
   struct gen_group *sampler_state =
      gen_spec_find_struct(context.spec, "SAMPLER_STATE");

   uint64_t samplers_offset = state->dynamic_state_base + offset;
   for (uint32_t i = 0; i < 4; i++) {
      uint64_t sampler_state_offset = samplers_offset + i * 16;
      if (ImGui::TreeNodeEx((void *) sampler_state_offset,
                            ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen,
                            "sampler state %d", i)) {
         struct aubinator_reader reader(state->gtt, sampler_state_offset);
         decode_group(sampler_state, reader.gen(), options);
         ImGui::TreePop();
      }
   }
}

static void
handle_media_interface_descriptor_load(struct gpu_state *state,
                                       const struct aubinator_reader& reader,
                                       const struct decode_options *options)
{
   struct gen_group *descriptor_structure =
      gen_spec_find_struct(context.spec, "INTERFACE_DESCRIPTOR_DATA");

   if (!descriptor_structure) {
      fprintf(stdout, "did not find INTERFACE_DESCRIPTOR_DATA info\n");
      return;
   }

   uint64_t start = state->dynamic_state_base + gen_read_dword(reader.gen(), 3);
   unsigned length = gen_read_dword(reader.gen(), 2) / 32;
   for (unsigned i = 0; i < length; i++) {
      ImGui::Text("descriptor %u:", i);
      struct aubinator_reader child_reader(state->gtt, start + 32 * i);
      decode_group(descriptor_structure, child_reader.gen(), options);

      start = state->instruction_base + gen_read_dword(child_reader.gen(), 0);
      if (!gtt_valid_offset(state->gtt, start)) {
         ImGui::Text("kernel: %08" PRIx64 " <not valid>\n", start);
         continue;
      }

      char button_name[64];
      snprintf(button_name, sizeof(button_name), "Kernel##%" PRIx64, start);
      if (ImGui::Button(button_name))
         new_shader_window(state, start, "CS/Media");

      dump_samplers(state, gen_read_dword(child_reader.gen(), 3) & ~0x1f, options);
      dump_binding_table(state, gen_read_dword(child_reader.gen(), 4) & ~0x1f, options);
   }
}

// /* Heuristic to determine whether a uint32_t is probably actually a float
//  * (http://stackoverflow.com/a/2953466)
//  */

// static bool
// probably_float(uint32_t bits)
// {
//    int exp = ((bits & 0x7f800000U) >> 23) - 127;
//    uint32_t mant = bits & 0x007fffff;

//    /* +- 0.0 */
//    if (exp == -127 && mant == 0)
//       return true;

//    /* +- 1 billionth to 1 billion */
//    if (-30 <= exp && exp <= 30)
//       return true;

//    /* some value with only a few binary digits */
//    if ((mant & 0x0000ffff) == 0)
//       return true;

//    return false;
// }

// static void
// handle_3dstate_vertex_buffers(struct gen_spec *spec, uint32_t *p)
// {
//    uint32_t *end, *s, *dw, *dwend;
//    uint64_t offset;
//    int n, i, count, stride;

//    end = (p[0] & 0xff) + p + 2;
//    for (s = &p[1], n = 0; s < end; s += 4, n++) {
//       if (gen_spec_get_gen(spec) >= gen_make_gen(8, 0)) {
//          offset = *(uint64_t *) &s[1];
//          dwend = gtt_at32(offset + s[3]);
//       } else {
//          offset = s[1];
//          dwend = gtt_at32(s[2] + 1);
//       }

//       stride = field(s[0], 0, 11);
//       count = 0;
//       fprintf(stdout, "vertex buffer %d, size %d\n", n, s[3]);
//       for (dw = gtt_at32(offset), i = 0; dw < dwend && i < 256; dw++) {
//          if (count == 0 && count % (8 * 4) == 0)
//             fprintf(stdout, "  ");

//          if (probably_float(*dw))
//             fprintf(stdout, "  %8.2f", *(float *) dw);
//          else
//             fprintf(stdout, "  0x%08x", *dw);

//          i++;
//          count += 4;

//          if (count == stride) {
//             fprintf(stdout, "\n");
//             count = 0;
//          } else if (count % (8 * 4) == 0) {
//             fprintf(stdout, "\n");
//          } else {
//             fprintf(stdout, " ");
//          }
//       }
//       if (count > 0 && count % (8 * 4) != 0)
//          fprintf(stdout, "\n");
//    }
// }

// static void
// handle_3dstate_constant(struct gen_spec *spec, uint32_t *p)
// {
//    int i, j, length;
//    uint32_t *dw;
//    float *f;

//    for (i = 0; i < 4; i++) {
//       length = (p[1 + i / 2] >> (i & 1) * 16) & 0xffff;
//       f = (float *) (gtt_at32(p[3 + i * 2] + dynamic_state_base));
//       dw = (uint32_t *) f;
//       for (j = 0; j < length * 8; j++) {
//          if (probably_float(dw[j]))
//             fprintf(stdout, "  %04.3f", f[j]);
//          else
//             fprintf(stdout, "  0x%08x", dw[j]);

//          if ((j & 7) == 7)
//             fprintf(stdout, "\n");
//       }
//    }
// }

static void
handle_3dstate_vs(struct gpu_state *state,
                  const struct aubinator_reader& reader,
                  const struct decode_options *)
{
   uint64_t start;
   int vs_enable;

   if (gen_spec_get_gen(context.spec) >= gen_make_gen(8, 0)) {
      start = read_offset(reader.gen(), 1, 6, 63);
      vs_enable = gen_read_dword(reader.gen(), 7) & 1;
   } else {
      start = read_offset(reader.gen(), 1, 6, 31);
      vs_enable = gen_read_dword(reader.gen(), 5) & 1;
   }

   if (vs_enable) {
      char button_name[30];
      snprintf(button_name, sizeof(button_name), "Kernel##%" PRIx64, start);
      if (ImGui::Button(button_name))
         new_shader_window(state, start, "VS/GS/DS");
   }
}

static void
handle_3dstate_hs(struct gpu_state *state,
                  const struct aubinator_reader& reader,
                  const struct decode_options *)
{
   uint64_t start;
   int hs_enable;

   if (gen_spec_get_gen(context.spec) >= gen_make_gen(8, 0)) {
      start = read_offset(reader.gen(), 3, 6, 63);
   } else {
      start = read_offset(reader.gen(), 3, 6, 31);
   }

   hs_enable = gen_read_dword(reader.gen(), 2) & 0x80000000;

   if (hs_enable) {
      char button_name[30];
      snprintf(button_name, sizeof(button_name), "Kernel##%" PRIx64, start);
      if (ImGui::Button(button_name))
         new_shader_window(state, start, "HS");
   }
}

static void
handle_3dstate_ps(struct gpu_state *state,
                  const struct aubinator_reader& reader,
                  const struct decode_options *)
{
   uint32_t mask = ~((1 << 6) - 1);
   static const char *pixel_type[3] = {"8 pixel", "16 pixel", "32 pixel"};
   uint32_t k_mask, k_offsets[3] = { 1, 0, 0 };
   const char *used[3] = { NULL, NULL, NULL };

   if (gen_spec_get_gen(context.spec) >= gen_make_gen(8, 0)) {
      k_mask = gen_read_dword(reader.gen(), 6) & 7;
      k_offsets[1] = 8;
      k_offsets[2] = 10;
   } else {
      k_mask = gen_read_dword(reader.gen(), 4) & 7;
      k_offsets[1] = 6;
      k_offsets[2] = 7;
   }

#define DISPATCH_8 1
#define DISPATCH_16 2
#define DISPATCH_32 4
   switch (k_mask) {
   case DISPATCH_8:
      used[0] = pixel_type[0];
      break;
   case DISPATCH_16:
      used[0] = pixel_type[1];
      break;
   case DISPATCH_8 | DISPATCH_16:
      used[0] = pixel_type[0];
      used[2] = pixel_type[1];
      break;
   case DISPATCH_32:
      used[0] = pixel_type[2];
      break;
   case DISPATCH_16 | DISPATCH_32:
      used[1] = pixel_type[2];
      used[2] = pixel_type[1];
      break;
   case DISPATCH_8 | DISPATCH_16 | DISPATCH_32:
      used[0] = pixel_type[0];
      used[1] = pixel_type[2];
      used[2] = pixel_type[1];
      break;
   default:
      break;
   }

   for (uint32_t i = 0; i < ARRAY_SIZE(k_offsets); i++) {
      if (!used[i]) {
         ImGui::Text("Kernel[%i] unused\n", i);
         continue;
      }

      char button_name[20];
      snprintf(button_name, sizeof(button_name), "Kernel[%i] %s", i, used[i]);
      if (ImGui::Button(button_name)) {
         new_shader_window(state,
                           gen_read_dword(reader.gen(), k_offsets[i]) & mask, "PS");
      }
   }
}

static void
handle_3dstate_binding_table_pointers(struct gpu_state *state,
                                      const struct aubinator_reader& reader,
                                      const struct decode_options *options)
{
   dump_binding_table(state, gen_read_dword(reader.gen(), 1), options);
}

static void
handle_3dstate_sampler_state_pointers(struct gpu_state *state,
                                      const struct aubinator_reader& reader,
                                      const struct decode_options *options)
{
   dump_samplers(state, gen_read_dword(reader.gen(), 1), options);
}

static void
handle_3dstate_sampler_state_pointers_gen6(struct gpu_state *state,
                                           const struct aubinator_reader& reader,
                                           const struct decode_options *options)
{
   dump_samplers(state, gen_read_dword(reader.gen(), 1), options);
   dump_samplers(state, gen_read_dword(reader.gen(), 2), options);
   dump_samplers(state, gen_read_dword(reader.gen(), 3), options);
}

static void
handle_3dstate_viewport_state_pointers_cc(struct gpu_state *state,
                                          const struct aubinator_reader& reader,
                                          const struct decode_options *options)
{
   struct gen_group *cc_viewport = gen_spec_find_struct(context.spec, "CC_VIEWPORT");
   uint64_t start = state->dynamic_state_base + (gen_read_dword(reader.gen(), 1) & ~0x1fu);

   for (uint32_t i = 0; i < 4; i++) {
      uint64_t viewport_offset = start + i * 8;
      if (ImGui::TreeNodeEx((void *) viewport_offset,
                            ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen,
                            "viewport %d", i)) {
         struct aubinator_reader child_reader(state->gtt, viewport_offset);
         decode_group(cc_viewport, child_reader.gen(), options);
         ImGui::TreePop();
      }
   }
}

static void
handle_3dstate_viewport_state_pointers_sf_clip(struct gpu_state *state,
                                               const struct aubinator_reader& reader,
                                               const struct decode_options *options)
{
   struct gen_group *sf_clip_viewport =
      gen_spec_find_struct(context.spec, "SF_CLIP_VIEWPORT");
   uint64_t start =
      state->dynamic_state_base + (gen_read_dword(reader.gen(), 1) & ~0x3fu);
   for (uint32_t i = 0; i < 4; i++) {
      uint64_t sf_clip_offset = start + i * 64;
      if (ImGui::TreeNodeEx((void *) sf_clip_offset,
                            ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen,
                            "viewport %d", i)) {
         struct aubinator_reader child_reader(state->gtt, sf_clip_offset);
         decode_group(sf_clip_viewport, child_reader.gen(), options);
         ImGui::TreePop();
      }
   }
}

static void
handle_3dstate_blend_state_pointers(struct gpu_state *state,
                                    const struct aubinator_reader& reader,
                                    const struct decode_options *options)
{
   struct gen_group *blend_state = gen_spec_find_struct(context.spec, "BLEND_STATE");
   uint64_t start = state->dynamic_state_base + (gen_read_dword(reader.gen(), 1) & ~0x3fu);

   struct aubinator_reader child_reader(state->gtt, start);
   decode_group(blend_state, child_reader.gen(), options);
}

static void
handle_3dstate_cc_state_pointers(struct gpu_state *state,
                                 const struct aubinator_reader& reader,
                                 const struct decode_options *options)
{
   struct gen_group *cc_state = gen_spec_find_struct(context.spec, "COLOR_CALC_STATE");
   uint64_t start = state->dynamic_state_base + (gen_read_dword(reader.gen(), 1) & ~0x3fu);

   struct aubinator_reader child_reader(state->gtt, start);
   decode_group(cc_state, child_reader.gen(), options);
}

static void
handle_3dstate_scissor_state_pointers(struct gpu_state *state,
                                      const struct aubinator_reader& reader,
                                      const struct decode_options *options)
{
   struct gen_group *scissor_rect = gen_spec_find_struct(context.spec, "SCISSOR_RECT");
   uint64_t start = state->dynamic_state_base + (gen_read_dword(reader.gen(), 1) & ~0x1fu);

   struct aubinator_reader child_reader(state->gtt, start);
   decode_group(scissor_rect, child_reader.gen(), options);
}

// static void
// handle_load_register_imm(struct gen_spec *spec, uint32_t *p)
// {
//    struct gen_group *reg = gen_spec_find_register(spec, p[1]);

//    if (reg != NULL) {
//       fprintf(stdout, "register %s (0x%x): 0x%x\n",
//               reg->name, reg->register_offset, p[2]);
//       decode_group(reg, &p[2]);
//    }
// }

static void
handle_3dstate_sample_pattern(struct gpu_state *state,
                              const struct aubinator_reader& reader,
                              const struct decode_options *options)
{
   if (ImGui::Button("Show pattern"))
      new_pattern_window(reader.gen());
}

static void
handle_snapshot(struct gpu_state *state,
                const struct aubinator_reader& reader,
                const struct decode_options *options)
{
   char title[20];
   snprintf(title, sizeof(title), "Show GPU state");
   if (ImGui::Button(title))
      new_state_window(gpu_state_at_offset(reader.get_offset()));
}

typedef void (*instruction_handler)(struct gpu_state *state,
                                    const struct aubinator_reader& reader,
                                    const struct decode_options *options);
#define CB(arg) ((void*)arg)

static struct hash_table *
init_accumulation_handlers(void)
{
   struct hash_table *handlers =
      _mesa_hash_table_create(NULL, _mesa_hash_string, _mesa_key_string_equal);

   _mesa_hash_table_insert(handlers, "STATE_BASE_ADDRESS", CB(handle_state_base_address));
   _mesa_hash_table_insert(handlers, "3DPRIMITIVE", CB(snapshot_gpu_state));

   return handlers;
}

static struct hash_table *
init_display_handlers(void)
{
   struct hash_table *handlers =
      _mesa_hash_table_create(NULL, _mesa_hash_string, _mesa_key_string_equal);

   // _mesa_hash_table_insert(handlers, "3DSTATE_VERTEX_BUFFERS", handle_3dstate_vertex_buffers);
   // _mesa_hash_table_insert(handlers, "3DSTATE_INDEX_BUFFERS", handle_3dstate_index_buffer);
   _mesa_hash_table_insert(handlers, "3DSTATE_VS", CB(handle_3dstate_vs));
   _mesa_hash_table_insert(handlers, "3DSTATE_GS", CB(handle_3dstate_vs));
   _mesa_hash_table_insert(handlers, "3DSTATE_DS", CB(handle_3dstate_vs));
   _mesa_hash_table_insert(handlers, "3DSTATE_HS", CB(handle_3dstate_hs));
   _mesa_hash_table_insert(handlers, "3DSTATE_PS", CB(handle_3dstate_ps));

   static const char *stages[] = { "VS", "GS", "DS", "HS", "PS" };
   for (uint32_t i = 0; i < ARRAY_SIZE(stages); i++) {
      // _mesa_hash_table_insert(handlers,
      //                         ralloc_asprintf(handlers,
      //                                         "3DSTATE_CONSTANT_%s", stages[i]),
      //                         handle_3dstate_constant);
      _mesa_hash_table_insert(handlers,
                              ralloc_asprintf(handlers,
                                              "3DSTATE_BINDING_TABLE_POINTERS_%s", stages[i]),
                              CB(handle_3dstate_binding_table_pointers));
      _mesa_hash_table_insert(handlers,
                              ralloc_asprintf(handlers,
                                              "3DSTATE_SAMPLER_STATE_POINTERS_%s", stages[i]),
                              CB(handle_3dstate_sampler_state_pointers));
   }
   _mesa_hash_table_insert(handlers,
                           ralloc_strdup(handlers, "3DSTATE_SAMPLER_STATE_POINTERS"),
                           CB(handle_3dstate_sampler_state_pointers_gen6));

   _mesa_hash_table_insert(handlers,
                           "3DSTATE_VIEWPORT_STATE_POINTERS_CC",
                           CB(handle_3dstate_viewport_state_pointers_cc));
   _mesa_hash_table_insert(handlers,
                           "3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP",
                           CB(handle_3dstate_viewport_state_pointers_sf_clip));
   _mesa_hash_table_insert(handlers,
                           "3DSTATE_CC_STATE_POINTERS",
                           CB(handle_3dstate_cc_state_pointers));
   _mesa_hash_table_insert(handlers,
                           "3DSTATE_SCISSOR_STATE_POINTERS",
                           CB(handle_3dstate_scissor_state_pointers));
   _mesa_hash_table_insert(handlers,
                           "3DSTATE_BLEND_STATE_POINTERS",
                           CB(handle_3dstate_blend_state_pointers));
   // _mesa_hash_table_insert(handlers,
   //                         "MI_LOAD_REGISTER_IMM",
   //                         handle_load_register_imm);
   _mesa_hash_table_insert(handlers,
                           "3DPRIMITIVE",
                           CB(handle_snapshot));

   _mesa_hash_table_insert(handlers,
                           "3DSTATE_SAMPLE_PATTERN",
                           CB(handle_3dstate_sample_pattern));
   _mesa_hash_table_insert(handlers,
                           "MEDIA_INTERFACE_DESCRIPTOR_LOAD",
                           CB(handle_media_interface_descriptor_load));

   return handlers;
}

#undef CB

static void
parse_commands(struct batch_window *window,
               const struct gen_dword_reader *reader,
               uint32_t n_dwords, bool from_ring)
{
   uint32_t dw_length;

   for (uint32_t dw_offset = 0; dw_offset < n_dwords; dw_offset += dw_length) {
      uint32_t dw0 = gen_read_dword(reader, dw_offset);
      struct gen_group *inst = gen_spec_find_instruction(context.spec, dw0);
      dw_length = gen_group_get_length(inst, dw0);
      assert(inst == NULL || dw_length > 0);
      dw_length = MAX(1, dw_length);
      if (inst == NULL) {
         ImGui::TextColored(context.error_color,
                            "unknown instruction %08x", dw0);
         continue;
      }

      struct gpu_state *state = context.last_gpu_state;
      uint64_t offset = reader_offset(reader) + 4 * dw_offset;
      struct aubinator_reader child_reader(reader, dw_offset);

      if (window) {
         if (window->cmd_filter.PassFilter(inst->name) &&
             ImGui::TreeNodeEx((void *) offset,
                               ((window->collapsed ? 0 : ImGuiTreeNodeFlags_DefaultOpen) |
                                ImGuiTreeNodeFlags_Framed),
                               "0x%08" PRIx64 ":  %s",
                               offset, inst->name)) {

            decode_group(inst, child_reader.gen(), &window->options);

            struct hash_entry *entry =
               _mesa_hash_table_search(context.display_handlers, inst->name);
            if (entry) {
               instruction_handler ui_handler = (instruction_handler) entry->data;
               ui_handler(state, child_reader, &window->options);
            }

            ImGui::TreePop();
         }
      } else {
         /* Only go through accumulation handlers when not rendering
          * (!window).
          */
         if ((dw0 & 0xffff0000) != AUB_MI_BATCH_BUFFER_START)
            gpu_state_add(state, state->gtt, offset, inst);

         struct hash_entry *entry =
            _mesa_hash_table_search(context.accumulation_handlers, inst->name);
         if (entry) {
            instruction_handler state_handler = (instruction_handler) entry->data;
            state_handler(state, child_reader, NULL);
         }
      }

      if ((dw0 & 0xffff0000) == AUB_MI_BATCH_BUFFER_START) {
         uint64_t start = read_address(reader, 1);

         if (dw0 & (1 << 22)) {
            /* MI_BATCH_BUFFER_START with "2nd Level Batch Buffer" set acts
             * like a subroutine call.  Commands that come afterwards get
             * processed once the 2nd level batch buffer returns with
             * MI_BATCH_BUFFER_END.
             */
            struct aubinator_reader child_reader(state->gtt, start);
            parse_commands(window, child_reader.gen(),
                           (memory_end(state->gtt) - start) / 4, false);
         } else {
            /* MI_BATCH_BUFFER_START with "2nd Level Batch Buffer" unset acts
             * like a goto. Nothing after it will ever get processed. Just
             * break out of this loop.
             */
            struct aubinator_reader child_reader(state->gtt, start);
            parse_commands(window, child_reader.gen(),
                           (memory_end(state->gtt) - start) / 4, false);
            break;
         }
      } else if ((dw0 & 0xffff0000) == AUB_MI_BATCH_BUFFER_END) {
         break;
      }
   }
}

#define GEN_ENGINE_RENDER 1
#define GEN_ENGINE_BLITTER 2

static uint32_t
frame_read_block_type(uint32_t *p)
{
   return p[1] & AUB_TRACE_OPERATION_MASK;
}

static void
handle_trace_block_data_write(uint32_t *p)
{
   UNUSED int operation = frame_read_block_type(p);
   UNUSED int type = p[1] & AUB_TRACE_TYPE_MASK;
   int address_space = p[1] & AUB_TRACE_ADDRESS_SPACE_MASK;
   uint64_t offset = p[3];
   uint32_t size = p[4];
   int header_length = p[0] & 0xffff;
   uint32_t *data = p + header_length + 2;
   UNUSED int engine = GEN_ENGINE_RENDER;

   if (gen_spec_get_gen(context.spec) >= gen_make_gen(8,0))
      offset += (uint64_t) p[5] << 32;

   assert(operation == AUB_TRACE_OP_DATA_WRITE);

   if (address_space != AUB_TRACE_MEMTYPE_GTT)
      return;

   struct gpu_state *state = context.last_gpu_state;
   state->gtt = memory_write_unref(state->gtt, offset, data, size);
}

static void
handle_trace_block_command_write(uint32_t *p, struct batch_window *window)
{
   UNUSED int operation = frame_read_block_type(p);
   int type = p[1] & AUB_TRACE_TYPE_MASK;
   UNUSED int address_space = p[1] & AUB_TRACE_ADDRESS_SPACE_MASK;
   uint64_t offset = p[3];
   uint32_t size = p[4];
   int header_length = p[0] & 0xffff;
   uint32_t *data = p + header_length + 2;

   if (gen_spec_get_gen(context.spec) >= gen_make_gen(8,0))
      offset += (uint64_t) p[5] << 32;

   assert(operation == AUB_TRACE_OP_COMMAND_WRITE);

   if (type != AUB_TRACE_TYPE_RING_PRB0 &&
       type != AUB_TRACE_TYPE_RING_PRB2) {
      if (window) {
         ImGui::TextColored(context.error_color,
                            "command write to unknown ring %d\n", type);
      }
   }

   /* When no window, record the states */
   if (!window) {
      struct hash_entry *entry;
      hash_table_foreach(context.gpu_states->table, entry) {
         struct gpu_state *state = (struct gpu_state *) entry->data;
         if (state != context.last_gpu_state)
            gpu_state_free(state);
      }
      _mesa_hash_table_clear(context.gpu_states->table, NULL);
      _mesa_hash_table_u64_insert(context.gpu_states, 1, context.last_gpu_state);
   }

   struct aubinator_reader reader(data);
   parse_commands(window, reader.gen(), size, true);
   //context.gtt_end = 0;
}

static void
handle_trace_header(uint32_t *p)
{
   /* The intel_aubdump tool from IGT is kind enough to put a PCI-ID= tag in
    * the AUB header comment.  If the user hasn't specified a hardware
    * generation, try to use the one from the AUB file.
    */
   uint32_t *end = p + (p[0] & 0xffff) + 2;
   int aub_pci_id = 0;

   if (context.pci_id == 0) {
      if (end > &p[12] && p[12] > 0)
         sscanf((char *)&p[13], "PCI-ID=%i", &aub_pci_id);
      context.pci_id = aub_pci_id;
   }
   context.read_pci_id = aub_pci_id != 0;

   if (!gen_get_device_info(context.pci_id, &context.devinfo)) {
      fprintf(stderr, "can't find device information: pci_id=0x%x\n", context.pci_id);
      exit(EXIT_FAILURE);
   }

   if (context.xml_path == NULL)
      context.spec = gen_spec_load(&context.devinfo);
   else
      context.spec = gen_spec_load_from_path(&context.devinfo, context.xml_path);
   context.disasm = gen_disasm_create(context.pci_id);

   if (context.spec == NULL || context.disasm == NULL)
      exit(EXIT_FAILURE);

   strncpy(context.app_name, (char *)&p[2], 32);
   context.app_name[32] = 0;
}

struct aub_file {
   FILE *stream;

   uint32_t *map, *end, *cursor;
   uint32_t *mem_end;
};

static struct aub_file *
aub_file_open(const char *filename)
{
   struct aub_file *file;
   struct stat sb;
   int fd;

   file = xtzalloc(struct aub_file);
   fd = open(filename, O_RDONLY);
   if (fd == -1) {
      fprintf(stderr, "open %s failed: %s\n", filename, strerror(errno));
      exit(EXIT_FAILURE);
   }

   if (fstat(fd, &sb) == -1) {
      fprintf(stderr, "stat failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
   }

   file->map = (uint32_t *) mmap(NULL, sb.st_size,
                                 PROT_READ, MAP_SHARED, fd, 0);
   if (file->map == MAP_FAILED) {
      fprintf(stderr, "mmap failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
   }

   close(fd);

   file->cursor = file->map;
   file->end = file->map + sb.st_size / 4;

   return file;
}

static struct aub_file *
aub_file_stdin(void)
{
   struct aub_file *file;

   file = xtzalloc(struct aub_file);
   file->stream = stdin;

   return file;
}

#define TYPE(dw)       (((dw) >> 29) & 7)
#define OPCODE(dw)     (((dw) >> 23) & 0x3f)
#define SUBOPCODE(dw)  (((dw) >> 16) & 0x7f)

#define MAKE_HEADER(type, opcode, subopcode) \
                   (((type) << 29) | ((opcode) << 23) | ((subopcode) << 16))

#define TYPE_AUB            0x7U

/* Classic AUB opcodes */
#define OPCODE_AUB          0x01U
#define SUBOPCODE_HEADER    0x05U
#define SUBOPCODE_BLOCK     0x41U
#define SUBOPCODE_BMP       0x1eU

/* Newer version AUB opcode */
#define OPCODE_NEW_AUB      0x2eU
#define SUBOPCODE_VERSION   0x00U
#define SUBOPCODE_REG_WRITE 0x03U
#define SUBOPCODE_MEM_POLL  0x05U
#define SUBOPCODE_MEM_WRITE 0x06U

#define MAKE_GEN(major, minor) ( ((major) << 8) | (minor) )

enum {
   AUB_ITEM_DECODE_OK,
   AUB_ITEM_DECODE_FAILED,
   AUB_ITEM_DECODE_NEED_MORE_DATA,
};

static int
aub_file_next_frame(struct aub_file *file,
                    uint32_t *frame_type,
                    uint32_t **output_cursor)
{
   uint32_t h, device, data_type, *new_cursor;
   int header_length, bias;

   if (file->end - file->cursor < 1)
      return AUB_ITEM_DECODE_NEED_MORE_DATA;

   h = *file->cursor;
   header_length = h & 0xffff;

   switch (OPCODE(h)) {
   case OPCODE_AUB:
      bias = 2;
      break;
   case OPCODE_NEW_AUB:
      bias = 1;
      break;
   default:
      ImGui::Text("unknown opcode %d at %td/%td\n",
           OPCODE(h), file->cursor - file->map,
           file->end - file->map);
      return AUB_ITEM_DECODE_FAILED;
   }

   new_cursor = file->cursor + header_length + bias;
   if ((h & 0xffff0000) == MAKE_HEADER(TYPE_AUB, OPCODE_AUB, SUBOPCODE_BLOCK)) {
      if (file->end - file->cursor < 4)
         return AUB_ITEM_DECODE_NEED_MORE_DATA;
      new_cursor += file->cursor[4] / 4;
   }

   if (new_cursor > file->end)
      return AUB_ITEM_DECODE_NEED_MORE_DATA;

   if (frame_type)
      *frame_type = h & 0xffff0000;
   if (output_cursor)
      *output_cursor = new_cursor;

   switch (h & 0xffff0000) {
   case MAKE_HEADER(TYPE_AUB, OPCODE_AUB, SUBOPCODE_HEADER):
      //handle_trace_header(file->cursor);
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_AUB, SUBOPCODE_BLOCK):
      //handle_trace_block(file->cursor);
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_AUB, SUBOPCODE_BMP):
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_NEW_AUB, SUBOPCODE_VERSION): {
      fprintf(stdout, "version block: dw1 %08x\n", file->cursor[1]);
      device = (file->cursor[1] >> 8) & 0xff;
      const char *device_map[] = {
         "bwr",
         "cln",
         "blc",
         "ctg",
         "el",
         "il",
         "sbr",
         "ivb",
         "lrb2",
         "hsw",
         "vlv",
         "bdw",
         "skl",
         "chv",
         "bxt",
         "cnl",
      };
      fprintf(stdout, "  device %s\n", device_map[device]);
      break;
   }
   case MAKE_HEADER(TYPE_AUB, OPCODE_NEW_AUB, SUBOPCODE_REG_WRITE):
      fprintf(stdout, "register write block: (dwords %d)\n", h & 0xffff);
      fprintf(stdout, "  reg 0x%x, data 0x%x\n",
              file->cursor[1], file->cursor[5]);
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_NEW_AUB, SUBOPCODE_MEM_WRITE):
      fprintf(stdout, "memory write block (dwords %d):\n", h & 0xffff);
      fprintf(stdout, "  address 0x%" PRIx64 "\n", *(uint64_t *) &file->cursor[1]);
      data_type = (file->cursor[3] >> 20) & 0xff;
      if (data_type != 0)
         fprintf(stdout, "  data type 0x%x\n", data_type);
      fprintf(stdout, "  address space 0x%x\n", (file->cursor[3] >> 28) & 0xf);
      break;
   case MAKE_HEADER(TYPE_AUB, OPCODE_NEW_AUB, SUBOPCODE_MEM_POLL):
      fprintf(stdout, "memory poll block (dwords %d):\n", h & 0xffff);
      break;
   default:
      fprintf(stdout, "unknown block type=0x%x, opcode=0x%x, "
             "subopcode=0x%x (%08x)\n", TYPE(h), OPCODE(h), SUBOPCODE(h), h);
      break;
   }

   return AUB_ITEM_DECODE_OK;
}

static int
aub_file_more_stuff(struct aub_file *file)
{
   return file->cursor < file->end || (file->stream && !feof(file->stream));
}

#define AUB_READ_BUFFER_SIZE (4096)

static void
aub_file_data_grow(struct aub_file *file)
{
   size_t old_size = (file->mem_end - file->map) * 4;
   size_t new_size = MAX(old_size * 2, AUB_READ_BUFFER_SIZE);
   uint32_t *new_start = (uint32_t *) realloc(file->map, new_size);

   file->cursor = new_start + (file->cursor - file->map);
   file->end = new_start + (file->end - file->map);
   file->map = new_start;
   file->mem_end = file->map + (new_size / 4);
}

static bool
aub_file_data_load(struct aub_file *file)
{
   size_t r;

   if (file->stream == NULL)
      return false;

   /* First remove any consumed data */
   if (file->cursor > file->map) {
      memmove(file->map, file->cursor,
              (file->end - file->cursor) * 4);
      file->end -= file->cursor - file->map;
      file->cursor = file->map;
   }

   /* Then load some new data in */
   if ((file->mem_end - file->end) < (AUB_READ_BUFFER_SIZE / 4))
      aub_file_data_grow(file);

   r = fread(file->end, 1, (file->mem_end - file->end) * 4, file->stream);
   file->end += r / 4;

   return r != 0;
}

static void
aub_file_reset(struct aub_file *file)
{
   file->cursor = file->map;
}

static void
aub_file_read_headers(void)
{
   bool found = false;

   list_for_each_entry(struct window, window, &context.windows, windows) {
      if (window->reset) window->reset(window);
   }

   if (context.disasm) {
      gen_disasm_destroy(context.disasm);
      context.disasm = NULL;
   }
   if (context.spec) {
      gen_spec_destroy(context.spec);
      context.spec = NULL;
   }

   aub_file_reset(context.file);
   while (aub_file_more_stuff(context.file) && !found) {
      uint32_t *new_cursor;
      uint32_t frame_type;

      switch (aub_file_next_frame(context.file, &frame_type, &new_cursor)) {
      case AUB_ITEM_DECODE_OK:
         found = MAKE_HEADER(TYPE_AUB, OPCODE_AUB, SUBOPCODE_HEADER) == frame_type;
         if (found)
            handle_trace_header(context.file->cursor);
         context.file->cursor = new_cursor;
         break;
      case AUB_ITEM_DECODE_NEED_MORE_DATA:
         if (!context.file->stream) {
            context.file->cursor = context.file->end;
            break;
         }
         if (aub_file_more_stuff(context.file) && !aub_file_data_load(context.file)) {
            fprintf(stderr, "failed to load data from stdin\n");
            exit(EXIT_FAILURE);
         }
         break;
      default:
         fprintf(stderr, "failed to parse aubdump data\n");
         exit(EXIT_FAILURE);
      }
   }
}

static void
aub_file_read_block(uint32_t new_index)
{
   int32_t index = -1;

   context.command_index = new_index;

   struct hash_entry *entry;
   hash_table_foreach(context.gpu_states->table, entry) {
      struct gpu_state *state = (struct gpu_state *) entry->data;
      gpu_state_free(state);
   }
   _mesa_hash_table_clear(context.gpu_states->table, NULL);
   context.last_gpu_state = gpu_state_new();
   _mesa_hash_table_u64_insert(context.gpu_states, 1, context.last_gpu_state);

   aub_file_reset(context.file);
   while (aub_file_more_stuff(context.file) && index < context.command_index) {
      uint32_t *new_cursor;
      uint32_t frame_type;

      switch (aub_file_next_frame(context.file, &frame_type, &new_cursor)) {
      case AUB_ITEM_DECODE_OK:
         if (frame_type == MAKE_HEADER(TYPE_AUB, OPCODE_AUB, SUBOPCODE_BLOCK)) {
            switch (frame_read_block_type(context.file->cursor)) {
            case AUB_TRACE_OP_DATA_WRITE:
               handle_trace_block_data_write(context.file->cursor);
               break;
            case AUB_TRACE_OP_COMMAND_WRITE:
               handle_trace_block_command_write(context.file->cursor, NULL);
               context.command_block = context.file->cursor;
               index++;
               break;
            }
         }
         context.file->cursor = new_cursor;
         break;
      case AUB_ITEM_DECODE_NEED_MORE_DATA:
         if (!context.file->stream) {
            context.file->cursor = context.file->end;
            break;
         }
         if (aub_file_more_stuff(context.file) && !aub_file_data_load(context.file)) {
            fprintf(stderr, "failed to load data from stdin\n");
            exit(EXIT_FAILURE);
         }
         break;
      default:
         fprintf(stderr, "failed to parse aubdump data\n");
         exit(EXIT_FAILURE);
      }
   }
}

/**/

static bool
select_offset_base(struct offset_at_base *offset)
{
   static const uint64_t zero = 0;
   struct gpu_state *state = context.last_gpu_state;
   const struct {
      const char *name;
      const uint64_t *base;
   } items[] = {
      { "zero",        &zero },
      { "general",     &state->general_state_base },
      { "surface",     &state->surface_state_base },
      { "dynamic",     &state->dynamic_state_base },
      { "instruction", &state->instruction_base },
   };
   bool selected = false;

   for (uint32_t i = 0; i < ARRAY_SIZE(items); i++) {
      if (ImGui::Selectable(items[i].name)) {
         offset->base = items[i].base;
         offset->base_name = items[i].name;
         selected = true;
      }
   }
   return selected;
}

static bool
offset_is_valid(struct offset_at_base *offset, memory_t *mem)
{
   return gtt_valid_offset(mem, *offset->base + offset->offset);
}

// static bool
// offset_is_valid_range(struct offset_at_base *offset, uint32_t size,
//                       memory_t *mem)
// {
//    return gtt_valid_range(mem, *offset->base + offset->offset, size);
// }

static uint64_t
offset_get_absolute(struct offset_at_base *offset)
{
   return *offset->base + offset->offset;
}

static uint32_t
offset_read_dword(struct offset_at_base *offset,
                  uint32_t dword_offset,
                  memory_t *mem)
{
   return memory_read_dword(mem, offset_get_absolute(offset) + 4 * dword_offset);
}

static bool
display_offset(struct offset_at_base *offset)
{
   bool changed = false;

   ImGui::Text("From:"); ImGui::SameLine();
   bool open_popup = ImGui::Button(offset->base_name); ImGui::SameLine();
   ImGui::Text("0x%08" PRIx64, *offset->base); ImGui::SameLine();
   if (open_popup)
      ImGui::OpenPopup("base picker");
   if (ImGui::BeginPopup("base picker")) {
      changed = select_offset_base(offset);
      ImGui::EndPopup();
   }

   ImGui::Text(" @ "); ImGui::SameLine();
   ImGui::PushItemWidth(100);
   snprintf(offset->address, sizeof(offset->address),
            "%" PRIx64, offset->offset);
   if (ImGui::InputText("", offset->address, sizeof(offset->address),
                        ImGuiInputTextFlags_CharsHexadecimal)) {
      sscanf(offset->address, "%" PRIx64, &offset->offset);
      changed = true;
   }
   ImGui::PopItemWidth();

   return changed;
}

/**/

static void
display_file_props(void)
{
   if (context.input_file)
      ImGui::Text("File name:        %s\n", context.input_file);
   ImGui::Text("PCI ID:           0x%x\n", context.pci_id);
   ImGui::Text("Application name: %s\n", context.app_name);

   ImGui::Text("Decoding as:"); ImGui::SameLine();
   if (ImGui::Button("Select..")) { ImGui::OpenPopup("select"); } ImGui::SameLine();
   if (ImGui::BeginPopup("select")) {
      for (uint32_t i = 0; i < ARRAY_SIZE(supported_gens); i++) {
         if (ImGui::Selectable(supported_gens[i].name)) {
            context.pci_id = supported_gens[i].pci_id;
            aub_file_read_headers();
         }
      }
      if (ImGui::Selectable("Reset")) {
         context.pci_id = 0;
         aub_file_read_headers();
      }
      ImGui::EndPopup();
   }
   ImGui::SameLine();
   ImGui::Text(gen_get_device_name(context.pci_id));
}

/**/

static void display_decode_options(struct decode_options *options)
{
   char name[40];
   snprintf(name, sizeof(name), "field filter##%p", options);
   options->field_filter.Draw(name);
   ImGui::Checkbox("Dwords", &options->show_dwords);
}

/* Batch decoding windows */

static void
display_batch_window(struct window *win)
{
   struct batch_window *window = (struct batch_window *) win;

   ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / (2 * 2));
   if (window_has_ctrl_key('f')) ImGui::SetKeyboardFocusHere();
   char name[40];
   snprintf(name, sizeof(name), "command filter##%p", window);
   window->cmd_filter.Draw(name); ImGui::SameLine();
   display_decode_options(&window->options); ImGui::SameLine();
   ImGui::PopItemWidth();

   ImGui::Checkbox("Collapse commands", &window->collapsed); ImGui::SameLine();
   if (window->ring_buffer == NULL) ImGui::Text("Batch buffer %i", context.command_index);
   ImGui::BeginChild(ImGui::GetID("##block"));
   uint32_t *commands =
      window->ring_buffer ? window->ring_buffer : context.command_block;
   handle_trace_block_command_write(commands, window);
   ImGui::EndChild();
}

static void
destroy_batch_window(struct window *win)
{
   struct batch_window *window = (struct batch_window *) win;

   free(window);
}

static void
new_batch_window(bool snapshot)
{
   struct batch_window *window = xtzalloc(struct batch_window);

   if (snapshot) {
      window->ring_buffer = context.command_block;
      /* TODO copy gpu state */
      snprintf(window->base.name, sizeof(window->base.name),
               "Batch view (batch %u)##%p", context.command_index, window);
   } else {
      snprintf(window->base.name, sizeof(window->base.name),
               "Batch view (live)##%p", window);
   }
   window->base.position = ImVec2(-1, -1);
   window->base.size = ImVec2(500, 600);
   window->base.opened = true;
   window->base.display = display_batch_window;
   window->base.destroy = destroy_batch_window;

   window->collapsed = true;
   window->options.show_dwords = true;

   list_add(&window->base.windows, &context.windows);
}

/* Decode as windows */

static void
select_structure(gen_group **group)
{
   static ImGuiTextFilter filter;
   filter.Draw();

   struct hash_entry *entry;
   hash_table_foreach(context.spec->commands, entry) {
      struct gen_group *cmd = (struct gen_group *) entry->data;
      if (filter.PassFilter(cmd->name) &&
          ImGui::Selectable(cmd->name)) {
         *group = cmd;
      }
   }
   hash_table_foreach(context.spec->registers_by_name, entry) {
      struct gen_group *reg = (struct gen_group *) entry->data;
      if (filter.PassFilter(reg->name) &&
          ImGui::Selectable(reg->name)) {
         *group = reg;
      }
   }
   hash_table_foreach(context.spec->structs, entry) {
      struct gen_group *strct = (struct gen_group *) entry->data;
      if (filter.PassFilter(strct->name) &&
          ImGui::Selectable(strct->name)) {
         *group = strct;
      }
   }
}

static void
display_as_window(struct window *win)
{
   struct as_window *window = (struct as_window *) win;
   struct gpu_state *state = window->state ? window->state : context.last_gpu_state;

   display_offset(&window->offset);

   bool open_popup = ImGui::Button(window->as ? window->as->name : "None");
   if (open_popup)
      ImGui::OpenPopup("struct picker");
   if (ImGui::BeginPopup("struct picker")) {
      select_structure(&window->as);
      ImGui::EndPopup();
   }
   ImGui::SameLine();

   if (window->as) {
      if (ImGui::Button("Previous"))
         window->offset.offset -= window->as->dw_length * 4;
      ImGui::SameLine();
      if (ImGui::Button("Next"))
         window->offset.offset += window->as->dw_length * 4;

      display_decode_options(&window->options);
   }

   ImGui::BeginChild(ImGui::GetID("##block"));
   if (!offset_is_valid(&window->offset, state->gtt))
       ImGui::TextColored(context.error_color, "outside of GTT");
   else if (window->as) {
      bool all_zeros = true;
      for (uint32_t dw = 0; dw < window->as->dw_length; dw++) {
         all_zeros = all_zeros &&
            offset_read_dword(&window->offset, dw, state->gtt) == 0;
      }
      if (all_zeros) ImGui::TextColored(context.error_color, "All dwords at 0");

      struct aubinator_reader reader(state->gtt, offset_get_absolute(&window->offset));
      decode_group(window->as, reader.gen(), &window->options);
   }
   ImGui::EndChild();
}

static void
destroy_as_window(struct window *win)
{
   struct as_window *window = (struct as_window *) win;

   gpu_state_free(window->state);
   free(window);
}

static void
reset_as_window(struct window *win)
{
   struct as_window *window = (struct as_window *) win;

   window->as = NULL;
}

static void
new_as_window(struct gpu_state *state, uint64_t offset)
{
   struct as_window *window = xtzalloc(struct as_window);

   build_window_title(&window->base, "Decode view", state);
   window->base.position = ImVec2(-1, -1);
   window->base.size = ImVec2(400, 400);
   window->base.opened = true;
   window->base.display = display_as_window;
   window->base.destroy = destroy_as_window;
   window->base.reset = reset_as_window;

   window->state = gpu_state_copy(state);
   window->offset.base = &state->general_state_base;
   window->offset.base_name = "general";
   window->offset.offset = offset;
   window->options.field_filter = ImGuiTextFilter();

   list_add(&window->base.windows, &context.windows);
}

/* Image decode windows */

static void
update_image_window(struct image_window *window)
{
   if (!gtt_valid_range(window->state->gtt,
                        window->base_address,
                        window->plane_stride) ||
       !window->width || !window->height)
      return;

   static const struct {
      CoglPixelFormat format;
      const char *shader;
   } formats[] = {
      { COGL_PIXEL_FORMAT_A_8,
        "cogl_layer = vec4(cogl_texel0.a, 0.0, 0.0, 1.0);\n", },
      { COGL_PIXEL_FORMAT_RG_88,
        "cogl_layer = vec4(cogl_texel0.r,\n"
        "                  cogl_texel0.g,\n"
        "                  0.0, 1.0);\n", },
      { COGL_PIXEL_FORMAT_RGB_888,
        "cogl_layer = vec4(cogl_texel0.r,\n"
        "                  cogl_texel0.g,\n"
        "                  cogl_texel0.b,\n"
        "                  1.0);\n", },
      { COGL_PIXEL_FORMAT_RGBA_8888,
        "cogl_layer = vec4(cogl_texel0.r,\n"
        "                  cogl_texel0.g,\n"
        "                  cogl_texel0.b,\n"
        "                  1.0);\n", },
   };
   uint32_t channels =
      isl_format_get_num_channels((enum isl_format) window->format);
   if (channels < 1 || channels > 4) return;

   uint8_t *data = (uint8_t *) malloc(window->plane_stride);
   memory_read(window->state->gtt, window->base_address, data, window->plane_stride);
   CoglTexture *texture =
      COGL_TEXTURE(cogl_texture_2d_new_from_data(ImGui_ImplGtk3Cogl_GetContext(),
                                                 window->width,
                                                 window->height,
                                                 formats[channels - 1].format,
                                                 window->stride,
                                                 (uint8_t *) data,
                                                 NULL));
   free(data);
   if (!texture) return;

   strncpy(window->shader, formats[channels - 1].shader, sizeof(window->shader));

   CoglPipeline *pipeline = cogl_pipeline_new(ImGui_ImplGtk3Cogl_GetContext());
   cogl_pipeline_set_layer_texture(pipeline, 0, texture);
   cogl_pipeline_set_layer_filters(pipeline, 0,
                                   COGL_PIPELINE_FILTER_NEAREST,
                                   COGL_PIPELINE_FILTER_NEAREST);
   cogl_object_unref(texture);

   // "cogl_position_in * vec2(%i, %i)\n"

   cogl_pipeline_add_layer_snippet(pipeline, 0,
                                   cogl_snippet_new(COGL_SNIPPET_HOOK_LAYER_FRAGMENT,
                                                    "",
                                                    window->shader));

   if (window->pipeline) cogl_object_unref(window->pipeline);
   window->pipeline = pipeline;
}

static bool
select_image_format(enum isl_format *format)
{
   bool selected = false;
   static ImGuiTextFilter filter;
   filter.Draw();

   for (uint32_t f = 0; f < ISL_FORMAT_RAW; f++) {
      const char *name = isl_format_get_name((enum isl_format) f);
      if (!name || !filter.PassFilter(name))
         continue;
      if (ImGui::Selectable(name)) {
         *format = (enum isl_format) f;
         selected = true;
      }
   }

   return selected;
}

static void
display_image_window(struct window *win)
{
   struct image_window *window = (struct image_window *) win;

   if (display_offset(&window->offset)) update_image_window(window);

   bool open_popup = ImGui::Button(isl_format_get_name(window->format));
   if (open_popup)
      ImGui::OpenPopup("format picker");
   if (ImGui::BeginPopup("format picker")) {
      if (select_image_format(&window->format))
         update_image_window(window);
      ImGui::EndPopup();
   }
   ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 2);
   if (ImGui::InputInt("Width", &window->width)) update_image_window(window);
   if (ImGui::InputInt("Height", &window->height)) update_image_window(window);
   if (ImGui::InputInt("Row stride", &window->stride)) update_image_window(window);

   snprintf(window->base_address_buf, sizeof(window->base_address_buf),
            "%" PRIx64, window->base_address);
   if (ImGui::InputText("Surface base address", window->base_address_buf,
                        sizeof(window->base_address_buf),
                        ImGuiInputTextFlags_CharsHexadecimal)) {
      sscanf(window->base_address_buf, "%" PRIx64, &window->base_address);
      update_image_window(window);
   }

   ImGui::PopItemWidth();

   ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
   if (ImGui::InputTextMultiline("##shader",
                                 window->shader, ARRAY_SIZE(window->shader),
                                 ImVec2(-1.0f, 0), 0)) {
      g_message("update shader");
      update_image_window(window);
   }

   if (window->pipeline) {
      ImGui::Image(window->pipeline,
                   ImVec2(ImGui::GetContentRegionAvailWidth(),
                          window->height * ImGui::GetContentRegionAvailWidth() / window->width));
      // if (ImGui::IsItemHovered()) {
      //    ImGui::BeginTooltip();
      //    ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
      //    CoglTexture *texture = cogl_pipeline_get_layer_texture(window->pipeline, 0);
      //    int tex_w = cogl_texture_get_width(texture), tex_h = cogl_texture_get_height(texture);
      //    float focus_sz = 32.0f;
      //    float focus_x = ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f; if (focus_x < 0.0f) focus_x = 0.0f; else if (focus_x > tex_w - focus_sz) focus_x = tex_w - focus_sz;
      //    float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y - focus_sz * 0.5f; if (focus_y < 0.0f) focus_y = 0.0f; else if (focus_y > tex_h - focus_sz) focus_y = tex_h - focus_sz;
      //    ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
      //    ImGui::Text("Max: (%.2f, %.2f)", focus_x + focus_sz, focus_y + focus_sz);
      //    ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
      //    ImVec2 uv1 = ImVec2((focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
      //    ImGui::Image(window->pipeline, ImVec2(128,128), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
      //    ImGui::EndTooltip();
      // }
   }

   if (!offset_is_valid(&window->offset, window->state->gtt))
      ImGui::TextColored(context.error_color, "Invalid offset in GTT");
   ImGui::PopItemWidth();
}

static void
destroy_image_window(struct window *win)
{
   struct image_window *window = (struct image_window *) win;

   if (window->pipeline) cogl_object_unref(window->pipeline);
   gpu_state_free(window->state);
   free(window);
}

static void
new_image_window(struct gpu_state *state, uint64_t offset)
{
   struct image_window *window = xtzalloc(struct image_window);

   snprintf(window->base.name, sizeof(window->base.name),
            "Image view##%p", window);
   window->base.opened = true;
   window->base.display = display_image_window;
   window->base.destroy = destroy_image_window;

   window->state = gpu_state_copy(state);
   window->offset.base = &state->surface_state_base;
   window->offset.base_name = "surface";
   window->offset.offset = offset;

   window->shader[0] = '\0';

   if (offset_is_valid(&window->offset, window->state->gtt)) {
      struct gen_group *render =
         gen_spec_find_struct(context.spec, "RENDER_SURFACE_STATE");
      struct aubinator_reader reader(window->state->gtt, offset);

      union gen_field_value value = { .u64 = 0 };
      gen_field_decode(gen_group_find_field(render, "Surface Base Address"),
                       reader.gen(), &value);
      window->base_address = value.u64;
      gen_field_decode(gen_group_find_field(render, "Width"),
                       reader.gen(), &value);
      window->width = value.u64 + 1;
      gen_field_decode(gen_group_find_field(render, "Height"),
                       reader.gen(), &value);
      window->height = value.u64 + 1;
      gen_field_decode(gen_group_find_field(render, "Surface Pitch"),
                       reader.gen(), &value);
      window->stride = value.u64 + 1;
      gen_field_decode(gen_group_find_field(render, "Surface QPitch"),
                       reader.gen(), &value);
      window->plane_stride = (value.u64 << 2) * window->stride;
      gen_field_decode(gen_group_find_field(render, "Surface Format"),
                       reader.gen(), &value);
      window->format = (enum isl_format) value.u64;
      gen_field_decode(gen_group_find_field(render, "Tiled Resource Mode"),
                       reader.gen(), &value);
      switch (value.u64) {
      default:
      case 0: window->tiling = ISL_TILING_LINEAR; break;
      case 1: window->tiling = ISL_TILING_Yf; break;
      case 2: window->tiling = ISL_TILING_Ys; break;
      }

      // const struct isl_format_layout *layout =
      //    isl_format_get_layout(value.u64);
      // uint32_t channels = isl_format_get_num_channels(value.u64);
      update_image_window(window);
   }

   list_add(&window->base.windows, &context.windows);
}

/* Pattern windows */

static void
display_pattern_window(struct window *win)
{
   struct pattern_window *window = (struct pattern_window *) win;
   static const struct {
      const char *name;
      uint8_t offset;
      uint8_t length;
   } ms[] = {
      { "16x", 0, 16 },
      { "8x", 16, 8 },
      { "4x", 24, 4 },
      { "2x", 28, 2 },
      { "1x", 30, 1 },
   };

   for (unsigned i = 0; i < ARRAY_SIZE(ms); i++) {
      ImGui::SameLine();
      ImGui::RadioButton(ms[i].name, &window->ms_offset, i);
   }

   ImGui::PushStyleColor(ImGuiCol_PlotLines, context.highlight_color);
   Aubinator::PlotDots("points", window->points,
                       ms[window->ms_offset].length, ms[window->ms_offset].offset,
                       "Sampling points", ImVec2(0.0, 0.0), ImVec2(1.0, 1.0),
                       ImVec2(ImGui::GetContentRegionAvailWidth(),
                              ImGui::GetContentRegionAvailWidth()), 5.0f);
   ImGui::PopStyleColor();
}

static void
destroy_pattern_window(struct window *win)
{
   free(win);
}

static void
new_pattern_window(const struct gen_dword_reader *reader)
{
   struct pattern_window *window = xtzalloc(struct pattern_window);

   snprintf(window->base.name, sizeof(window->base.name),
            "Sampling pattern##%p", window);
   window->base.position = ImVec2(-1, -1);
   window->base.size = ImVec2(300, 360);
   window->base.opened = true;
   window->base.display = display_pattern_window;
   window->base.destroy = destroy_pattern_window;

   for (unsigned p = 0, dw_idx = 1; p < ARRAY_SIZE(window->points); ) {
      union {
         uint32_t dw;
         uint8_t points[4];
      } data;
      data.dw = gen_read_dword(reader, dw_idx++);

      for (unsigned b = 0; b < 4 && p < ARRAY_SIZE(window->points); b++, p++) {
         uint8_t fixed_point = data.points[b];
         window->points[p] = ImVec2((fixed_point & 0xf) / 16.0f,
                                    ((fixed_point >> 4) & 0xf) / 16.0f);
      }
   }

   list_add(&window->base.windows, &context.windows);
}

static void
new_pattern_window(struct gpu_state *state)
{
   struct gpu_state_item *item = gpu_state_find(state, "3DSTATE_SAMPLE_PATTERN");
   if (!item)
      return;

   struct aubinator_reader reader(item->gtt, item->gtt_offset);
   new_pattern_window(reader.gen());
}

/* Shader windows */

static void
display_shader_window(struct window *win)
{
   struct shader_window *window = (struct shader_window *) win;

   display_offset(&window->offset);

   ImGui::BeginChild(ImGui::GetID("##block"));
   ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
   ImGui::InputTextMultiline("##shader",
                             window->shader, window->shader_len,
                             ImVec2(-1.0f, -1.0f), ImGuiInputTextFlags_ReadOnly);
   ImGui::PopItemWidth();
   ImGui::EndChild();
}

static void
destroy_shader_window(struct window *win)
{
   struct shader_window *window = (struct shader_window *) win;

   if (window->shader) free(window->shader);
   gpu_state_free(window->state);
   free(window);
}

static void
new_shader_window(struct gpu_state *state, uint64_t shader_offset, const char *type)
{
   struct shader_window *window = xtzalloc(struct shader_window);

   snprintf(window->base.name, sizeof(window->base.name),
            "%s shader view##%p", type, window);
   window->base.position = ImVec2(-1, -1);
   window->base.size = ImVec2(800, 300);
   window->base.opened = true;
   window->base.display = display_shader_window;
   window->base.destroy = destroy_shader_window;

   window->state = gpu_state_copy(state);
   window->offset.base = &state->instruction_base;
   window->offset.base_name = "instruction";
   window->offset.offset = shader_offset;

   if (offset_is_valid(&window->offset, window->state->gtt)) {
      uint64_t offset = offset_get_absolute(&window->offset);
      struct aubinator_reader reader(window->state->gtt, offset);
      uint32_t shader_length =
         gen_disasm_get_assembly_size(context.disasm, reader.gen());
      void *data = malloc(shader_length);
      memory_read(window->state->gtt, offset, data, shader_length);

      FILE *output = open_memstream(&window->shader, &window->shader_len);
      gen_disasm_disassemble(context.disasm, data, 0, output);
      fclose(output);
      free(data);
      if (window->shader)
         window->shader_len = strlen(window->shader);
   }

   list_add(&window->base.windows, &context.windows);
}

/* State windows */

static void
display_state_window(struct window *win)
{
   struct state_window *window = (struct state_window *) win;
   struct gpu_state *state = window->state ? window->state : context.last_gpu_state;

   if (window_has_ctrl_key('f')) ImGui::SetKeyboardFocusHere();
   char name[40];
   snprintf(name, sizeof(name), "filter##%p", window);
   window->cmd_filter.Draw(name);

   ImGui::Text("state items: %i\n", _mesa_hash_table_num_entries(state->items)); ImGui::SameLine();
   ImGui::Checkbox("Collapse commands", &window->collapsed); ImGui::SameLine();
   ImGui::Checkbox("Dwords", &window->options.show_dwords);

   if (ImGui::Button("Show URB")) { new_urb_window(state); } ImGui::SameLine();
   if (ImGui::Button("Show sampling pattern")) new_pattern_window(state);

   ImGui::BeginChild(ImGui::GetID("##block"));
   struct hash_entry *entry;
   hash_table_foreach(state->items, entry) {
      struct gpu_state_item *item = (struct gpu_state_item *) entry->data;
      ImGui::PushID(item);
      if (window->cmd_filter.PassFilter(item->group->name) &&
          ImGui::TreeNodeEx(item->group->name,
                            ((window->collapsed ? 0 : ImGuiTreeNodeFlags_DefaultOpen) |
                             ImGuiTreeNodeFlags_Framed))) {
         struct aubinator_reader child_reader(item->gtt, item->gtt_offset);
         decode_group(item->group, child_reader.gen(), &window->options);

         struct hash_entry *entry =
            _mesa_hash_table_search(context.display_handlers, item->group->name);
         if (entry) {
            instruction_handler ui_handler = (instruction_handler) entry->data;
            ui_handler(state, child_reader, &window->options);
         }

         ImGui::TreePop();
      }
      ImGui::PopID();
   }
   ImGui::EndChild();
}

static void
destroy_state_window(struct window *win)
{
   struct state_window *window = (struct state_window *) win;

   gpu_state_free(window->state);
   free(window);
}

static void
new_state_window(struct gpu_state *state)
{
   struct state_window *window = xtzalloc(struct state_window);

   build_window_title(&window->base, "State view", state);
   window->base.position = ImVec2(-1, -1);
   window->base.size = ImVec2(500, 600);
   window->base.opened = true;
   window->base.display = display_state_window;
   window->base.destroy = destroy_state_window;

   window->collapsed = true;
   window->state = state ? gpu_state_copy(state) : NULL;

   list_add(&window->base.windows, &context.windows);
}

/* URB windows */

static void
display_urb_window(struct window *win)
{
   struct urb_window *window = (struct urb_window *) win;
   const char *stage_names[] = { "VS", "GS", "DS", "HS", "PS", "MEDIA" };

   Aubinator::BeginURB("URB layout", ARRAY_SIZE(window->allocations) * 2,
                       window->max_allocation,
                       ImVec2(ImGui::GetContentRegionAvailWidth(), 0));
   for (uint32_t s = 0; s < ARRAY_SIZE(window->allocations); s++) {
      Aubinator::URBItem(stage_names[s],
                         ImVec2(window->allocations[s].offset,
                                window->allocations[s].length),
                         &window->allocations[s].color);
      char title[20];
      snprintf(title, sizeof(title), "%s constants", stage_names[s]);
      Aubinator::URBItem(title,
                         ImVec2(window->constants[s].offset,
                                window->constants[s].length),
                         &window->constants[s].color);
   }
   Aubinator::EndURB();
   ImGui::Text("URB total size: %u", window->max_allocation);

   for (uint32_t s = 0; s < ARRAY_SIZE(window->allocations); s++) {
      ImGui::ColorButton(stage_names[s],
                         window->allocations[s].length == 0 ?
                         ImColor(131, 131, 131) :
                         window->allocations[s].color,
                         ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
      ImGui::SameLine();
      ImGui::Text("%s: (%s) offset: %u, length: %u", stage_names[s],
                  window->allocations[s].length == 0 ? "disabled" : "enabled",
                  window->allocations[s].offset,
                  window->allocations[s].length);
   }
   for (uint32_t s = 0; s < ARRAY_SIZE(window->allocations); s++) {
      ImGui::ColorButton(stage_names[s],
                         window->constants[s].length == 0 ?
                         ImColor(131, 131, 131) :
                         window->constants[s].color,
                         ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
      ImGui::SameLine();
      char title[20];
      snprintf(title, sizeof(title), "%s constants", stage_names[s]);
      ImGui::Text("%s: (%s) offset: %u, length: %u", title,
                  window->constants[s].length == 0 ? "disabled" : "enabled",
                  window->constants[s].offset,
                  window->constants[s].length);
   }
}

static void
destroy_urb_window(struct window *win)
{
   struct urb_window *window = (struct urb_window *) win;

   gpu_state_free(window->state);
   free(window);
}

static bool
get_allocation(struct gpu_state *state,
               const char *instruction,
               const char *offset_field,
               uint32_t *offset,
               uint32_t offset_units,
               const char *length_field,
               uint32_t *length,
               uint32_t length_units)
{
   struct gpu_state_item *item = gpu_state_find(state, instruction);
   if (!item)
      return false;

   struct gen_field *gen_offset_field =
      gen_group_find_field(item->group, offset_field);
   struct gen_field *gen_length_field =
      gen_group_find_field(item->group, length_field);
   if (!gen_offset_field || !gen_length_field)
      return false;

   struct aubinator_reader reader(item->gtt, item->gtt_offset);

   union gen_field_value value = { .u64 = 0 };
   gen_field_decode(gen_offset_field, reader.gen(), &value);
   *offset = (uint32_t) value.u64 * offset_units;
   gen_field_decode(gen_length_field, reader.gen(), &value);
   *length = (uint32_t) value.u64 * length_units;

   return true;
}

static bool
get_constant_allocation(struct gpu_state *state, const char *instruction,
                        uint32_t *offset, uint32_t *length)
{
   struct gpu_state_item *item = gpu_state_find(state, instruction);
   if (!item)
      return false;

   struct gen_field *gen_offset_field =
      gen_group_find_field(item->group, "Constant Buffer Offset");
   struct gen_field *gen_length_field =
      gen_group_find_field(item->group, "Constant Buffer Size");
   if (!gen_offset_field || !gen_length_field)
      return false;

   struct aubinator_reader reader(item->gtt, item->gtt_offset);

   union gen_field_value value = { .u64 = 0 };
   gen_field_decode(gen_offset_field, reader.gen(), &value);
   *offset = (uint32_t) value.u64 * 1024;
   gen_field_decode(gen_length_field, reader.gen(), &value);
   *length = (uint32_t) value.u64 * 1024;

   return true;
}

static void
new_urb_window(struct gpu_state *state)
{
   struct urb_window *window = xtzalloc(struct urb_window);

   snprintf(window->base.name, sizeof(window->base.name),
            "URB view##%p", window);
   window->base.position = ImVec2(-1, -1);
   window->base.size = ImVec2(500, 300);
   window->base.opened = true;
   window->base.display = display_urb_window;
   window->base.destroy = destroy_urb_window;

   window->state = gpu_state_copy(state);

   list_add(&window->base.windows, &context.windows);

   const uint32_t urb_offset_units = 8 * 1024; /* 8Kb units */
   const uint32_t urb_length_units = 64; /* 512-bit units */
   const struct {
      const char *inst; const char *offset; const char *length;
   } alloc_fields[] {
      { "3DSTATE_URB_VS", "VS URB Starting Address", "VS Number of URB Entries" },
      { "3DSTATE_URB_GS", "GS URB Starting Address", "GS Number of URB Entries" },
      { "3DSTATE_URB_DS", "DS URB Starting Address", "DS Number of URB Entries" },
      { "3DSTATE_URB_HS", "HS URB Starting Address", "HS Number of URB Entries" },
   };
   for (uint32_t s = 0; s < ARRAY_SIZE(alloc_fields); s++) {
      get_allocation(window->state, alloc_fields[s].inst,
                     alloc_fields[s].offset, &window->allocations[s].offset, urb_offset_units,
                     alloc_fields[s].length, &window->allocations[s].length, urb_length_units);
   }

   const char *const_insts[] = {
      "3DSTATE_PUSH_CONSTANT_ALLOC_VS",
      "3DSTATE_PUSH_CONSTANT_ALLOC_GS",
      "3DSTATE_PUSH_CONSTANT_ALLOC_DS",
      "3DSTATE_PUSH_CONSTANT_ALLOC_HS",
      "3DSTATE_PUSH_CONSTANT_ALLOC_PS",
   };
   for (uint32_t s = 0; s < ARRAY_SIZE(const_insts); s++) {
      get_constant_allocation(window->state, const_insts[s],
                              &window->constants[s].offset,
                              &window->constants[s].length);
   }

   window->max_allocation = 0;
   for (uint32_t s = 0; s < ARRAY_SIZE(window->allocations); s++) {
      window->max_allocation = MAX3(window->max_allocation,
                                    window->allocations[s].offset +
                                    window->allocations[s].length,
                                    window->constants[s].offset +
                                    window->constants[s].length);
   }
}

/**/

static void
destroy_window_noop(struct window *win)
{
}

static void
display_registers_window(struct window *win)
{
   static struct ImGuiTextFilter filter;
   if (window_has_ctrl_key('f')) ImGui::SetKeyboardFocusHere();
   filter.Draw();

   ImGui::BeginChild(ImGui::GetID("##block"));
   struct hash_entry *entry;
   hash_table_foreach(context.spec->registers_by_name, entry) {
      struct gen_group *reg = (struct gen_group *) entry->data;
      if (filter.PassFilter(reg->name) &&
          ImGui::CollapsingHeader(reg->name)) {
         const struct gen_field *field = reg->fields;
         while (field) {
            ImGui::Text("%s : %i -> %i\n", field->name, field->start, field->end);
            field = field->next;
         }
      }
   }
   ImGui::EndChild();
}

static void
show_register_window(void)
{
   struct window *window = &context.registers_window;

   if (window->opened) {
      window->opened = false;
      return;
   }

   snprintf(window->name, sizeof(window->name), "Registers");
   window->position = ImVec2(-1, -1);
   window->size = ImVec2(200, 400);
   window->opened = true;
   window->display = display_registers_window;
   window->destroy = destroy_window_noop;

   list_add(&window->windows, &context.windows);
}

static void
display_commands_window(struct window *win)
{
   static struct ImGuiTextFilter cmd_filter;
   if (window_has_ctrl_key('f')) ImGui::SetKeyboardFocusHere();
   cmd_filter.Draw("name filter");
   static struct ImGuiTextFilter field_filter;
   field_filter.Draw("field filter");

   static char opcode_str[9] = { 0, };
   ImGui::InputText("opcode filter", opcode_str, sizeof(opcode_str),
                    ImGuiInputTextFlags_CharsHexadecimal);
   size_t opcode_len = strlen(opcode_str);
   uint64_t opcode = strtol(opcode_str, NULL, 16);

   static bool show_dwords = true;
   ImGui::Checkbox("Dwords", &show_dwords);

   ImGui::BeginChild(ImGui::GetID("##block"));
   struct hash_entry *entry;
   hash_table_foreach(context.spec->commands, entry) {
      struct gen_group *cmd = (struct gen_group *) entry->data;
      if ((cmd_filter.PassFilter(cmd->name) &&
           (opcode_len == 0 || (opcode & cmd->opcode_mask) == cmd->opcode)) &&
          ImGui::CollapsingHeader(cmd->name)) {
         const struct gen_field *field = cmd->fields;
         int32_t last_dword = -1;
         while (field) {
            if (show_dwords && field->start / 32 != last_dword) {
               for (last_dword = MAX(0, last_dword + 1);
                    last_dword < field->start / 32; last_dword++) {
                  ImGui::TextColored(context.dwords_color,
                                     "Dword %d", last_dword);
               }
               ImGui::TextColored(context.dwords_color, "Dword %d", last_dword);
            }
            if (field_filter.PassFilter(field->name))
               ImGui::Text("%s : %i -> %i\n", field->name, field->start, field->end);
            field = field->next;
         }
      }
   }
   hash_table_foreach(context.spec->structs, entry) {
      struct gen_group *cmd = (struct gen_group *) entry->data;
      if (cmd_filter.PassFilter(cmd->name) && opcode_len == 0 &&
          ImGui::CollapsingHeader(cmd->name)) {
         const struct gen_field *field = cmd->fields;
         int32_t last_dword = -1;
         while (field) {
            if (show_dwords && field->start / 32 != last_dword) {
               last_dword = field->start / 32;
               ImGui::TextColored(context.dwords_color,
                                  "Dword %d", last_dword);
            }
            if (field_filter.PassFilter(field->name))
               ImGui::Text("%s : %i -> %i\n", field->name, field->start, field->end);
            field = field->next;
         }
      }
   }
   ImGui::EndChild();
}

static void
show_commands_window(void)
{
   struct window *window = &context.commands_window;

   if (window->opened) {
      window->opened = false;
      return;
   }

   snprintf(window->name, sizeof(window->name), "Commands & structs");
   window->position = ImVec2(-1, -1);
   window->size = ImVec2(300, 400);
   window->opened = true;
   window->display = display_commands_window;
   window->destroy = destroy_window_noop;

   list_add(&window->windows, &context.windows);
}

/* Main window */

static const char *
human_size(size_t size)
{
   unsigned divisions = 0;
   double v = size;
   double divider = 1024;
   while (v >= divider) {
      v /= divider;
      divisions++;
   }

   static const char *units[] = { "Bytes", "Kilobytes", "Megabytes", "Gigabytes" };
   static char result[20];
   snprintf(result, sizeof(result), "%.2f %s",
            v, divisions >= ARRAY_SIZE(units) ? "Too much!" : units[divisions]);
   return result;
}

static void
display_aubfile_window(struct window *win)
{
   ImGuiColorEditFlags cflags = (ImGuiColorEditFlags_NoAlpha |
                                 ImGuiColorEditFlags_NoLabel |
                                 ImGuiColorEditFlags_NoInputs);
   ImGui::ColorEdit3("background", (float *)&context.clear_color, cflags); ImGui::SameLine();
   ImGui::ColorEdit3("error", (float *)&context.error_color, cflags); ImGui::SameLine();
   ImGui::ColorEdit3("highlight", (float *)&context.highlight_color, cflags); ImGui::SameLine();
   ImGui::ColorEdit3("dwords", (float *)&context.dwords_color, cflags); ImGui::SameLine();
   if (ImGui::Button("Commands list") || has_ctrl_key('c')) { show_commands_window(); } ImGui::SameLine();
   if (ImGui::Button("Registers list") || has_ctrl_key('r')) { show_register_window(); } ImGui::SameLine();
   if (ImGui::Button("Help") || has_ctrl_key('h')) { ImGui::OpenPopup("Help"); }

   if (ImGui::Button("Previous batch") || has_ctrl_key('p')) aub_file_read_block(MAX(context.command_index - 1, 0));
   ImGui::SameLine();
   ImGui::Text("%04i", context.command_index);
   ImGui::SameLine();
   if (ImGui::Button("Next batch") || has_ctrl_key('n')) aub_file_read_block(context.command_index + 1);

   if (ImGui::Button("New batch window") || has_ctrl_key('b')) { new_batch_window(false); } ImGui::SameLine();
   if (ImGui::Button("New decode window")) { new_as_window(NULL, 0); } ImGui::SameLine();
   if (ImGui::Button("New image window")) { new_image_window(context.last_gpu_state, 0); } ImGui::SameLine();
   if (ImGui::Button("New state window") || has_ctrl_key('s')) new_state_window(NULL);

   display_file_props();

   ImGui::Text("Memory consumption: %s\n", human_size(memory_size()));
   ImGui::Text("GPU states: %i\n",
               _mesa_hash_table_num_entries(context.gpu_states->table));
   ImGui::Text("last GPU state items: %i\n",
               _mesa_hash_table_num_entries(context.last_gpu_state->items));

   ImGui::SetNextWindowContentWidth(500);
   if (ImGui::BeginPopupModal("Help", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("Some global keybindings:");
      ImGui::Separator();

      static const char *texts[] = {
         "Ctrl-h",          "show this screen",
         "Ctrl-c",          "show commands list",
         "Ctrl-r",          "show registers list",
         "Ctrl-b",          "new batch window",
         "Ctrl-s",          "new state window",
         "Ctrl-p/n",        "switch to previous/next batch buffer",
         "Ctrl-Tab",        "switch focus between window",
         "Ctrl-left/right", "align window to the side of the screen",
      };
      float align = 0.0f;
      for (uint32_t i = 0; i < ARRAY_SIZE(texts); i += 2)
         align = MAX(align, ImGui::CalcTextSize(texts[i]).x);
      align += ImGui::GetStyle().WindowPadding.x + 10;

      for (uint32_t i = 0; i < ARRAY_SIZE(texts); i += 2) {
         ImGui::Text(texts[i]); ImGui::SameLine(align); ImGui::Text(texts[i + 1]);
      }

      if (ImGui::Button("Done") || ImGui::IsKeyPressed(ImGuiKey_Escape))
         ImGui::CloseCurrentPopup();
      ImGui::EndPopup();
   }
}

static void
show_aubfile_window(void)
{
   struct window *window = &context.file_window;

   if (window->opened)
      return;

   snprintf(window->name, sizeof(window->name),
            "Aubinator: Intel AUB file decoder");
   window->size = ImVec2(-1, 200);
   window->position =
      ImVec2(0, ImGui::GetIO().DisplaySize.y - window->size.y);
   window->opened = true;
   window->display = display_aubfile_window;
   window->destroy = NULL;

   list_add(&window->windows, &context.windows);
}

/* Focus management */

static struct window *
update_focus(void)
{
   int focus_dir = (ImGui::GetIO().KeyCtrl &&
                    ImGui::GetIO().KeysDownDuration[ImGuiKey_Tab] == 0.0f) ?
      (ImGui::GetIO().KeyShift ? -1 : 1) :
      0;

   struct window *focus = context.focused_window;
   if (!context.focused_window) {
      focus = context.focused_window =
         list_first_entry(&context.windows, struct window, windows);
      return focus;
   }

   /* Only modify the list of windows on release of Ctrl key. */
   if (!ImGui::GetIO().KeyCtrl) {
      list_del(&context.focused_window->windows);
      list_add(&context.focused_window->windows, &context.windows);
   }

   if (focus_dir == 0)
      return NULL;

   if (focus_dir < 0) {
      if (focus->windows.prev == &context.windows)
         focus = list_last_entry(&context.windows, struct window, windows);
      else
         focus = LIST_ENTRY(struct window, focus->windows.prev, windows);
   } else {
      if (focus->windows.next == &context.windows)
         focus = list_first_entry(&context.windows, struct window, windows);
      else
         focus = LIST_ENTRY(struct window, focus->windows.next, windows);
   }

   context.focused_window = focus;

   return focus;
}

static void
handle_resize(struct window *window)
{
   ImVec2 screen_size = ImGui::GetIO().DisplaySize;

   if (window != context.focused_window) {
      ImGui::SetNextWindowPos(window->position, ImGuiCond_FirstUseEver);
      ImGui::SetNextWindowSize(window->size, ImGuiCond_FirstUseEver);
      return;
   }

   if (has_ctrl_imgui_key(ImGuiKey_LeftArrow)) {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(window->size.x, screen_size.y));
   } else if (has_ctrl_imgui_key(ImGuiKey_RightArrow)) {
      ImGui::SetNextWindowPos(ImVec2(screen_size.x - window->size.x, 0));
      ImGui::SetNextWindowSize(ImVec2(window->size.x, screen_size.y));
   }
}

/* Main redrawing */

static void
display_windows(void)
{
   struct window *new_focus = update_focus();

   /* Start by disposing closed windows, we don't want to destroy windows that
    * have already been scheduled to be painted. So destroy always happens on
    * the next draw cycle, prior to any drawing.
    */
   list_for_each_entry_safe(struct window, window,
                            &context.windows, windows) {
      if (window->opened)
         continue;

      if (window->destroy) {
         list_del(&window->windows);
         if (window == context.focused_window) {
            context.focused_window = list_first_entry(&context.windows,
                                                      struct window, windows);
         }
         window->destroy(window);
      } else
         window->opened = true;
   }

   list_for_each_entry(struct window, window, &context.windows, windows) {
      handle_resize(window);
      if (new_focus && window == context.focused_window) ImGui::SetNextWindowFocus();
      ImGui::Begin(window->name, &window->opened);
      if (!new_focus && ImGui::IsRootWindowOrAnyChildFocused()) context.focused_window = window;
      window->display(window);
      window->position = ImGui::GetWindowPos();
      window->size = ImGui::GetWindowSize();
      ImGui::End();
   }

   if (has_ctrl_key('w') &&
       context.focused_window != &context.file_window) {
      context.focused_window->opened = false;
   }
}

static void repaint_window(CoglOnscreen *onscreen, gpointer user_data)
{
   ImGui_ImplGtk3Cogl_NewFrame();

   show_aubfile_window();

   display_windows();

   /* Rendering */
   CoglFramebuffer *fb = COGL_FRAMEBUFFER(onscreen);
   cogl_framebuffer_set_viewport(fb,
                                 0, 0,
                                 cogl_framebuffer_get_width(fb),
                                 cogl_framebuffer_get_height(fb));

   cogl_framebuffer_clear4f(fb, COGL_BUFFER_BIT_COLOR | COGL_BUFFER_BIT_DEPTH,
                            context.clear_color.x,
                            context.clear_color.y,
                            context.clear_color.z, 1.0);
   ImGui::Render();
   cogl_onscreen_swap_buffers(onscreen);
}

static void
print_help(const char *progname, FILE *file)
{
   fprintf(file,
           "Usage: %s [OPTION]... [FILE]\n"
           "Decode aub file contents from either FILE or the standard input.\n\n"
           "A valid --gen option must be provided.\n\n"
           "      --help          display this help and exit\n"
           "      --gen=platform  decode for given platform (ivb, byt, hsw, bdw, chv, skl, kbl, bxt or cnl)\n"
           "      --headers       decode only command headers\n"
           "      --color[=WHEN]  colorize the output; WHEN can be 'auto' (default\n"
           "                        if omitted), 'always', or 'never'\n"
           "      --no-offsets    don't print instruction offsets\n"
           "      --xml=DIR       load hardware xml description from directory DIR\n",
           progname);
}

int main(int argc, char *argv[])
{
   int c, i;
   bool help = false;
   const struct option aubinator_opts[] = {
      { "help",       no_argument,       (int *) &help,                 true },
      { "gen",        required_argument, NULL,                          'g' },
      { "xml",        required_argument, NULL,                          'x' },
      { NULL,         0,                 NULL,                          0 }
   };

   memset(&context, 0, sizeof(context));

   i = 0;
   while ((c = getopt_long(argc, argv, "", aubinator_opts, &i)) != -1) {
      switch (c) {
      case 'g':
         context.pci_id = 0;
         for (unsigned g = 0; g < ARRAY_SIZE(supported_gens); g++) {
            if (!strcmp(optarg, supported_gens[g].name)) {
               context.pci_id = supported_gens[g].pci_id;
               break;
            }
         }
         if (context.pci_id == 0) {
            fprintf(stderr, "can't parse gen: '%s', expected ivb, byt, hsw, "
                                   "bdw, chv, skl, kbl or bxt\n", optarg);
            exit(EXIT_FAILURE);
         }
         break;
      case 'x':
         context.xml_path = strdup(optarg);
         break;
      default:
         break;
      }
   }

   if (help || argc == 1) {
      print_help(argv[0], stderr);
      exit(0);
   }

   if (optind < argc)
      context.input_file = argv[optind];

   if (context.input_file == NULL)
      context.file = aub_file_stdin();
   else
      context.file = aub_file_open(context.input_file);

   context.accumulation_handlers = init_accumulation_handlers();
   context.display_handlers = init_display_handlers();

   context.clear_color = ImColor(114, 144, 154);
   context.dwords_color = ImColor(29, 177, 194, 255);
   context.error_color = ImColor(236, 255, 0, 255);
   context.highlight_color = ImColor(0, 230, 0, 255);

   context.gpu_states = _mesa_hash_table_u64_create(NULL);
   context.last_gpu_state = gpu_state_new();
   _mesa_hash_table_u64_insert(context.gpu_states, 1, context.last_gpu_state);

   list_inithead(&context.windows);

   aub_file_read_headers();
   aub_file_read_block(0);

   gtk_init(&argc, &argv);

   GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
   gtk_window_resize(GTK_WINDOW(window), 800, 600);

   GtkWidget *box = gtk_event_box_new();
   gtk_widget_set_can_focus(box, TRUE);
   gtk_container_add(GTK_CONTAINER(window), box);
   gtk_widget_show_all(window);

   UNUSED CoglOnscreen *onscreen = ImGui_ImplGtk3Cogl_Init(box, repaint_window, NULL);

   char *config_file = NULL;
   if (context.input_file) {
      size_t max_len = strlen(context.input_file) + 10;
      config_file = (char *) malloc(max_len);
      snprintf(config_file, max_len, "%s.ini", context.input_file);
   }
   ImGui::GetIO().IniFilename = config_file;
   ImGui::GetIO().Fonts->AddFontDefault();
   //ImGui::GetIO().Fonts->AddFontFromFileTTF("./Roboto-Medium.ttf", 16.0f);

   gtk_main();

   free(context.xml_path);

   return EXIT_SUCCESS;
}
