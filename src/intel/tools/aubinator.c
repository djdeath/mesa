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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "util/macros.h"

#include "aubinator_batch_decoder.h"

static void
setup_pager(void)
{
   int fds[2];
   pid_t pid;

   if (!isatty(1))
      return;

   if (pipe(fds) == -1)
      return;

   pid = fork();
   if (pid == -1)
      return;

   if (pid == 0) {
      close(fds[1]);
      dup2(fds[0], 0);
      execlp("less", "less", "-rFi", NULL);
   }

   close(fds[0]);
   dup2(fds[1], 1);
   close(fds[1]);
}

static void
print_help(const char *progname, FILE *file)
{
   fprintf(file,
           "Usage: %s [OPTION]... FILE\n"
           "Decode aub file contents.\n\n"
           "A valid --gen option must be provided.\n\n"
           "      --help          display this help and exit\n"
           "      --gen=platform  decode for given platform (ivb, byt, hsw, bdw, chv, skl, kbl or bxt)\n"
           "      --headers       decode only command headers\n"
           "      --color[=WHEN]  colorize the output; WHEN can be 'auto' (default\n"
           "                        if omitted), 'always', or 'never'\n"
           "      --no-pager      don't launch pager\n"
           "      --no-offsets    don't print instruction offsets\n",
           progname);
}

static bool
is_prefix(const char *arg, const char *prefix, const char **value)
{
   int l = strlen(prefix);

   if (strncmp(arg, prefix, l) == 0 && (arg[l] == '\0' || arg[l] == '=')) {
      if (arg[l] == '=')
         *value = arg + l + 1;
      else
         *value = NULL;

      return true;
   }

   return false;
}

struct aub_file {
   char *filename;
   int fd;
   struct stat sb;
   uint32_t *map, *end, *cursor;
};

static struct aub_file *
aub_file_open(const char *filename)
{
   struct aub_file *file;

   file = malloc(sizeof *file);
   file->filename = strdup(filename);
   file->fd = open(file->filename, O_RDONLY);
   if (file->fd == -1) {
      fprintf(stderr, "open %s failed: %s", file->filename, strerror(errno));
      exit(EXIT_FAILURE);
   }

   if (fstat(file->fd, &file->sb) == -1) {
      fprintf(stderr, "stat failed: %s", strerror(errno));
      exit(EXIT_FAILURE);
   }

   file->map = mmap(NULL, file->sb.st_size,
                    PROT_READ, MAP_SHARED, file->fd, 0);
   if (file->map == MAP_FAILED) {
      fprintf(stderr, "mmap failed: %s", strerror(errno));
      exit(EXIT_FAILURE);
   }

   file->cursor = file->map;
   file->end = file->map + file->sb.st_size / 4;

   return file;
}

int main(int argc, char *argv[])
{
   struct aub_file *file;
   int i;
   bool pager = true;
   const char *value, *input_file = NULL;
   char gen_val[24] = { 0, };
   const struct {
      const char *name;
      int pci_id;
   } gens[] = {
      { "ivb", 0x0166 }, /* Intel(R) Ivybridge Mobile GT2 */
      { "hsw", 0x0416 }, /* Intel(R) Haswell Mobile GT2 */
      { "byt", 0x0155 }, /* Intel(R) Bay Trail */
      { "bdw", 0x1616 }, /* Intel(R) HD Graphics 5500 (Broadwell GT2) */
      { "chv", 0x22B3 }, /* Intel(R) HD Graphics (Cherryview) */
      { "skl", 0x1912 }, /* Intel(R) HD Graphics 530 (Skylake GT2) */
      { "kbl", 0x591D }, /* Intel(R) Kabylake GT2 */
      { "bxt", 0x0A84 }  /* Intel(R) HD Graphics (Broxton) */
   }, *gen = NULL;
   struct gen_device_info devinfo;
   struct aub_batch_decoder batch_decoder;

   if (argc == 1) {
      print_help(argv[0], stderr);
      exit(EXIT_FAILURE);
   }

   for (i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "--no-pager") == 0) {
         pager = false;
      } else if (strcmp(argv[i], "--no-offsets") == 0) {
         batch_decoder.print_offsets = false;
      } else if (is_prefix(argv[i], "--gen", &value)) {
         if (value == NULL) {
            fprintf(stderr, "option '--gen' requires an argument\n");
            exit(EXIT_FAILURE);
         }
         snprintf(gen_val, sizeof(gen_val), "%s", value);
      } else if (strcmp(argv[i], "--headers") == 0) {
         batch_decoder.full_decode = false;
      } else if (is_prefix(argv[i], "--color", &value)) {
         if (value == NULL || strcmp(value, "always") == 0)
            batch_decoder.print_color = AUB_PRINT_COLOR_ALWAYS;
         else if (strcmp(value, "never") == 0)
            batch_decoder.print_color = AUB_PRINT_COLOR_NEVER;
         else if (strcmp(value, "auto") == 0)
            batch_decoder.print_color = AUB_PRINT_COLOR_AUTO;
         else {
            fprintf(stderr, "invalid value for --color: %s", value);
            exit(EXIT_FAILURE);
         }
      } else if (strcmp(argv[i], "--help") == 0) {
         print_help(argv[0], stdout);
         exit(EXIT_SUCCESS);
      } else {
         if (argv[i][0] == '-') {
            fprintf(stderr, "unknown option %s\n", argv[i]);
            exit(EXIT_FAILURE);
         }
         input_file = argv[i];
         break;
      }
   }

   for (i = 0; i < ARRAY_SIZE(gens); i++) {
      if (!strcmp(gen_val, gens[i].name)) {
         gen = &gens[i];
         break;
      }
   }

   if (gen == NULL) {
      fprintf(stderr, "can't find gen '%s', argument --gen is required\n"
                      "valid arguments: "
                      "ivb, byt, hsw, bdw, chv, skl, kbl or bxt\n", gen_val);
      exit(EXIT_FAILURE);
   }

   if (!gen_get_device_info(gen->pci_id, &devinfo)) {
      fprintf(stderr, "can't find device information: pci_id=0x%x name=%s\n",
              gen->pci_id, gen->name);
      exit(EXIT_FAILURE);
   }


   /* Do this before we redirect stdout to pager. */
   if (batch_decoder.print_color == AUB_PRINT_COLOR_AUTO) {
      batch_decoder.print_color =
         isatty(1) ? AUB_PRINT_COLOR_ALWAYS : AUB_PRINT_COLOR_NEVER;
   }

   if (isatty(1) && pager)
      setup_pager();

   if (input_file == NULL) {
       print_help(argv[0], stderr);
       exit(EXIT_FAILURE);
   } else {
       file = aub_file_open(input_file);
   }

   if (!aub_batch_decoder_init(&batch_decoder, &devinfo)) {
      fprintf(stderr, "unable to initialize batch decoder\n");
      exit(EXIT_FAILURE);
   }

   aub_batch_decoder_decode_aub(&batch_decoder,
                                file->map, file->sb.st_size, stdout);
   aub_batch_decoder_fini(&batch_decoder);

   fflush(stdout);
   /* close the stdout which is opened to write the output */
   close(1);

   wait(NULL);

   return EXIT_SUCCESS;
}
