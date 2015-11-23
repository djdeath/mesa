top_srcdir = ../../..
top_builddir = ../../..

#CC = emcc
CFLAGS = \
	-m32 \
	-D__linux__ \
	-D__GLIBC__ \
	-DHAVE_PTHREAD \
	-DHAVE_LIBDRM \
	-Ihstubs \
	\
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src/ \
	-I$(top_srcdir)/src/mapi \
	-I$(top_srcdir)/src/mesa/ \
	-I$(top_srcdir)/src/gallium/include \
	-I$(top_srcdir)/src/gallium/auxiliary \
	-I$(top_srcdir)/src/mesa/drivers/dri/common \
	-I$(top_srcdir)/src/mesa/drivers/dri/intel/server \
	-I$(top_srcdir)/src/gtest/include \
	-I$(top_srcdir)/src/glsl/nir \
	-I$(top_builddir)/src/glsl/nir \
	-I$(top_builddir)/src/mesa/drivers/dri/common \
	$(shell pkg-config libdrm --cflags)

I965_DIR = $(top_srcdir)/src/mesa/drivers/dri/i965

SOURCES = \
	debug.c \
	\
	$(I965_DIR)/brw_device_info.c \
	$(I965_DIR)/brw_device_info.h \
	$(I965_DIR)/brw_disasm.c \
	$(I965_DIR)/brw_eu.c \
	$(I965_DIR)/brw_eu_compact.c \
	$(I965_DIR)/brw_eu_emit.c \
	$(I965_DIR)/brw_eu.h \
	$(I965_DIR)/brw_packed_float.c \
	$(I965_DIR)/brw_reg.h \
	$(top_srcdir)/src/util/ralloc.c \
	$(top_srcdir)/src/util/ralloc.h


genasm.js: $(SOURCES)
	$(CC) $(CFLAGS) $(filter %.c,$(SOURCES)) -o $@

clean:
	rm -f genasm.js genasm.js.map
