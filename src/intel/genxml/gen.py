import struct

class DecodeState:
    def __init__(self, memory, view):
        self.memory = memory
        self.views = [view]
        self.view = view
        self.value = 0
        self.state = {}

    def push_view(self, view):
        self.views.append(view)
        self.view = self.views[-1]

    def pop_view(self):
        self.views.pop()
        self.view = self.views[-1]

    def apply(self, name, fields):
        self.state[name] = {}
        for k, v in fields.iteritems():
            print(k)
            print(v)
            self.state[name][k] = v

class View:
    def __init__(self, base, offset, size):
        self.base = base
        self.start_offset = offset
        self.offset = offset
        self.size = size

    def ended(self):
        if self.size == 0:
            return False
        return self.offset >= (self.start_offset + self.size)

    def advance(self, dw_length):
        self.offset += 4 * dw_length

    def read_dword(self, dw_offset):
        return self.base.read_dword(self.offset / 4 + dw_offset)

# Basic classes
class Struct:
    def __init__(self, name, length=0, bias=0):
        self.name = name
        self.length = length
        self.bias = bias
        self.fields = []
        self.opcode = None

    def is_base(self):
        return False

    def is_container(self):
        return True

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
            retf = f.decode(state)
            if hasattr(f, 'name'):
                ret[f.name] = retf
            else:
                i = 0
                # todo...
                print("fuuuuuu")
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
    def __init__(self, name, start, gen_type, end=None, default=None):
        self.name = name
        self.start = start
        self.end = end if end else start
        self.gen_type = gen_type
        self.default = default

    def decode(self, state):
        if self.gen_type.is_base():
            # Generate a value field for base types
            value = 0
            if (self.start - self.end) <= 32:
                dw = state.view.read_dword(self.start / 32)
                value = (dw >> (self.start % 2)) & ((1L << (self.end - self.start + 1)) - 1)
            else:
                assert ((self.start - self.end) % 32) == 0
                value = 0
                for i in range(self.start - self.end):
                    dw = state.view.read_dword(self.start / 32)
                    value = value | (dw << (32 * i))
            state.value = value
            ret = self.gen_type.decode(state)
        elif self.gen_type.is_container() and self.start >= 32:
            # Just push a new view for container types, they read the view
            # themselves.
            offset = self.start / 8
            state.push_view(View(state.view, offset, state.view.size - offset))
            ret = self.gen_type.decode(state)
            state.pop_view()
        else:
            ret = self.gen_type.decode(state)
        return ret

# Basic types
class BaseType:
    def is_base(self):
        return True

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

    def decode(self, state):
        if state.value in self.index:
            return { 'pretty': self.index[state.value],
                     'value': state.value }
        return { 'value': state.value }

class Boolean(BaseType):
    def __init__(self):
        pass

    def decode(self, state):
        return { 'pretty': 'True' if state.value != 0 else 'False',
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
        s = struct.pack('>l', state.value)
        return { 'pretty': struct.unpack('>f', s)[0],
                 'value': state.value }

class UFixed(BaseType):
    def __init__(self, num, fract):
        self.num = num
        self.fract = fract

    def decode(self, state):
        num = state.value >> self.fract
        max = (1 << self.fract) - 1
        return { 'pretty': num + float(state.value) / max,
                 'value': state.value }

class SFixed(BaseType):
    def __init__(self, num, fract):
        self.num = num
        self.fract = fract

    def decode(self, state):
        num = state.value >> self.fract
        sign = -1 if (num & (1 << self.num)) != 0 else 1
        num = num & ((1 << num) - 1)
        max = (1 << self.fract) - 1
        return { 'pretty': sign * float(state.value & max) / max,
                 'value': state.value }
        super(state) # TODO

class OffsetFrom(BaseType):
    def __init__(self, offset=None, gen_type=None):
        self.offset = offset
        self.gen_type = gen_type

    def is_container(self):
        return True

    def decode(self, state):
        if not self.offset:
            return { 'value': state.value,
                     'pretty': '0x%x' % state.value }

        print(state.state['STATE_BASE_ADDRESS'])
        print(state.state['STATE_BASE_ADDRESS'][self.offset])
        addr = state.state['STATE_BASE_ADDRESS'][self.offset]['value'] + state.value
        state.push_view(View(state.memory, addr, 0))
        ret = { 'value': self.gen_type.decode(state),
                'pretty': '0x%x' % addr }
        state.pop_view()
        return ret

class Address(BaseType):
    def __init__(self, decode_func=None):
        self.decode_func = decode_func

    def is_container(self):
        return True

    def decode(self, state):
        if not self.decode_func:
            return { 'value': state.value,
                     'pretty': '0x%x' % state.value }

        addr = state.value
        state.push_view(View(state.memory, addr, 0))
        ret = { 'value': self.decode_func(state),
                'pretty': '0x%x' % addr }
        state.pop_view()
        return ret

class ArrayOf:
    def __init__(self, gen_type, length):
        self.gen_type = gen_type

    def decode(self, state):
        pass


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
        ret = []
        while True:
            dw = state.view.read_dword(0)
            inst = self.find_instruction(dw)
            decoded = inst.decode(state)
            state.apply(inst.name, decoded)
            ret.append({ 'name': inst.name, 'value': decoded })
            if 'DWord Length' in decoded:
                state.view.advance(decoded['DWord Length']['value'] + inst.bias)
            else:
                state.view.advance(1)
            if inst.name == 'MI_BATCH_BUFFER_END':
                break
        return ret

    def decode_instructions(self, state):
        ret = []
        while not state.view.ended():
            dw = state.view.read_dword(0)
            inst = self.find_instruction(dw)
            decoded = inst.decode(state)
            state.apply(inst.name, decoded)
            ret.append({ 'name': inst.name, 'value': decoded })
            state.view.advance(decoded['DWord Length']['value'] + inst.bias)
        return ret
