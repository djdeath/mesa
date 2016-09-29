#!/usr/bin/env python2
#encoding=utf-8

from __future__ import (
    absolute_import, division, print_function, unicode_literals
)
import xml.parsers.expat
import re
import sys
import copy

license =  """/*
 * Copyright (C) 2016 Intel Corporation
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
"""

pack_header = """%(license)s

/* Instructions, enums and structures for %(platform)s.
 *
 * This file has been generated, do not hand edit.
 */

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#ifndef __gen_field_functions
#define __gen_field_functions

union __gen_value {
   float f;
   uint32_t dw;
};

static inline uint64_t
__gen_get_uint(uint32_t dw, uint32_t start, uint32_t end)
{
   return (dw << (31 - end)) >> ((31 - end) + start);
}

static inline int64_t
__gen_get_sint(uint32_t dw, uint32_t start, uint32_t end)
{
   int64_t v = __gen_get_uint(dw, start, end);

   return v;
}

static inline float
__gen_get_float(uint32_t dw)
{
   return ((union __gen_value) { .dw = (dw) }).f;
}

static inline float
__gen_get_ufixed(uint32_t dw, uint32_t start, uint32_t end, uint32_t fract_bits)
{
   const float base = __gen_get_uint(dw, start + 1, end - fract_bits);
   const float fract =
     (float) __gen_get_uint(dw, end - fract_bits, end) / (float) ((1 << fract_bits) - 1);

   return base + fract;
}

static inline float
__gen_get_sfixed(uint32_t dw, uint32_t start, uint32_t end, uint32_t fract_bits)
{
   const float v = __gen_get_ufixed(dw, start + 1, end, fract_bits);

   if (__gen_get_uint(dw, start, start))
      return -v;
   else
      return v;
}

#ifndef __gen_print_user_data
#error #define __gen_print_user_data before including this file
#endif

#endif

"""

def to_alphanum(name):
    substitutions = {
        ' ': '',
        '/': '',
        '[': '',
        ']': '',
        '(': '',
        ')': '',
        '-': '',
        ':': '',
        '.': '',
        ',': '',
        '=': '',
        '>': '',
        '#': '',
        'α': 'alpha',
        '&': '',
        '*': '',
        '"': '',
        '+': '',
        '\'': '',
    }

    for i, j in substitutions.items():
        name = name.replace(i, j)

    return name

def safe_name(name):
    name = to_alphanum(name)
    if not name[0].isalpha():
        name = '_' + name

    return name

def num_from_str(num_str):
    if num_str.lower().startswith('0x'):
        return int(num_str, base=16)
    else:
        assert(not num_str.startswith('0') and 'octals numbers not allowed')
        return int(num_str)

class Field(object):
    ufixed_pattern = re.compile(r"u(\d+)\.(\d+)")
    sfixed_pattern = re.compile(r"s(\d+)\.(\d+)")

    def __init__(self, parser, attrs):
        self.parser = parser
        if "name" in attrs:
            self.name = safe_name(attrs["name"])
        self.start = int(attrs["start"])
        self.end = int(attrs["end"])
        self.type = attrs["type"]

        if "prefix" in attrs:
            self.prefix = attrs["prefix"]
        else:
            self.prefix = None

        if "default" in attrs:
            self.default = int(attrs["default"])
        else:
            self.default = None

        ufixed_match = Field.ufixed_pattern.match(self.type)
        if ufixed_match:
            self.type = 'ufixed'
            self.fractional_size = int(ufixed_match.group(2))

        sfixed_match = Field.sfixed_pattern.match(self.type)
        if sfixed_match:
            self.type = 'sfixed'
            self.fractional_size = int(sfixed_match.group(2))

class Group(object):
    def __init__(self, parser, parent, start, count, size):
        self.parser = parser
        self.parent = parent
        self.start = start
        self.count = count
        self.size = size
        self.fields = []

    class DWord:
        def __init__(self):
            self.size = 32
            self.fields = []
            self.address = None

    def collect_dwords(self, dwords, start, dim):
        for field in self.fields:
            if type(field) is Group:
                if field.count == 1:
                    field.collect_dwords(dwords, start + field.start, dim)
                else:
                    for i in range(field.count):
                        field.collect_dwords(dwords,
                                             start + field.start + i * field.size,
                                             "%s[%d]" % (dim, i))
                continue

            index = (start + field.start) // 32
            if not index in dwords:
                dwords[index] = self.DWord()

            clone = copy.copy(field)
            clone.start = clone.start + start
            clone.end = clone.end + start
            clone.dim = dim
            dwords[index].fields.append(clone)

            if field.type == "address":
                # assert dwords[index].address == None
                dwords[index].address = field

            # Coalesce all the dwords covered by this field. The two cases we
            # handle are where multiple fields are in a 64 bit word (typically
            # and address and a few bits) or where a single struct field
            # completely covers multiple dwords.
            while index < (start + field.end) // 32:
                if index + 1 in dwords and not dwords[index] == dwords[index + 1]:
                    dwords[index].fields.extend(dwords[index + 1].fields)
                dwords[index].size = 64
                dwords[index + 1] = dwords[index]
                index = index + 1

    def emit_print_function(self, start):
        dwords = {}
        self.collect_dwords(dwords, 0, "")

        # Determine number of dwords in this group. If we have a size, use
        # that, since that'll account for MBZ dwords at the end of a group
        # (like dword 8 on BDW+ 3DSTATE_HS). Otherwise, use the largest dword
        # index we've seen plus one.
        if self.size > 0:
            length = self.size // 32
        else:
            length = max(dwords.keys()) + 1

        for index in range(length):
            # Handle MBZ dwords
            if not index in dwords:
                print("")
                print("   /* empty dw[%d] */" % index)
                continue

            # For 64 bit dwords, we aliased the two dword entries in the dword
            # dict it occupies. Now that we're emitting the pack function,
            # skip the duplicate entries.
            dw = dwords[index]
            if index > 0 and index - 1 in dwords and dw == dwords[index - 1]:
                continue

            print("")
            dword_start = index * 32
            if dw.address == None:
                address_count = 0
            else:
                address_count = 1

            # if dw.size == 32 and dw.address == None:
            #     v = None
            #     # print("   dw[%d] =" % index)
            # elif len(dw.fields) > address_count:
            #     v = "v%d" % index
            #     print("   const uint%d_t %s =" % (dw.size, v))
            # else:
            #     v = "0"

            has_values = False
            for field in dw.fields:
                if len(field.values) == 0:
                    continue

                has_values = True
                print("   const char *%s_values[] = {" % field.name)
                for value in field.values:
                    print("      [%d] = \"%s\"," % (value.value, value.name));
                print("   };")
            if has_values:
                print("")

            field_fmts = []
            field_args = []
            field_names = []
            field_index = 0
            for field in dw.fields:
                if field.type != "mbo":
                    name = field.name + field.dim

                if field.type == "address":
                    if dw.size == 32:
                        d = "%x"
                        s = "dw[%d]" % index
                    else:
                        d = "%lx"
                        s = "__gen_get_uint(dw[%d], %d, %d) | __gen_get_uint(dw[%d], %d, %d) << 32" % \
                            (index, field.start - dword_start, 31, \
                             index + 1, 0, field.end - (dword_start + 32))
                elif field.type == "uint":
                    d = "%lu"
                    s = "__gen_get_uint(dw[%d], %d, %d)" % \
                        (index, field.start - dword_start, field.end - dword_start)
                elif field.type == "int":
                    d = "%li"
                    s = "__gen_get_sint(dw[%d], %d, %d)" % \
                        (index, field.start - dword_start, field.end - dword_start)
                elif field.type == "bool":
                    d = "%s"
                    s = "__gen_get_uint(dw[%d], %d, %d) ? \"true\" : \"false\"" % \
                        (index, field.start - dword_start, field.end - dword_start)
                elif field.type == "float":
                    d = "%f"
                    s = "__gen_get_float(dw[%d])" % index
                elif field.type == "offset":
                    d = "%lu"
                    s = "__gen_get_uint(dw[%d], %d, %d)" % \
                        (index, field.start - dword_start, field.end - dword_start)
                elif field.type == 'ufixed':
                    d = "%f"
                    s = "__gen_get_ufixed(dw[%d], %d, %d, %d)" % \
                        (index, field.start - dword_start, field.end - dword_start, field.fractional_size)
                elif field.type == 'sfixed':
                    d = "%f"
                    s = "__gen_get_sfixed(dw[%d], %d, %d, %d)" % \
                        (index, field.start - dword_start, field.end - dword_start, field.fractional_size)
                elif field.type in self.parser.structs:
                    d = None
                    s = None
                    print("   data->indent += 1;")
                    if field.start == dword_start:
                        print("   %s_print(data, &dw[%d]);" % \
                              (self.parser.gen_prefix(safe_name(field.type)), index))
                    else:
                        value_name = "v%d_%d" % (index, field_index)
                        print("   const uint%d_t %s = __gen_get_uint(dw[%d], %d, %d);" % \
                              (dw.size, value_name, index, field.start - dword_start, field.end - dword_start))
                        print("   %s_print(data, &%s);" % \
                              (self.parser.gen_prefix(safe_name(field.type)), value_name))
                    print("   data->indent -= 1;")
                    print("")
                    field_index = field_index + 1
                else:
                    print("/* unhandled field %s, type %s */\n" % (name, field.type))
                    d = None
                    s = None

                if not s == None:
                    if len(field.values) > 0:
                        d = "%%s: %s (%%s)" % d
                        s = "%s, %s_values[(int) %s]" % (s, field.name, s)
                    else:
                        d = "%%s: %s" % d
                    field_fmts.append(d)
                    field_args.append(s)
                    field_names.append(name)

            if len(field_fmts) > 0:
                print("   __gen_print(")
                print("      data, &dw[%d]," % index)
                num_args = len(field_fmts)
                for i in range(num_args):
                    if i == num_args - 1:
                        print("      \"%%*s%s\\n\"," % field_fmts[i])
                    else:
                        print("      \"%%*s%s\\n\"" % field_fmts[i])
                for i in range(num_args):
                    if i == (num_args - 1):
                        print("      data->indent, data->indent_str, \"%s\", %s);" \
                              % (field_names[i], field_args[i]))
                    else:
                        print("      data->indent, data->indent_str, \"%s\", %s," \
                              % (field_names[i], field_args[i]))

class Value(object):
    def __init__(self, attrs):
        self.name = safe_name(attrs["name"])
        self.value = int(attrs["value"])

class Parser(object):
    def __init__(self):
        self.parser = xml.parsers.expat.ParserCreate()
        self.parser.StartElementHandler = self.start_element
        self.parser.EndElementHandler = self.end_element

        self.instruction = None
        self.structs = {}
        self.registers = {}

    def start_element(self, name, attrs):
        if name == "genxml":
            self.platform = attrs["name"]
            self.gen = attrs["gen"].replace('.', '')
            print(pack_header % {'license': license, 'platform': self.platform})
        elif name in ("instruction", "struct", "register"):
            if name == "instruction":
                self.instruction = safe_name(attrs["name"])
                self.length_bias = int(attrs["bias"])
            elif name == "struct":
                self.struct = safe_name(attrs["name"])
                self.structs[attrs["name"]] = 1
            elif name == "register":
                self.register = safe_name(attrs["name"])
                self.reg_num = num_from_str(attrs["num"])
                self.registers[attrs["name"]] = 1
            if "length" in attrs:
                self.length = int(attrs["length"])
                size = self.length * 32
            else:
                self.length = None
                size = 0
            self.group = Group(self, None, 0, 1, size)

        elif name == "group":
            group = Group(self, self.group,
                          int(attrs["start"]), int(attrs["count"]), int(attrs["size"]))
            self.group.fields.append(group)
            self.group = group
        elif name == "field":
            self.group.fields.append(Field(self, attrs))
            self.values = []
        elif name == "enum":
            self.values = []
            self.enum = safe_name(attrs["name"])
            if "prefix" in attrs:
                self.prefix = safe_name(attrs["prefix"])
            else:
                self.prefix= None
        elif name == "value":
            self.values.append(Value(attrs))

    def end_element(self, name):
        if name  == "instruction":
            self.emit_instruction()
            self.instruction = None
            self.group = None
        elif name == "struct":
            self.emit_struct()
            self.struct = None
            self.group = None
        elif name == "register":
            self.emit_register()
            self.register = None
            self.reg_num = None
            self.group = None
        elif name == "group":
            self.group = self.group.parent
        elif name  == "field":
            self.group.fields[-1].values = self.values

    def gen_prefix(self, name):
        if name[0] == "_":
            return 'GEN%s%s' % (self.gen, name)
        else:
            return 'GEN%s_%s' % (self.gen, name)

    def emit_print_function(self, name, group):
        name = self.gen_prefix(name)
        print("static inline void\n%s_print(__gen_print_user_data *data, const void * restrict src)\n{" %
              name)

        # Cast dst to make header C++ friendly
        print("   const uint32_t * restrict dw = (const uint32_t * restrict) src;")

        group.emit_print_function(0)

        print("}\n")

    def emit_instruction(self):
        self.emit_print_function(self.instruction, self.group)

    def emit_register(self):
        self.emit_print_function(self.register, self.group)

    def emit_struct(self):
        self.emit_print_function(self.struct, self.group)

    def parse(self, filename):
        file = open(filename, "rb")
        self.parser.ParseFile(file)
        file.close()

if len(sys.argv) < 2:
    print("No input xml file specified")
    sys.exit(1)

input_file = sys.argv[1]

p = Parser()
p.parse(input_file)
