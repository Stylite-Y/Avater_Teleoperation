"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class BP4_NN_IO_t(object):
    __slots__ = ["input", "output"]

    __typenames__ = ["float", "float"]

    __dimensions__ = [[35], [12]]

    def __init__(self):
        self.input = [ 0.0 for dim0 in range(35) ]
        self.output = [ 0.0 for dim0 in range(12) ]

    def encode(self):
        buf = BytesIO()
        buf.write(BP4_NN_IO_t._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack('>35f', *self.input[:35]))
        buf.write(struct.pack('>12f', *self.output[:12]))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != BP4_NN_IO_t._get_packed_fingerprint():
            raise ValueError("Decode error")
        return BP4_NN_IO_t._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = BP4_NN_IO_t()
        self.input = struct.unpack('>35f', buf.read(140))
        self.output = struct.unpack('>12f', buf.read(48))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if BP4_NN_IO_t in parents: return 0
        tmphash = (0x2eb76ea00481d757) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff) + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if BP4_NN_IO_t._packed_fingerprint is None:
            BP4_NN_IO_t._packed_fingerprint = struct.pack(">Q", BP4_NN_IO_t._get_hash_recursive([]))
        return BP4_NN_IO_t._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

