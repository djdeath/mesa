import argparse
import re
import sys
import xml.etree.cElementTree as et

class Codegen:

    _file = None
    _indent = 0
    _endl_request = False

    endl="\n"
    use_tabs = False
    use_newlines = False

    def __init__(self, f):
        self._file = f

    def __call__(self, string):
        if not self._file:
            return self
        if self._endl_request:
            self._file.write(''.rjust(self._indent))
            self._endl_request = False
        self._file.write(string)
        return self

    def endl(self):
        self._file.write('\n')
        self._endl_request = True
        return self

    #without indenting or new lines
    def frag(self, *args):
        code = ' '.join(map(str, args))
        self._file.write(code)

    def indent(self, n):
        self._indent = self._indent + n
    def outdent(self, n):
        self._indent = self._indent - n

#
#
#

def write_prolog(f, doc):
    f('import gen').endl().endl()
    genxml = doc.getroot()
    f("cs = gen.GenCS('%s')" % genxml.get('name')).endl()


def write_enums(f, doc):
    enums = doc.findall('.//enum')
    for enum in enums:
        f("cs.enum('%s'," % enum.get('name')).endl()
        f.indent(4)
        f('{').endl()
        f.indent(4)
        for value in enum.findall('.//value'):
            f("'%s': %i," % (value.get('name'), int(value.get('value')))).endl()
        f.outdent(4)
        f('})').endl()
        f.outdent(4)

_type_to_gen_type = {
    'float': "gen.Float(%i)",
    'int': "gen.Integer(%i)",
    'uint': "gen.UInteger(%i)",
    'bool': 'gen.Boolean()',
    'offset': 'gen.Offset()',
    'address': 'gen.Address()',
}

sfixed_reg = "s([0-9]+)\\.([0-9]+)"
ufixed_reg = "u([0-9]+)\\.([0-9]+)"

def type_to_gen_type(name, size):
    if name in _type_to_gen_type:
        if '%' in _type_to_gen_type[name]:
            return _type_to_gen_type[name] % size
        else:
            return _type_to_gen_type[name]
    l = re.search(sfixed_reg, name)
    if l and len(l.groups()) > 0:
        return 'gen.SFixed(%s, %s)' % (l.groups()[0], l.groups()[1])
    l = re.search(ufixed_reg, name)
    if l and len(l.groups()) > 0:
        return 'gen.UFixed(%s, %s)' % (l.groups()[0], l.groups()[1])
    return "cs.type('%s')" % name

def write_field(f, field):
    name = field.get('name')
    fields = {
        'start': field.get('start'),
        'end': field.get('end'),
        'gen_type': type_to_gen_type(field.get('type'),
                                     int(field.get('end')) - int(field.get('start')))
    }
    if 'default' in field.attrib:
        fields['default'] = field.get('default')
    f("gen.Field(name='%s'" % name)
    for k, v in fields.iteritems():
        f(", %s=%s" % (k, v))
    f(")")

def write_fields(f, fields):
    fields = sorted(fields, key=lambda e: int(e.get('start')))
    for field in fields[:-1]:
        write_field(f, field)
        f("\\").endl()
    write_field(f, fields[-1])

def write_group(f, group):
    count = int(group.get('count'))
    start = int(group.get('start'))
    size = int(group.get('size'))
    if count == 0:
        f("gen.VariableGroup(start=%i, size=%i) \\" % (start, size)).endl()
    else:
        f("gen.FixedGroup(start=%i, count=%i, size=%i) \\" % (start, count, size)).endl()
    fields = sorted(group.findall('field'), key=lambda e: int(e.get('start')))
    f.indent(4)
    if len(fields) > 0:
        for field in fields[:-1]:
            f('.add(')
            write_field(f, field)
            f(') \\').endl()
        f('.add(')
        write_field(f, fields[-1])
        f(')')
    f.outdent(4)

def write_structs(f, doc, match, func):
    structs = doc.findall(match)
    for struct in structs:
        name = struct.get('name')
        length = int(struct.get('length')) if 'length' in struct.attrib else 0
        bias = int(struct.get('bias')) if 'bias' in struct.attrib else 0
        f("cs.%s(name='%s', length=%i, bias=%i) \\" % (func, name, length, bias)).endl()
        fields = sorted(struct.findall('field'), key=lambda e: int(e.get('start')))
        groups = sorted(struct.findall('group'), key=lambda e: int(e.get('start')))
        f.indent(4)
        if len(fields) > 0:
            for field in fields[:-1]:
                f('.add(')
                write_field(f, field)
                f(') \\').endl()
            f('.add(')
            write_field(f, fields[-1])
            f(')')
            if len(groups) > 0:
                f(' \\').endl()
        if len(groups) > 0:
            for group in groups[:-1]:
                f('.add(')
                write_group(f, group)
                f(') \\').endl()
            f('.add(')
            write_group(f, groups[-1])
            f(')')
        f.outdent(4)
        f.endl()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--out', help='Where to write the files.')
    parser.add_argument('--xml', help='GEN XML file.', required=True)
    args = parser.parse_args()

    doc = et.parse(args.xml)

    def get_out_file(f):
        if f == None:
            return sys.stdout
        return open(filename, 'w')

    f = Codegen(get_out_file(args.out))
    write_prolog(f, doc)
    f.endl()
    write_enums(f, doc)
    f.endl()
    write_structs(f, doc, './/struct', 'struct')
    f.endl()
    write_structs(f, doc, './/instruction', 'instruction')
    f.endl()


if __name__ == '__main__':
    main()
