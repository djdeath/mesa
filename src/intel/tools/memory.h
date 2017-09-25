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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _memory_t memory_t;

memory_t *memory_new(void);
memory_t *memory_ref(memory_t *parent_mem);
void memory_unref(memory_t *mem);

uint32_t memory_read_dword(memory_t *mem, uint64_t offset);

void memory_read(memory_t *mem, uint64_t offset, void *data, size_t length);
memory_t *memory_write(memory_t *mem, uint64_t offset, void *data, size_t length);
memory_t *memory_write_unref(memory_t *mem, uint64_t offset, void *data, size_t length);

uint64_t memory_start(memory_t *mem);
uint64_t memory_end(memory_t *mem);

size_t memory_size(void);

#ifdef __cplusplus
}
#endif

#endif /* __MEMORY_H__ */
