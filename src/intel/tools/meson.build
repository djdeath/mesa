# Copyright © 2017 Intel Corporation

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

aubinator = executable(
  'aubinator',
  files('aubinator.c', 'disasm.c', 'gen_disasm.h', 'intel_aub.h'),
  dependencies : [dep_expat, dep_zlib, dep_dl, dep_thread, dep_m],
  include_directories : [inc_common, inc_intel],
  link_with : [libintel_common, libintel_compiler, libmesa_util],
  c_args : [c_vis_args, no_override_init_args],
  build_by_default : false,
)

if with_intel_tools
  inc_imgui = include_directories('imgui')
  aubinator_ui = executable(
    'aubinator_ui',
    files('aubinator_ui.cpp',
          'aubinator_imgui_widgets.cpp',
          'imgui/imgui.cpp',
          'imgui/imgui_demo.cpp',
          'imgui/imgui_draw.cpp',
          'imgui/imgui_impl_gtk3_cogl.cpp',
          'memory.c',
          'disasm.c',),
    dependencies : [dep_aubinator_ui, dep_expat, dep_zlib, dep_dl, dep_thread, dep_m],
    include_directories : [inc_common, inc_intel, inc_imgui],
    link_with : [libisl, libintel_common, libintel_compiler, libmesa_util],
    c_args : [c_vis_args, no_override_init_args],
    cpp_args : cpp_vis_args,
    build_by_default : false,
  )
endif

aubinator_error_decode = executable(
  'aubinator_error_decode',
  files('aubinator_error_decode.c', 'disasm.c', 'gen_disasm.h'),
  dependencies : [dep_zlib, dep_thread],
  include_directories : [inc_common, inc_intel],
  link_with : [libintel_common, libintel_compiler, libmesa_util],
  c_args : [c_vis_args, no_override_init_args],
  build_by_default : false,
)

memory_tests = executable(
  'memory_tests',
  files('memory.c'),
  include_directories : inc_common,
  link_with : libmesa_util,
  c_args : [c_vis_args, no_override_init_args, '-DBUILD_TESTS'],
  build_by_default : false,
)
