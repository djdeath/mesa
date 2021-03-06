

# Copyright © 2011 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

include Makefile.sources

AM_CFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/ \
	-I$(top_srcdir)/src/mapi \
	-I$(top_srcdir)/src/mesa/ \
	-I$(top_srcdir)/src/gallium/include \
	-I$(top_srcdir)/src/gallium/auxiliary \
	-I$(top_srcdir)/src/mesa/drivers/dri/common \
	-I$(top_srcdir)/src/mesa/drivers/dri/intel/server \
	-I$(top_srcdir)/src/gtest/include \
	-I$(top_builddir)/src/glsl/nir \
	-I$(top_builddir)/src/mesa/drivers/dri/common \
	$(DEFINES) \
	$(VISIBILITY_CFLAGS) \
	$(INTEL_CFLAGS)

AM_CXXFLAGS = $(AM_CFLAGS)

EXTRA_DIST = \
	brw_oa_hsw.xml \
	brw_oa_bdw.xml \
	brw_oa_chv.xml \
	brw_oa_skl.xml \
	brw_oa.py

noinst_LTLIBRARIES = libi965_dri.la
libi965_dri_la_SOURCES = $(i965_FILES)
libi965_dri_la_LIBADD = $(INTEL_LIBS)

TEST_LIBS = \
	libi965_dri.la \
	../common/libdricommon.la \
	../common/libmegadriver_stub.la \
        ../../../libmesa.la \
	$(DRI_LIB_DEPS) \
        $(CLOCK_LIB) \
	../common/libdri_test_stubs.la

TESTS = \
	test_fs_cmod_propagation \
	test_fs_saturate_propagation \
        test_eu_compact \
	test_vf_float_conversions \
        test_vec4_copy_propagation \
        test_vec4_register_coalesce

check_PROGRAMS = $(TESTS)

test_fs_cmod_propagation_SOURCES = \
	test_fs_cmod_propagation.cpp
test_fs_cmod_propagation_LDADD = \
	$(top_builddir)/src/gtest/libgtest.la \
	$(TEST_LIBS)

test_fs_saturate_propagation_SOURCES = \
	test_fs_saturate_propagation.cpp
test_fs_saturate_propagation_LDADD = \
	$(top_builddir)/src/gtest/libgtest.la \
	$(TEST_LIBS)

test_vf_float_conversions_SOURCES = \
	test_vf_float_conversions.cpp
test_vf_float_conversions_LDADD = \
	$(top_builddir)/src/gtest/libgtest.la \
	$(TEST_LIBS)

test_vec4_register_coalesce_SOURCES = \
	test_vec4_register_coalesce.cpp
test_vec4_register_coalesce_LDADD = \
        $(top_builddir)/src/gtest/libgtest.la \
        $(TEST_LIBS)

test_vec4_copy_propagation_SOURCES = \
	test_vec4_copy_propagation.cpp
test_vec4_copy_propagation_LDADD = \
        $(top_builddir)/src/gtest/libgtest.la \
        $(TEST_LIBS)

test_eu_compact_SOURCES = \
	test_eu_compact.c
nodist_EXTRA_test_eu_compact_SOURCES = dummy.cpp
test_eu_compact_LDADD = $(TEST_LIBS)


BUILT_SOURCES = \
	brw_oa_hsw.h \
	brw_oa_hsw.c \
	brw_oa_bdw.h \
	brw_oa_bdw.c \
	brw_oa_chv.h \
	brw_oa_chv.c \
	brw_oa_skl.h \
	brw_oa_skl.c

brw_oa_hsw.c: brw_oa_hsw.xml brw_oa.py
	$(PYTHON2) $(PYTHON_FLAGS) $(srcdir)/brw_oa.py --code=$(builddir)/brw_oa_hsw.c --include="brw_oa_hsw.h" $(srcdir)/brw_oa_hsw.xml

brw_oa_hsw.h brw_oa_hsw.c: brw_oa_hsw.xml brw_oa.py Makefile
	$(PYTHON2) $(PYTHON_FLAGS) $(srcdir)/brw_oa.py \
	   --header=$(builddir)/brw_oa_hsw.h \
	   --code=$(builddir)/brw_oa_hsw.c \
	   --chipset="hsw" \
	   $(srcdir)/brw_oa_hsw.xml

brw_oa_bdw.h brw_oa_bdw.c: brw_oa_bdw.xml brw_oa.py Makefile
	$(PYTHON2) $(PYTHON_FLAGS) $(srcdir)/brw_oa.py \
	   --header=$(builddir)/brw_oa_bdw.h \
	   --code=$(builddir)/brw_oa_bdw.c \
	   --chipset="bdw" \
	   $(srcdir)/brw_oa_bdw.xml

brw_oa_chv.h brw_oa_chv.c: brw_oa_chv.xml brw_oa.py Makefile
	$(PYTHON2) $(PYTHON_FLAGS) $(srcdir)/brw_oa.py \
	   --header=$(builddir)/brw_oa_chv.h \
	   --code=$(builddir)/brw_oa_chv.c \
	   --chipset="chv" \
	   $(srcdir)/brw_oa_chv.xml

brw_oa_skl.h brw_oa_skl.c: brw_oa_skl.xml brw_oa.py Makefile
	$(PYTHON2) $(PYTHON_FLAGS) $(srcdir)/brw_oa.py \
	   --header=$(builddir)/brw_oa_skl.h \
	   --code=$(builddir)/brw_oa_skl.c \
	   --chipset="skl" \
	   $(srcdir)/brw_oa_skl.xml

