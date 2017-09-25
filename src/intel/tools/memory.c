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

#undef NDEBUG

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "util/macros.h"

#define LEAF_SIZE  (4096UL)

#define BASE_SHIFT   (12)
#define BRANCH_SHIFT (5)
#define BRANCH_MASK  (0x1f)

#define LEAF_MASK    (~(LEAF_SIZE - 1UL))

#define MAX_LEVEL    (11)

#define BRANCH(offset, level) \
   (((offset) >> (BASE_SHIFT + BRANCH_SHIFT * (level))) & BRANCH_MASK)

static inline unsigned
branch(uint64_t offset, unsigned level)
{
   offset >>= BASE_SHIFT;
   return (offset >> (BRANCH_SHIFT * (MAX_LEVEL - level))) & BRANCH_MASK;
}

typedef struct {
   unsigned refcount;

   enum {
      MEMORY_NODE_TYPE_BRANCH,
      MEMORY_NODE_TYPE_LEAF,
   } type;
} memory_node;

typedef struct {
   memory_node base;

   uint64_t offset;
   uint8_t data[];
} memory_leaf_node;

typedef struct {
   memory_node base;

   memory_node *children[32];
   unsigned level;
} memory_branch_node;

struct _memory_t {
   memory_node *root;

   uint32_t refcount;

   uint64_t base_offset;
   uint64_t start;
   uint64_t end;

   uint64_t last_leaf_offset;
   memory_leaf_node *last_leaf;
};

/**/

struct memory_iterator {
   memory_node *path[MAX_LEVEL + 2];
   unsigned n_path;

   uint64_t offset;
   bool create;
};

static uint32_t n_branches = 0;
static uint32_t n_leafs = 0;

size_t
memory_size(void)
{
   return (LEAF_SIZE + sizeof(memory_leaf_node)) * n_leafs +
          sizeof(memory_branch_node) * n_branches;
}

static memory_node *
memory_node_ref(memory_node *node)
{
   if (!node)
      return NULL;

   node->refcount++;
   return node;
}

static void
memory_node_unref(memory_node *node)
{
   if (!node)
      return;

   node->refcount--;
   if (node->refcount == 0) {
      if (node->type == MEMORY_NODE_TYPE_BRANCH) {
         memory_branch_node *branch = (memory_branch_node *) node;
         for (unsigned i = 0; i < ARRAY_SIZE(branch->children); i++)
            memory_node_unref(branch->children[i]);
         n_branches--;
      } else
         n_leafs--;
      free(node);
   }
}

static memory_node *
memory_leaf_node_new(uint64_t offset)
{
   memory_leaf_node *node = calloc(1, LEAF_SIZE + sizeof(*node));
   node->base.type = MEMORY_NODE_TYPE_LEAF;
   node->base.refcount = 1;
   node->offset = offset;

   n_leafs++;

   return &node->base;
}

static memory_node *
memory_leaf_node_copy(memory_leaf_node *src)
{
   memory_leaf_node *node = calloc(1, LEAF_SIZE + sizeof(*node));
   node->base.type = MEMORY_NODE_TYPE_LEAF;
   node->base.refcount = 1;
   node->offset = src->offset;
   memcpy(node->data, src->data, LEAF_SIZE);

   n_leafs++;

   return &node->base;
}

static memory_node *
memory_branch_node_new(unsigned level)
{
   memory_branch_node *node = calloc(1, LEAF_SIZE + sizeof(*node));
   node->base.type = MEMORY_NODE_TYPE_BRANCH;
   node->base.refcount = 1;
   node->level = level;

   n_branches++;

   return &node->base;
}

static memory_node *
memory_branch_node_copy(memory_branch_node *src)
{
   memory_branch_node *node = calloc(1, LEAF_SIZE + sizeof(*node));
   node->base.type = MEMORY_NODE_TYPE_BRANCH;
   node->base.refcount = 1;
   node->level = src->level;

   for (unsigned i = 0; i < ARRAY_SIZE(src->children); i++)
      node->children[i] = memory_node_ref(src->children[i]);

   n_branches++;

   return &node->base;
}

static bool
find_path(struct memory_iterator *iter)
{
   if (iter->path[iter->n_path - 1]->type == MEMORY_NODE_TYPE_LEAF)
      return true;

   memory_branch_node *bnode =
      (memory_branch_node *) iter->path[iter->n_path - 1];
   unsigned child = branch(iter->offset, iter->n_path - 1);
   assert(child < ARRAY_SIZE(bnode->children));
   if (!bnode->children[child]) {
      if (!iter->create)
         return false;

      if (bnode->level == MAX_LEVEL) {
         bnode->children[child] =
            memory_leaf_node_new(iter->offset - iter->offset % LEAF_SIZE);
      } else {
         bnode->children[child] = memory_branch_node_new(bnode->level + 1);
      }
   }

   iter->path[iter->n_path++] = bnode->children[child];
   assert(iter->n_path <= ARRAY_SIZE(iter->path));
   return find_path(iter);
}

/**/

memory_t *
memory_new(void)
{
   memory_t *mem = calloc(1, sizeof(*mem));
   mem->base_offset = 0;
   mem->root = memory_branch_node_new(0);
   mem->refcount = 1;

   return mem;
}

memory_t *
memory_ref(memory_t *mem)
{
   mem->refcount++;

   return mem;
}

void
memory_unref(memory_t *mem)
{
   if (--mem->refcount == 0) {
      memory_node_unref(mem->root);
      free(mem);
   }
}

static memory_t *
memory_copy(memory_t *parent_mem)
{
   memory_t *mem = calloc(1, sizeof(*mem));

   mem->base_offset = parent_mem->base_offset;
   mem->root = memory_node_ref(parent_mem->root);
   mem->start = parent_mem->start;
   mem->end = parent_mem->end;
   mem->refcount = 1;

   return mem;
}


uint32_t
memory_read_dword(memory_t *mem, uint64_t offset)
{
   assert(offset % 4 == 0);

   /* Reading dwords is usually contiguous, so it saves quite a bit of
    * computation to keep pointer to the last accessed leaf.
    */
   memory_leaf_node *leaf;
   if (!mem->last_leaf ||
       (mem->last_leaf_offset & LEAF_MASK) != (offset & LEAF_MASK)) {
      struct memory_iterator iter = {
         .n_path = 1,
         .offset = offset,
         .create = false,
      };
      iter.path[0] = mem->root;
      if (!find_path(&iter))
         return 0;

      mem->last_leaf = leaf = (memory_leaf_node *) iter.path[iter.n_path - 1];
      mem->last_leaf_offset = offset;
   } else {
      leaf = mem->last_leaf;
   }
   /* fprintf(stderr, "read offset=%llu:\n", offset); */
   /* for (unsigned i = 0; i < iter.n_path; i++) */
   /*    fprintf(stderr, "\tpath=%i %p\n", i, iter.path[i]); */

   uint32_t *data = (uint32_t *) leaf->data;
   uint64_t node_offset = offset % LEAF_SIZE;
   return data[node_offset / 4];
}

static void
update_path(memory_node **nodes, unsigned n_nodes, uint64_t leaf_offset)
{
   for (unsigned n = 0; n < n_nodes; n++) {
      unsigned idx = n_nodes - n - 1;

      /* memory_node *old = nodes[idx]; */

      if (nodes[idx]->type == MEMORY_NODE_TYPE_BRANCH) {
         memory_branch_node *tmp =
            (memory_branch_node *) memory_branch_node_copy((memory_branch_node *) nodes[idx]);
         nodes[idx] = &tmp->base;

         uint64_t child = branch(leaf_offset, MAX_LEVEL - n + 1);
         memory_node_unref(tmp->children[child]);
         tmp->children[child] = nodes[idx + 1];
      } else {
         nodes[idx] = memory_leaf_node_copy((memory_leaf_node *) nodes[idx]);
      }

      /* memory_node *new = nodes[idx]; */

      /* fprintf(stderr, "\tupdate_path idx=%i old=%p new=%p child_idx=%i\n", */
      /*         idx, old, new, branch(leaf_offset, MAX_LEVEL - n)); */
   }
}

void
memory_read(memory_t *mem, uint64_t offset, void *data, size_t length)
{
   while (length > 0) {
      struct memory_iterator iter = {
         .n_path = 1,
         .offset = offset,
         .create = true,
      };
      iter.path[0] = mem->root;
      find_path(&iter);

      memory_leaf_node *leaf = (memory_leaf_node *) iter.path[iter.n_path - 1];

      size_t copy_length = MIN2(length, LEAF_SIZE - (offset - leaf->offset));
      memcpy(data, &leaf->data[offset - leaf->offset], copy_length);

      offset += copy_length;
      data += copy_length;
      length -= copy_length;
   }
}

memory_t *
memory_write(memory_t *parent_mem, uint64_t offset, void *data, size_t length)
{
   memory_t *mem = memory_copy(parent_mem);

   while (length > 0) {
      struct memory_iterator iter = {
         .n_path = 1,
         .offset = offset,
         .create = true,
      };
      iter.path[0] = mem->root;
      find_path(&iter);
      /* fprintf(stderr, "pre update offset=%llu:\n", offset); */
      /* for (unsigned i = 0; i < iter.n_path; i++) */
      /*    fprintf(stderr, "\tpath=%i %p\n", i, iter.path[i]); */
      /* memory_node *src = iter.path[iter.n_path - 1]; */
      update_path(iter.path, iter.n_path, iter.offset);
      find_path(&iter);
      /* fprintf(stderr, "post update offset=%llu:\n", offset); */
      /* for (unsigned i = 0; i < iter.n_path; i++) */
      /*    fprintf(stderr, "\tpath=%i %p\n", i, iter.path[i]); */

      memory_leaf_node *dst = (memory_leaf_node *) iter.path[iter.n_path - 1];
      size_t copy_length = MIN2(length, LEAF_SIZE - (offset - dst->offset));
      memcpy(&dst->data[offset - dst->offset], data, copy_length);
      /* fprintf(stderr, "=> write into dst=%p src=%p\n", dst, src); */

      assert(copy_length != 0);

      offset += copy_length;
      data += copy_length;
      length -= copy_length;

      memory_node_unref(mem->root);
      mem->root = iter.path[0];
   }

   mem->start = MIN2(mem->start, offset);
   mem->end   = MAX2(mem->end,   offset + length);

   return mem;
}

memory_t *
memory_write_unref(memory_t *mem, uint64_t offset, void *data, size_t length)
{
   memory_t *new_mem = memory_write(mem, offset, data, length);
   memory_unref(mem);
   return new_mem;
}

uint64_t
memory_start(memory_t *mem)
{
   return mem->start;
}

uint64_t
memory_end(memory_t *mem)
{
   return mem->end;
}

#ifdef BUILD_TESTS
int
main(int argc, char *argv[])
{
   {
      uint64_t leaf_size = LEAF_SIZE;

      for (unsigned i = 0; i <= MAX_LEVEL; i++)
         assert(branch(0, i) == 0);

      for (unsigned i = 0; i < MAX_LEVEL; i++)
         assert(branch(leaf_size, i) == 0);
      assert(branch(leaf_size, MAX_LEVEL) == 1);

      for (unsigned i = 0; i < MAX_LEVEL; i++)
         assert(branch(leaf_size * 2ULL, i) == 0);
      assert(branch(leaf_size * 2ULL, MAX_LEVEL) == 2);

      for (unsigned i = 0; i < (MAX_LEVEL - 1); i++)
         assert(branch(leaf_size * 32ULL, 0) == 0);
      assert(branch(leaf_size * 32ULL, MAX_LEVEL - 1) == 1);
      assert(branch(leaf_size * 32ULL, MAX_LEVEL) == 0);
   }

   {
      memory_t *mem1 = memory_new();

      char *data = "PlopPlopPlop";
      memory_t *mem2 = memory_write(mem1, 0, data, strlen(data));

      assert(mem1 != mem2);

      assert(memory_read_dword(mem1, 0) == 0);
      assert(((memory_read_dword(mem2, 0) >> 0) & 0xff)  == 'P');
      assert(((memory_read_dword(mem2, 0) >> 8) & 0xff)  == 'l');
      assert(((memory_read_dword(mem2, 0) >> 16) & 0xff) == 'o');
      assert(((memory_read_dword(mem2, 0) >> 24) & 0xff) == 'p');

      memory_unref(mem1);
      memory_unref(mem2);

      assert(memory_size() == 0);
   }

   {
      memory_t *mem1 = memory_new();

      char *data = "PlopPlopPlop";
      memory_t *mem2 = memory_write(mem1, LEAF_SIZE, data, strlen(data));

      assert(mem1 != mem2);

      assert(memory_read_dword(mem1, LEAF_SIZE) == 0);
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 0) & 0xff)  == 'P');
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 8) & 0xff)  == 'l');
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 16) & 0xff) == 'o');
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 24) & 0xff) == 'p');

      memory_unref(mem1);
      memory_unref(mem2);

      assert(memory_size() == 0);
   }

   {
      memory_t *mem1 = memory_new();

      char *data = "PlopPlopPlop";
      memory_t *mem2 = memory_write(mem1, LEAF_SIZE, data, strlen(data));
      assert(mem1 != mem2);

      assert(memory_read_dword(mem1, LEAF_SIZE) == 0);
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 0) & 0xff)  == 'P');
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 8) & 0xff)  == 'l');
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 16) & 0xff) == 'o');
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 24) & 0xff) == 'p');

      memory_t *mem3 = memory_write(mem1, LEAF_SIZE * 2, data, strlen(data));
      assert(mem2 != mem3);

      assert(memory_read_dword(mem2, LEAF_SIZE * 2) == 0);
      assert(((memory_read_dword(mem3, LEAF_SIZE * 2) >> 0) & 0xff)  == 'P');
      assert(((memory_read_dword(mem3, LEAF_SIZE * 2) >> 8) & 0xff)  == 'l');
      assert(((memory_read_dword(mem3, LEAF_SIZE * 2) >> 16) & 0xff) == 'o');
      assert(((memory_read_dword(mem3, LEAF_SIZE * 2) >> 24) & 0xff) == 'p');

      assert(memory_size() > 0);

      memory_unref(mem1);
      memory_unref(mem2);
      memory_unref(mem3);

      assert(memory_size() == 0);
   }

   {
      memory_t *mem1 = memory_new();

      char *data = "PlopPlipPlop";
      memory_t *mem2 = memory_write(mem1, LEAF_SIZE - 4, data, strlen(data));

      assert(mem1 != mem2);

      assert(memory_read_dword(mem1, 0) == 0);
      assert(memory_read_dword(mem2, 0) == 0);

      assert(((memory_read_dword(mem2, LEAF_SIZE - 4) >> 0) & 0xff)  == 'P');
      assert(((memory_read_dword(mem2, LEAF_SIZE - 4) >> 8) & 0xff)  == 'l');
      assert(((memory_read_dword(mem2, LEAF_SIZE - 4) >> 16) & 0xff) == 'o');
      assert(((memory_read_dword(mem2, LEAF_SIZE - 4) >> 24) & 0xff) == 'p');

      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 0) & 0xff)  == 'P');
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 8) & 0xff)  == 'l');
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 16) & 0xff) == 'i');
      assert(((memory_read_dword(mem2, LEAF_SIZE) >> 24) & 0xff) == 'p');

      char ret[200] = { 0, };
      memory_read(mem2, LEAF_SIZE - 4, ret, strlen(data));

      assert(strcmp(ret, data) == 0);

      memory_unref(mem1);
      memory_unref(mem2);

      assert(memory_size() == 0);
   }

   return 0;
}
#endif
