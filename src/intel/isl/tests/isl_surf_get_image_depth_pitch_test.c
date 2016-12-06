/*
 * Copyright 2016 Intel Corporation
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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/gen_device_info.h"
#include "isl/isl.h"
#include "isl/isl_priv.h"

#define BDW_GT2_DEVID 0x161a

// An asssert that works regardless of NDEBUG.
#define t_assert(cond) \
   do { \
      if (!(cond)) { \
         fprintf(stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
         abort(); \
      } \
   } while (0)

static void
test_bdw_2d_r8g8b8a8_unorm_42x43_levels09_tiley0(void)
{
   bool ok;

   struct gen_device_info devinfo;
   t_assert(gen_get_device_info(BDW_GT2_DEVID, &devinfo));

   struct isl_device dev;
   isl_device_init(&dev, &devinfo, /*bit6_swizzle*/ false);

   struct isl_surf surf;
   ok = isl_surf_init(&dev, &surf,
                      .dim = ISL_SURF_DIM_2D,
                      .format = ISL_FORMAT_R8G8B8A8_UNORM,
                      .width = 42,
                      .height = 43,
                      .depth = 1,
                      .levels = 9,
                      .array_len = 1,
                      .samples = 1,
                      .usage = ISL_SURF_USAGE_TEXTURE_BIT |
                               ISL_SURF_USAGE_DISABLE_AUX_BIT,
                      .tiling_flags = ISL_TILING_Y0_BIT);
   t_assert(ok);

   uint32_t depth_pitch =
      isl_surf_get_depth_pitch(&dev, &surf);

   t_assert(depth_pitch == isl_surf_get_array_pitch(&surf));
}

static void
test_bdw_3d_r8g8b8a8_unorm_256x256x256_levels09_tiley0(void)
{
   bool ok;

   struct gen_device_info devinfo;
   t_assert(gen_get_device_info(BDW_GT2_DEVID, &devinfo));

   struct isl_device dev;
   isl_device_init(&dev, &devinfo, /*bit6_swizzle*/ false);

   struct isl_surf surf;
   ok = isl_surf_init(&dev, &surf,
                      .dim = ISL_SURF_DIM_3D,
                      .format = ISL_FORMAT_R8G8B8A8_UNORM,
                      .width = 256,
                      .height = 256,
                      .depth = 256,
                      .levels = 9,
                      .array_len = 1,
                      .samples = 1,
                      .usage = ISL_SURF_USAGE_TEXTURE_BIT |
                               ISL_SURF_USAGE_DISABLE_AUX_BIT,
                      .tiling_flags = ISL_TILING_Y0_BIT);
   t_assert(ok);

   uint32_t depth_pitch =
      isl_surf_get_depth_pitch(&dev, &surf);

   t_assert(depth_pitch >= 256 * 256 * 4);
   t_assert(depth_pitch < isl_surf_get_array_pitch(&surf));
   t_assert(depth_pitch % 4096 == 0);
}

static void
test_bdw_3d_r8g8b8a8_unorm_256x256x256_levels09_linear(void)
{
   bool ok;

   struct gen_device_info devinfo;
   t_assert(gen_get_device_info(BDW_GT2_DEVID, &devinfo));

   struct isl_device dev;
   isl_device_init(&dev, &devinfo, /*bit6_swizzle*/ false);

   struct isl_surf surf;
   ok = isl_surf_init(&dev, &surf,
                      .dim = ISL_SURF_DIM_3D,
                      .format = ISL_FORMAT_R8G8B8A8_UNORM,
                      .width = 256,
                      .height = 256,
                      .depth = 256,
                      .levels = 1,
                      .array_len = 1,
                      .samples = 1,
                      .usage = ISL_SURF_USAGE_TEXTURE_BIT |
                               ISL_SURF_USAGE_DISABLE_AUX_BIT,
                      .tiling_flags = ISL_TILING_LINEAR_BIT);
   t_assert(ok);

   uint32_t depth_pitch =
      isl_surf_get_depth_pitch(&dev, &surf);

   t_assert(depth_pitch >= 256 * 256 * 4);
   t_assert(depth_pitch < isl_surf_get_array_pitch(&surf));
}

static void
test_bdw_3d_r8g8b8_unorm_256x256x256_levels09_linear(void)
{
   bool ok;

   struct gen_device_info devinfo;
   t_assert(gen_get_device_info(BDW_GT2_DEVID, &devinfo));

   struct isl_device dev;
   isl_device_init(&dev, &devinfo, /*bit6_swizzle*/ false);

   struct isl_surf surf;
   ok = isl_surf_init(&dev, &surf,
                      .dim = ISL_SURF_DIM_3D,
                      .format = ISL_FORMAT_R8G8B8_UNORM,
                      .width = 256,
                      .height = 256,
                      .depth = 256,
                      .levels = 1,
                      .array_len = 1,
                      .samples = 1,
                      .usage = ISL_SURF_USAGE_TEXTURE_BIT |
                               ISL_SURF_USAGE_DISABLE_AUX_BIT,
                      .tiling_flags = ISL_TILING_LINEAR_BIT);
   t_assert(ok);

   uint32_t depth_pitch =
      isl_surf_get_depth_pitch(&dev, &surf);

   t_assert(depth_pitch >= 256 * 256 * 3);
   t_assert(depth_pitch < isl_surf_get_array_pitch(&surf));
}

static void
test_bdw_2d_r8g8b8a8_unorm_256x256x256_levels09_linear(void)
{
   bool ok;

   struct gen_device_info devinfo;
   t_assert(gen_get_device_info(BDW_GT2_DEVID, &devinfo));

   struct isl_device dev;
   isl_device_init(&dev, &devinfo, /*bit6_swizzle*/ false);

   struct isl_surf surf;
   ok = isl_surf_init(&dev, &surf,
                      .dim = ISL_SURF_DIM_2D,
                      .format = ISL_FORMAT_R8G8B8A8_UNORM,
                      .width = 256,
                      .height = 256,
                      .depth = 1,
                      .levels = 1,
                      .array_len = 256,
                      .samples = 1,
                      .usage = ISL_SURF_USAGE_TEXTURE_BIT |
                               ISL_SURF_USAGE_DISABLE_AUX_BIT,
                      .tiling_flags = ISL_TILING_LINEAR_BIT);
   t_assert(ok);

   uint32_t depth_pitch =
      isl_surf_get_depth_pitch(&dev, &surf);

   t_assert(depth_pitch == isl_surf_get_array_pitch(&surf));
}

static void
test_bdw_1d_r8g8b8a8_unorm_256_levels09_linear(void)
{
   bool ok;

   struct gen_device_info devinfo;
   t_assert(gen_get_device_info(BDW_GT2_DEVID, &devinfo));

   struct isl_device dev;
   isl_device_init(&dev, &devinfo, /*bit6_swizzle*/ false);

   struct isl_surf surf;
   ok = isl_surf_init(&dev, &surf,
                      .dim = ISL_SURF_DIM_1D,
                      .format = ISL_FORMAT_R8G8B8A8_UNORM,
                      .width = 256,
                      .height = 1,
                      .depth = 1,
                      .levels = 1,
                      .array_len = 1,
                      .samples = 1,
                      .usage = ISL_SURF_USAGE_TEXTURE_BIT |
                               ISL_SURF_USAGE_DISABLE_AUX_BIT,
                      .tiling_flags = ISL_TILING_LINEAR_BIT);
   t_assert(ok);

   uint32_t depth_pitch =
      isl_surf_get_depth_pitch(&dev, &surf);

   t_assert(depth_pitch == isl_surf_get_array_pitch(&surf));
}

int main(void)
{
   test_bdw_2d_r8g8b8a8_unorm_42x43_levels09_tiley0();
   test_bdw_3d_r8g8b8a8_unorm_256x256x256_levels09_tiley0();
   test_bdw_3d_r8g8b8a8_unorm_256x256x256_levels09_linear();
   test_bdw_3d_r8g8b8_unorm_256x256x256_levels09_linear();
   test_bdw_2d_r8g8b8a8_unorm_256x256x256_levels09_linear();
   test_bdw_1d_r8g8b8a8_unorm_256_levels09_linear();
}
