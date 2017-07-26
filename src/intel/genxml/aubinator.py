import argparse
import math
import mmap
import pprint
import os
import struct

import gen
import gen9

class FileStream:
    _CHUNK_SIZE = 100 * 1024 # 100Kb

    def __init__(self, file):
        self.file = file
        self.filesize = os.fstat(self.file.fileno()).st_size
        self.offset = 0
        self.view = { 'offset': 0,
                      'size': self._CHUNK_SIZE,
                      'memory': mmap.mmap(fileno=self.file.fileno(),
                                          length=min(self._CHUNK_SIZE, self.filesize),
                                          access=mmap.ACCESS_READ,
                                          offset=0) }

    def _view_at(self, offset):
        if offset >= self.view['offset'] and \
           offset < (self.view['offset'] + self.view['size']):
            return self.view
        self.view['memory'].close()
        off = int(offset / self._CHUNK_SIZE) * self._CHUNK_SIZE
        self.view = { 'offset': off,
                      'size': self._CHUNK_SIZE,
                      'memory': mmap.mmap(fileno=self.file.fileno(),
                                          length=min(self._CHUNK_SIZE, self.filesize - off),
                                          access=mmap.ACCESS_READ,
                                          offset=off) }
        return self.view

    def read_dword(self, dword_offset = 0):
        print('read dword file stream offset=%i dw_offset=%i' % (self.offset, dword_offset))
        offset = self.offset + dword_offset * 4
        view = self._view_at(offset)
        print('\toffset=%i' % view['offset'])
        print('\tlen=%i' % len(view['memory']))
        print('\taccess=%i' % (offset - view['offset']))
        return struct.unpack_from('I', view['memory'], offset - view['offset'])[0]

    def has_dwords(self, dwords):
        return (self.offset + dwords * 4) < self.filesize


# Split memory in chunks so we can have a large address space without actually
# using too much of the process' one.
class Memory:
    _CHUNK_SIZE = pow(10, 6) # 1Mb

    class MemoryChunk:
        def __init__(self, offset, size):
            self.offset = offset
            self.size = size
            self.memory = mmap.mmap(-1, size)

    def __init__(self):
        self.chunks = []

    def _find_chunk(self, offset):
        for c in self.chunks:
            if c.offset <= offset and (c.offset + c.size) < offset:
                return c
        offset -= offset % self._CHUNK_SIZE
        c = MemoryChunk(offset, self._CHUNK_SIZE)
        self.chunks.append(c)
        return c

    def write(self, offset, data, size):
        c = self._find_chunk(offset)
        limit = min(c.size - (offset - c.offset), size)
        c.memory[(offset - c.offset):limit] = data[0:limit]
        if limit < size:
            c = self._find_chunk(offset + limit)
            c.memory[0:(size - limit)] = data[limit:(size - limit)]

    def write_from_stream(self, mem_offset, stream_offset, size):
        pass

    def read_dword(self, dw_offset):
        byte_offset = dw_offset * 4
        c = self._find_chunk(byte_offset)
        return struct.unpack_from('I', c.memory, byte_offset - c.offset)


#
def read_commands(f, memory, offset, size):
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
        #memory.write(offset, data, size)
        print("===> data-write len=%i" % header_length)
    elif operation == 0x00000002: # command-write
        print("===> command-write len=%i" % header_length)
        read_commands(f, memory, header_length + 2, size)

    return True

#
def read_aub_frame(f, memory):
    new_offset = f.offset

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

    print("new_offset=%u -> %u" % (f.offset, new_offset))
    f.offset = new_offset
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
