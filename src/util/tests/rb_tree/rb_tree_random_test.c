/*
 * Copyright © 2018 Intel Corporation
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

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "rb_tree.h"

#define max(a, b) ({                            \
         __typeof(a) _a = (a);                  \
         __typeof(b) _b = (b);                  \
         _a > _b ? _a : _b;                     \
      })

struct item {
   struct rb_node node;

   int value;
   bool in;
};

static int
cmp_item(const struct rb_node *n1, const struct rb_node *n2)
{
   const struct item *i1 = rb_node_data(struct item, n1, node),
      *i2 =  rb_node_data(struct item, n2, node);

   return i1->value - i2->value;
}

static int
cmp_item_val(const struct rb_node *n, const void *val)
{
   const struct item *i = rb_node_data(struct item, n, node);

   return i->value - *(const int *)val;
}

static int
get_new_value(struct rb_tree *tree1, struct rb_tree *tree2)
{
   static int counter = 1;
   return counter++;

   int v;

   do {
      v = rand();
   } while (v == 0 ||
            rb_tree_search(tree1, &v, cmp_item_val) ||
            rb_tree_search(tree2, &v, cmp_item_val));

   return v;
}

static void
add_item(struct rb_tree *tree1, struct rb_tree *tree2, struct item *it)
{
   it->value = get_new_value(tree1, tree2);
   rb_tree_insert(tree1, &it->node, cmp_item);
}

static struct item *
get_random_node(struct item *items, int n_items, bool in)
{
   struct item *it;

   do {
      int r = rand() % n_items;

      it = in ?
         (items[r].in ? &items[r] : NULL) :
         (!items[r].in ? &items[r] : NULL);
   } while (!it);

   return it;
}

int
main(int argc, char *argv[])
{
   int seed = 8675309;
   int init_inserts = 2459;
   int count = 2459;
   struct rb_tree tree1, tree2;
   struct item *items;

   if (argc == 4) {
      seed = atoi(argv[1]);
      init_inserts = atoi(argv[2]);
      count = atoi(argv[3]);
   } else if (argc != 1) {
      errx(1, "USAGE: %s seed initial_insertions iter_count\n", argv[0]);
   }

   init_inserts = max(init_inserts, count);

   srand(seed);

   rb_tree_init(&tree1);
   rb_tree_init(&tree2);
   items = calloc(init_inserts + count, sizeof(*items));

   for (int i = 0; i < init_inserts; i++) {
      items[i].in = true;
      add_item(&tree1, &tree2, &items[i]);
      rb_tree_validate(&tree1);
   }
   for (int i = init_inserts; i < (init_inserts + count); i++) {
      add_item(&tree2, &tree1, &items[i]);
      rb_tree_validate(&tree2);
   }

   /* Randomly swap items between the 2 trees. */
   for (int i = 0; i < count; i++) {
      int op = rand() % 2;

      switch (op) {
      case 0: {
         struct item *it = get_random_node(items, init_inserts + count, false);

         rb_tree_remove(&tree2, &it->node);
         rb_tree_insert(&tree1, &it->node, cmp_item);
         it->in = true;
         break;
      }
      case 1: {
         struct item *it = get_random_node(items, init_inserts + count, true);

         rb_tree_remove(&tree1, &it->node);
         rb_tree_insert(&tree2, &it->node, cmp_item);
         it->in = false;
         break;
      }
      }

      rb_tree_validate(&tree1);
      rb_tree_validate(&tree2);
   }

   for (int i = 0; i < (init_inserts + count); i++) {
      if (items[i].in)
         assert(rb_tree_search(&tree1, &items[i].value, cmp_item_val));
      else
         assert(rb_tree_search(&tree2, &items[i].value, cmp_item_val));
   }

   printf("ok\n");
   return 0;
}
