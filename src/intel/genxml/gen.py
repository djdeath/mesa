# Basic classes
class Enum:
    def __init__(self, name, values):
        self.name = name
        self.values = values

class Struct:
    def __init__(self, name, length=0, bias=0):
        self.name = name
        self.length = length
        self.bias = bias
        self.fields = []
        self.opcode = None

    def add(self, field):
        self.fields.append(field)
        return self

    def _build_opcode_and_mask(self):
        self.opcode = 0
        self.mask = 0
        for f in self.fields:
            if f.default is not None and f.name != 'DWord Length':
                self.opcode |= f.default << f.start
                self.mask |= ((1 << (f.end - f.start + 1)) - 1) << f.start

    def match(self, dword):
        if not self.opcode:
            self._build_opcode_and_mask()
        return (self.mask & dword) == self.opcode

class FixedGroup:
    def __init__(self, start, count, size):
        self.count = count
        self.start = start
        self.size = size
        self.fields = []
        self.default = None

    def add(self, field):
        self.fields.append(field)
        return self

class VariableGroup:
    def __init__(self, start, size):
        self.start = start
        self.size = size
        self.fields = []
        self.default = None

    def add(self, field):
        self.fields.append(field)
        return self

class Field:
    def __init__(self, name, start, end, gen_type, default=None):
        self.name = name
        self.start = start
        self.end = end
        self.gen_type = gen_type
        self.default = default

# Basic types
class Offset:
    def __init__(self):
        pass

class Address:
    def __init__(self):
        pass

class Boolean:
    def __init__(self):
        pass

class Integer:
    def __init__(self, bits):
        self.bits = bits

class UInteger:
    def __init__(self, bits):
        self.bits = bits

class Float:
    def __init__(self, bits):
        self.bits = bits

class UFixed:
    def __init__(self, num, fract):
        self.num = num
        self.fract = fract

class SFixed:
    def __init__(self, num, fract):
        self.num = num
        self.fract = fract

# Command streamer builder
class GenCS:
    def __init__(self, name):
        self.name = name
        self.instrs = {}
        self.instrs_by_op = []
        self.structs = {}
        self.enums = {}

    def enum(self, *args):
        enum = Enum(*args)
        # Deal with nameless enums
        if enum.name:
            self.enums[enum.name] = enum
        return enum

    def struct(self, **args):
        struct = Struct(**args)
        self.structs[struct.name] = struct
        return struct

    def instruction(self, **args):
        inst = Struct(**args)
        self.instrs[inst.name] = inst
        return inst

    def type(self, name):
        if name in self.enums:
            return self.enums[name]
        if name in self.structs:
            return self.structs[name]
        raise Exception("Unknown type '%s'" % name)

    def find_instruction(self, dword):
        for inst_name, inst in self.instrs.iteritems():
            if inst.match(dword):
                return inst
        return None
