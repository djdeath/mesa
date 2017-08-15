import argparse
import math
import mmap
import pprint
import os
import struct

import gen
import gen9

class FileStream:
    _CHUNK_SIZE = pow(2, 20) # 1Mb

    class FileChunk:
        def __init__(self, offset, size, memory):
            self.offset = offset
            self.size = size
            self.memory = memory

        def deinit(self):
            self.memory.close()

    def __init__(self, file):
        self.file = file
        self.filesize = os.fstat(self.file.fileno()).st_size
        self.byte_offset = 0
        self.view = self.FileChunk(0, self._CHUNK_SIZE,
                                   mmap.mmap(fileno=self.file.fileno(),
                                             length=min(self._CHUNK_SIZE, self.filesize),
                                             access=mmap.ACCESS_READ,
                                             offset=0))

    def _view_at(self, offset):
        if offset >= self.view.offset and \
           offset < (self.view.offset + self.view.size):
            return self.view
        self.view.deinit()
        off = int(offset / self._CHUNK_SIZE) * self._CHUNK_SIZE
        self.view = self.FileChunk(off, self._CHUNK_SIZE,
                                   mmap.mmap(fileno=self.file.fileno(),
                                             length=min(self._CHUNK_SIZE, self.filesize - off),
                                             access=mmap.ACCESS_READ,
                                             offset=off))
        return self.view

    def read_absolute_dword(self, byte_offset = 0):
        view = self._view_at(byte_offset)
        return struct.unpack_from('I', view.memory, byte_offset - view.offset)[0]

    def read_dword(self, dword_offset = 0):
        return self.read_absolute_dword(self.byte_offset + dword_offset * 4)

    def has_dwords(self, dwords):
        return (self.byte_offset + dwords * 4) < self.filesize

    def write_to_memory(self, offset, size, memory, mem_offset):
        print("write_to_memory offset=%u size=%u mem_offset=%u dw0=%x" %
              (offset, size, mem_offset, self.read_absolute_dword(offset)))
        s = offset
        while s < (offset + size):
            v = self._view_at(s)
            v_offset = s - v.offset
            m_offset = mem_offset + (s - offset)
            m_size = min(v.size - v_offset, size - (s - offset))
            memory.write(m_offset, v.memory[v_offset:(v_offset + m_size)], m_size)
            s += m_size

# Split memory in chunks so we can have a large address space without actually
# using too much of the process' addressing space.
class Memory:
    _CHUNK_SIZE = pow(2, 20) # 1Mb

    class MemoryChunk:
        def __init__(self, offset, size):
            self.offset = offset
            self.size = size
            self.memory = mmap.mmap(-1, size)

    def __init__(self):
        self.chunks = []

    def _find_chunk(self, offset):
        for c in self.chunks:
            if c.offset <= offset and offset < (c.offset + c.size):
                return c
        c_offset = offset - (offset % self._CHUNK_SIZE)
        c = self.MemoryChunk(c_offset, self._CHUNK_SIZE)
        self.chunks.append(c)
        return c

    def write(self, offset, data, size):
        s = offset
        while s < (offset + size):
            c = self._find_chunk(s)
            c_offset = s - c.offset
            c_size = min(c.size, size - (s - offset))
            print("writing %u at %u | %u, %u - %u, %u" % (c_size, (s - c.offset),
                                                          (s - c.offset), c_size,
                                                          (s - offset), c_size))
            c.memory[c_offset:(c_offset + c_size)] = data[(s - offset):c_size]
            s += c_size

    def read_dword(self, dw_offset):
        byte_offset = dw_offset * 4
        c = self._find_chunk(byte_offset)
        return struct.unpack_from('I', c.memory, byte_offset - c.offset)[0]


#
def read_commands(f, memory, offset, size):
    print('initial view offset=%i/%i size=%i' % (f.byte_offset, offset, size))
    state = gen.DecodeState(memory, gen.View(f, offset, size))
    ret = gen9.cs.decode_instructions(state)
    pp = pprint.PrettyPrinter(indent=4)
    pp.pprint(ret)

#
def read_aub_block(f, memory):
    if not f.has_dwords(6):
        return False

    dw0 = f.read_dword(0)
    header_length = dw0 & 0x0000ffff

    dw1 = f.read_dword(1)
    operation     = dw1 & 0x000000ff
    block_type    = dw1 & 0x0000ff00
    address_space = dw1 & 0x00ff0000

    offset = f.read_dword(3)
    size   = f.read_dword(4)
    offset += f.read_dword(5) << 32 # gen8+

    if operation == 0x00000001: # data-write
        f.write_to_memory(f.byte_offset + 4 * (header_length + 2), size, memory, offset)
        print("===> data-write len=%i" % size)
    elif operation == 0x00000002: # command-write
        print("===> command-write len=%i" % header_length)
        read_commands(f, memory, 4 * (header_length + 2), size)

    return True

#
def read_aub_frame(f, memory):
    new_offset = f.byte_offset

    def aub_type(dw):
        return (dw >> 29) & 0x7

    def aub_opcode(dw):
        return (dw >> 23) & 0x3f

    def aub_subopcode(dw):
        return (dw >> 16) & 0x7f

    def make_header(type, opcode, subopcode):
        return type << 29 | opcode << 23 | subopcode << 16

    if not f.has_dwords(1):
        return False
    dw = f.read_dword()
    header_length = dw & 0xffff

    opcode = aub_opcode(dw)
    if opcode == 0x01:
        bias = 2
    elif opcode == 0x2e:
        bias = 1
    else:
        print('Unknown aub opcode %x' % opcode)
        return False

    not_eos = True
    new_offset += (header_length + bias) * 4

    t = dw & 0xffff0000
    if t == make_header(7, 1, 0x05): # header
        pass
    elif t == make_header(7, 1, 0x41): # block
        new_offset += f.read_dword(4)
        not_eos = read_aub_block(f, memory)
    elif t == make_header(7, 1, 0x1e): # bmp
        pass
    else:
        pass

    print("new_offset=%u -> %u" % (f.byte_offset, new_offset))
    f.byte_offset = new_offset
    return not_eos


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('file')
    args = parser.parse_args()

    f = FileStream(open(args.file, 'rb'))
    memory = Memory()
    while read_aub_frame(f, memory):
        pass


if __name__ == '__main__':
    main()
