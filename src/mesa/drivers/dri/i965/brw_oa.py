#!/usr/bin/env python2
#
# Copyright (c) 2015 Intel Corporation
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

import xml.etree.ElementTree as ET
import argparse
import parsley
import re
import sys

def print_err(*args):
    sys.stderr.write(' '.join(map(str,args)) + '\n')

c_file = None
_c_indent = 0

def c(*args):
    if c_file:
        code = ' '.join(map(str,args))
        for line in code.splitlines():
            text = ''.rjust(_c_indent) + line
            c_file.write(text.rstrip() + "\n")

# indented, but no trailing newline...
def c_line_start(code):
    if c_file:
        c_file.write(''.rjust(_c_indent) + code)
def c_raw(code):
    if c_file:
        c_file.write(code)

def c_indent(n):
    global _c_indent
    _c_indent = _c_indent + n
def c_outdent(n):
    global _c_indent
    _c_indent = _c_indent - n

header_file = None
_h_indent = 0

def h(*args):
    if header_file:
        code = ' '.join(map(str,args))
        for line in code.splitlines():
            text = ''.rjust(_h_indent) + line
            header_file.write(text.rstrip() + "\n")

def h_indent(n):
    global _c_indent
    _h_indent = _h_indent + n
def h_outdent(n):
    global _c_indent
    _h_indent = _h_indent - n


hw_vars = {}
hw_vars["$EuCoresTotalCount"] = "brw->perfquery.sys_vars.n_eus"
hw_vars["$EuSlicesTotalCount"] = "brw->perfquery.sys_vars.n_eu_slices"
hw_vars["$EuSubslicesTotalCount"] = "brw->perfquery.sys_vars.n_eu_sub_slices"
hw_vars["$EuThreadsCount"] = "brw->perfquery.sys_vars.eu_threads_count"
hw_vars["$SliceMask"] = "brw->perfquery.sys_vars.slice_mask"
hw_vars["$SubsliceMask"] = "brw->perfquery.sys_vars.subslice_mask"
hw_vars["$GpuTimestampFrequency"] = "brw->perfquery.sys_vars.timestamp_frequency"
hw_vars["$GpuMinFrequency"] = "brw->perfquery.sys_vars.gt_min_freq"
hw_vars["$GpuMaxFrequency"] = "brw->perfquery.sys_vars.gt_max_freq"

class EmitInstrContext(object):
    def __init__(self):
        self.idx = 0
        self.code = []

    def emit_expression(self, expr):
        i = self.idx
        self.idx += 1
        v = "tmp%i" % i
        self.code.append("double %s = %s;" % (v, expr))
        return v


class EmitExprContext(object):
    def __init__(self):
        pass

    def emit_expression(self, expr):
        return expr

class BinExpr(object):
    def __init__(self, op, left, right):
        self.op = op
        self.left = left
        self.right = right

    def get_variables(self):
        return self.left.get_variables() + self.right.get_variables()

    def emit(self, ctx):
        l = self.left.emit(ctx)
        r = self.right.emit(ctx)
        if len(self.op) > 2:
            return ctx.emit_expression("%s(%s, %s)" % (self.op, l, r))
        elif self.op == '/':
            return ctx.emit_expression("%s ? %s %s %s : 0" % (r, l, self.op, r))
        else:
            return ctx.emit_expression("%s %s %s" % (l, self.op, r))
        return v

class NumberConst(object):
    def __init__(self, value):
        self.value = value

    def get_variables(self):
        return []

    def emit(self, ctx):
        return ctx.emit_expression(self.value)

class NameRef(object):
    def __init__(self, name):
        self.name = name

    def get_variables(self):
        return [self.name]

    def emit(self, ctx):
        if self.name in hw_vars:
            return ctx.emit_expression(hw_vars[self.name])
        else:
            return ctx.emit_expression("%s(brw, query, accumulator)" % read_funcs[self.name[1:]])

class ReadExpr(object):
    def __init__(self, name, offset):
        self.name = name
        self.offset = offset

    def get_variables(self):
        return []

    def emit(self, ctx):
        lctx = EmitExprContext() # Avoid generating a new variable for the offset
        return ctx.emit_expression("accumulator[query->%s_offset + %s]" % (self.name.lower(),
                                                                           self.offset.emit(lctx)))

grammar = parsley.makeGrammar("""

expr        = andExpr:l ( ws '||' andExpr:r        -> BinExpr('||', l, r)
                        | ws                       -> l )
andExpr     = bitExpr:l ( ws '&&' bitExpr:r        -> BinExpr('&&', l, r)
                        | ws                       -> l )
bitExpr     = eqExpr:l  ( ws '&' eqExpr:r          -> BinExpr('&', l, r)
                        | ws                       -> l )
eqExpr      = relExpr:l ( ws '==' relExpr:r        -> BinExpr('==', l, r)
                        | ws '!=' relExpr:r        -> BinExpr('!=', l, r)
                        | ws                       -> l )
relExpr     = addExpr:l ( ws '>'  addExpr:r        -> BinExpr('>', l, r)
                        | ws '>=' addExpr:r        -> BinExpr('>=', l, r)
                        | ws '<' addExpr:r         -> BinExpr('<', l, r)
                        | ws '<=' addExpr:r        -> BinExpr('<=', l, r)
                        | ws                       -> l )
addExpr     = mulExpr:l ( ws '+' mulExpr:r         -> BinExpr('+', l, r)
                        | ws '-' mulExpr:r         -> BinExpr('-', l, r)
                        | ws                       -> l )
mulExpr     = primExpr:l ( ws '*' mulExpr:r        -> BinExpr('*', l, r)
                         | ws '/' mulExpr:r        -> BinExpr('/', l, r)
                         | ws                      -> l )
primExpr    = ws '(' expr:e ws ')'                 -> e
            | readExpr
            | funcExpr
            | ident:n                              -> NameRef(n)
            | number

readExpr    = ws 'read' ws '(' name:n ws ','
                               number:o  ')'       -> ReadExpr(n, o)

funcExpr    = ws ( 'max'
                 | 'min' ):o ws '(' expr:l ws ','
                                    expr:r ws ')'  -> BinExpr(o.upper(), l, r)

number      = ws ( < '0x' hexa_char+ >:n           -> NumberConst(n)
                 | < '0b' bin_char+ >:n            -> NumberConst(n)
                 | < number_char+ >:n              -> NumberConst(n) )
number_char = :c ?(re.match('[0-9]', c))           -> c
hexa_char   = :c ?(re.match('[A-F0-9]', c))        -> c
bin_char    = :c ?(re.match('[01]', c))            -> c

ident       = ws < '$' name_char+ >:s              -> s
name        = ws < name_char+ >:s                  -> s
name_char   = :c ?(re.match('[_a-zA-Z0-9]', c))    -> c

ws          = (:c ?(re.match('[\\n\\t ]', c)))*

""", globals())


def output_counter_read(set, counter, counter_vars):
    c("\n")
    c("/* " + set.get('name') + " :: " + counter.get('name') + " */")
    ret_type = counter.get('data_type')
    if ret_type == "uint64":
        ret_type = "uint64_t"

    c("static " + ret_type)
    read_sym = set.get('chipset').lower() + "__" + set.get('underscore_name') + "__" + counter.get('underscore_name') + "__read"
    c(read_sym + "(struct brw_context *brw,\n")
    c_indent(len(read_sym) + 1)
    c("const struct brw_perf_query_info *query,\n")
    c("uint64_t *accumulator)\n")
    c_outdent(len(read_sym) + 1)

    c("{")
    c_indent(3)

    equation = counter.get('equation')
    expr = grammar(equation).expr()
    emit_context = EmitInstrContext()
    retvar = expr.emit(emit_context)
    c("/* Equation: %s */" % equation)
    for l in emit_context.code:
        c(l)
    c("")
    c("return %s;" % retvar)

    c_outdent(3)
    c("}")

    return read_sym

def output_counter_max(set, counter, counter_vars):
    max_eq = counter.get('max_equation')

    if not max_eq:
        return "0; /* undefined */"

    try:
        val = float(max_eq)
        return max_eq + ";"
    except:
        pass

    # We can only report constant maximum values via INTEL_performance_query
    expr = grammar(max_eq).expr()
    for v in expr.get_variables():
        if v not in hw_vars:
            return "0; /* unsupported (varies over time) */"

    c("\n")
    c("/* " + set.get('name') + " :: " + counter.get('name') + " */")
    ret_type = counter.get('data_type')
    if ret_type == "uint64":
        ret_type = "uint64_t"

    c("static " + ret_type)
    max_sym = set.get('chipset').lower() + "__" + set.get('underscore_name') + "__" + counter.get('underscore_name') + "__max"
    c(max_sym + "(struct brw_context *brw)\n")

    c("{")
    c_indent(3)

    emit_context = EmitInstrContext()
    retvar = expr.emit(emit_context)
    c("/* Equation: %s */" % max_eq)
    for l in emit_context.code:
        c(l)
    c("")
    c("return %s;" % retvar)

    c_outdent(3)
    c("}")

    return max_sym + "(brw);"

c_type_sizes = { "uint32_t": 4, "uint64_t": 8, "float": 4, "double": 8, "bool": 4 }
def sizeof(c_type):
    return c_type_sizes[c_type]

def pot_align(base, pot_alignment):
    return (base + pot_alignment - 1) & ~(pot_alignment - 1);

semantic_type_map = {
    "duration": "raw",
    "ratio": "event"
    }

def output_counter_report(set, counter, current_offset):
    data_type = counter.get('data_type')
    data_type_uc = data_type.upper()
    c_type = data_type

    if "uint" in c_type:
        c_type = c_type + "_t"

    semantic_type = counter.get('semantic_type')
    if semantic_type in semantic_type_map:
        semantic_type = semantic_type_map[semantic_type]

    semantic_type_uc = semantic_type.upper()

    c("\n")

    availability = counter.get('availability')
    if availability:
        e = grammar(availability).expr()
        expression = e.emit(EmitExprContext())

        lines = expression.split(' && ')
        n_lines = len(lines)
        if n_lines == 1:
            c("if (" + lines[0] + ") {")
        else:
            c("if (" + lines[0] + " &&")
            c_indent(4)
            for i in range(1, (n_lines - 1)):
                c(lines[i] + " &&")
            c(lines[(n_lines - 1)] + ") {")
            c_outdent(4)
        c_indent(3)

    c("counter = &query->counters[query->n_counters++];\n")
    c("counter->oa_counter_read_" + data_type + " = " + read_funcs[counter.get('symbol_name')] + ";\n")
    c("counter->name = \"" + counter.get('name') + "\";\n")
    c("counter->desc = \"" + counter.get('description') + "\";\n")
    c("counter->type = GL_PERFQUERY_COUNTER_" + semantic_type_uc + "_INTEL;\n")
    c("counter->data_type = GL_PERFQUERY_COUNTER_DATA_" + data_type_uc + "_INTEL;\n")
    c("counter->raw_max = " + max_values[counter.get('symbol_name')] + "\n")

    current_offset = pot_align(current_offset, sizeof(c_type))
    c("counter->offset = " + str(current_offset) + ";\n")
    c("counter->size = sizeof(" + c_type + ");\n")

    if availability:
        c_outdent(3);
        c("}")

    return current_offset + sizeof(c_type)

parser = argparse.ArgumentParser()
parser.add_argument("xml", help="XML description of metrics")
parser.add_argument("--header", help="Header file to write")
parser.add_argument("--code", help="C file to write")
parser.add_argument("--chipset", help="Chipset to generate code for")

args = parser.parse_args()

chipset = args.chipset.lower()

if args.header:
    header_file = open(args.header, 'w')

if args.code:
    c_file = open(args.code, 'w')

tree = ET.parse(args.xml)


copyright = """/* Autogenerated file, DO NOT EDIT manually!
 *
 * Copyright (c) 2015 Intel Corporation
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

"""

h(copyright)
h("""#pragma once

struct brw_context;

""")

c(copyright)
c(
"""
#include <stdint.h>
#include <stdbool.h>

#include "util/hash_table.h"

""")

c("#include \"brw_oa_" + chipset + ".h\"")

c(
"""
#include "brw_context.h"
#include "brw_performance_query.h"


#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))

""")

for set in tree.findall(".//set"):
    max_values = {}
    read_funcs = {}
    counter_vars = {}
    counters = set.findall("counter")

    assert set.get('chipset').lower() == chipset

    for counter in counters:
        empty_vars = {}
        read_funcs[counter.get('symbol_name')] = output_counter_read(set, counter, counter_vars)
        max_values[counter.get('symbol_name')] = output_counter_max(set, counter, empty_vars)
        counter_vars["$" + counter.get('symbol_name')] = counter


    c("\nstatic struct brw_perf_query_counter " + chipset + "_" + set.get('underscore_name') + "_query_counters[" + str(len(counters)) + "];\n")
    c("static struct brw_perf_query_info " + chipset + "_" + set.get('underscore_name') + "_query = {\n")
    c_indent(3)

    c(".kind = OA_COUNTERS,\n")
    c(".name = \"" + set.get('name') + "\",\n")
    c(".guid = \"" + set.get('hw_config_guid') + "\",\n")

    c(".counters = " + chipset + "_" + set.get('underscore_name') + "_query_counters,")
    c(".n_counters = 0,")
    c(".oa_metrics_set_id = 0, /* determined at runtime, via sysfs */")

    if chipset == "hsw":
        c(""".oa_format = I915_OA_FORMAT_A45_B8_C8,

/* Accumulation buffer offsets... */
.gpu_time_offset = 0,
.a_offset = 1,
.b_offset = 46,
.c_offset = 54,
""")
    else:
        c(""".oa_format = I915_OA_FORMAT_A32u40_A4u32_B8_C8,

/* Accumulation buffer offsets... */
.gpu_time_offset = 0,
.gpu_clock_offset = 1,
.a_offset = 2,
.b_offset = 38,
.c_offset = 46,
""")

    c_outdent(3)
    c("};\n")

    c("\nstatic void\n")
    c("register_" + set.get('underscore_name') + "_counter_query(struct brw_context *brw)\n")
    c("{\n")
    c_indent(3)

    c("static struct brw_perf_query_info *query = &" + chipset + "_" + set.get('underscore_name') + "_query;\n")
    c("struct brw_perf_query_counter *counter;\n")

    c("\n")
    c("/* Note: we're assuming there can't be any variation in the definition ")
    c(" * of a query between contexts so it's ok to describe a query within a ")
    c(" * global variable which only needs to be initialized once... */")
    c("\nif (!query->data_size) {")
    c_indent(3)

    offset = 0
    for counter in counters:
        offset = output_counter_report(set, counter, offset)


    c("\nquery->data_size = counter->offset + counter->size;\n")

    c_outdent(3)
    c("}");

    c("\n_mesa_hash_table_insert(brw->perfquery.oa_metrics_table, query->guid, query);")

    c_outdent(3)
    c("}\n")

h("void brw_oa_register_queries_" + chipset + "(struct brw_context *brw);\n")

c("\nvoid")
c("brw_oa_register_queries_" + chipset + "(struct brw_context *brw)")
c("{")
c_indent(3)

for set in tree.findall(".//set"):
    c("register_" + set.get('underscore_name') + "_counter_query(brw);")

c_outdent(3)
c("}")
