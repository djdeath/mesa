class DecodeState:
    def __init__(self, memory, view):
        self.memory = memory
        self.views = [view]
        self.view = view
        self.value = 0

    def push_view(self, view):
        self.views.append(view)
        self.view = self.views[-1]

    def pop_view(self):
        self.views.pop()
        self.view = self.views[-1]

class View:
    def __init__(self, base, offset, size):
        self.base = base
        self.start_offset = offset
        self.offset = offset
        self.size = size

    def ended(self):
        return self.offset >= (self.start_offset + self.size)

    def advance(self, length):
        self.offset += length

    def read_dword(self, offset):
        return self.base.read_dword(self.offset + offset)

# Basic classes
class Container:
    def is_container(self):
        return True

class Struct(Container):
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

    def decode(self, state):
        ret = {}
        for f in self.fields:
            ret[f.name] = f.decode(state)
        return ret

class FixedGroup():
    def __init__(self, start, count, size):
        self.count = count
        self.start = start
        self.size = size
        self.fields = []
        self.default = None

    def add(self, field):
        self.fields.append(field)
        return self

    def decode(self, state):
        return {}

class VariableGroup():
    def __init__(self, start, size):
        self.start = start
        self.size = size
        self.fields = []
        self.default = None

    def add(self, field):
        self.fields.append(field)
        return self

    def decode(self, state):
        return {}

class Field:
    def __init__(self, name, start, end, gen_type, default=None):
        self.name = name
        self.start = start
        self.end = end
        self.gen_type = gen_type
        self.default = default

    def decode(self, state):
        if self.gen_type.is_container() and self.start >= 32:
            offset = self.start / 8
            state.push_view(View(state.view, offset, state.view.size - offset))
            ret = self.gen_type.decode(state)
            state.pop_view()
        else:
            dw = state.view.read_dword(self.start / 32)
            state.value = (dw >> self.start) & ((1 << (self.end - self.start + 1)) - 1)
            ret = self.gen_type.decode(state)
        return ret

# Basic types
class BaseType:
    def is_container(self):
        return False

    def decode(self, state):
        return { 'value': state.value }

class Enum(BaseType):
    def __init__(self, name, values):
        self.name = name
        self.values = values
        self.index = {}
        for k, v in self.values.iteritems():
            self.index[v] = k

    def value(self, state, value):
        if value in self.index:
            return { description: self.index[value], value: value }
        return { value: value }

class Boolean(BaseType):
    def __init__(self):
        pass

    def decode(self, state):
        return { 'description': 'True' if state.value != 0 else 'False',
                 'value': state.value }

class Integer(BaseType):
    def __init__(self, bits):
        self.bits = bits

class UInteger(BaseType):
    def __init__(self, bits):
        self.bits = bits

class Float(BaseType):
    def __init__(self, bits):
        self.bits = bits

    def decode(self, state):
        super(state) # TODO

class UFixed(BaseType):
    def __init__(self, num, fract):
        self.num = num
        self.fract = fract

    def decode(self, state):
        super(state) # TODO

class SFixed(BaseType):
    def __init__(self, num, fract):
        self.num = num
        self.fract = fract

    def decode(self, state):
        super(state) # TODO

class Offset:
    def __init__(self):
        pass

    def is_container(self):
        return True

    def decode(self, state):
        return {} # TODO

class Address:
    def __init__(self):
        pass

    def is_container(self):
        return True

    def decode(self, state):
        return {} # TODO

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

    def decode_instructions_until(self, state):
        pass

    def decode_instructions(self, state):
        ret = []
        while not state.view.ended():
            dw = state.view.read_dword(0)
            print("%x" % dw)
            inst = self.find_instruction(dw)
            print("%x" % dw)
            ret.append(inst.decode(state))
            state.view.advance(4)
            break
        return ret
