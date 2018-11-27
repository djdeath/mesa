/*
 * Copyright © 2019 Intel Corporation
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

#ifndef __IGS_CLIENT_H__
#define __IGS_CLIENT_H__

#include <stdbool.h>
#include <stdint.h>

int igs_client_acquire_perf(int drm_fd,
                            int metrics_set_id,
                            int report_format,
                            int period_exponent,
                            uint32_t ctx_id,
                            uint64_t timeout_ms);

bool igs_client_can_register_configurations(int drm_fd);

uint32_t igs_client_register_configuration(int drm_fd,
                                           const char *uuid,
                                           uint32_t n_mux_regs,
                                           const uint32_t *mux_regs,
                                           uint32_t n_boolean_regs,
                                           const uint32_t *boolean_regs,
                                           uint32_t n_flex_regs,
                                           const uint32_t *flex_regs);

#endif /* __IGS_CLIENT_H__ */
