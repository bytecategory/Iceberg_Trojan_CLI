from Crypto.Cipher import DES
import struct
import zlib
import binascii
import socket
import sys
def reverse_bytes(b):
    assert type(b) == bytes
    ba = bytearray(b)
    for i in range(0, len(b)):
        ba[i] = int(format(b[i], '0>8b')[::-1], 2)
    return bytes(ba)
def get_new_key(key):
    ba = bytearray(8)
    for i, b in enumerate(key):
        ba[i % 8] ^= b
    return bytes(ba)
def padding(d):
    ba = bytearray(d)
    while len(ba) % 8 != 0:
        ba.append(0)
    return bytes(ba)
def append_len(d):
    assert type(d) == bytes
    length = struct.pack('<L', len(d))
    return bytes(length + d)
def remove_len(d):
    assert type(d) == bytes
    return d[4:]
def e_des_encrypt(plain, key):
    des = DES.new(reverse_bytes(get_new_key(key)), DES.MODE_ECB)
    return des.encrypt(padding(append_len(plain)))
def e_des_decrypt(raw, key):
    des = DES.new(reverse_bytes(get_new_key(key)), DES.MODE_ECB)
    t = des.decrypt(raw)
    return remove_len(t)
plain = "MicrosoftWebゑㄤゑ0ゑㄤゑ0ゑㄤゑ0ゑㄤゑ0ゑㄤゑ0ゑㄤゑ0ゑㄤゑ0ゑㄤゑ0ゑㄤゑ0ゑㄤゑ0".encode('gbk')
key = b"Microsoft"
ciph = e_des_encrypt(plain, key)
data = binascii.unhexlify('0d0f3e0308010000')+zlib.compress(ciph)
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect((sys.argv[1],8080))
s.send(data)
while True:
    recevied=s.recv(4096)
    zlib_start = recevied.find(b'\x78\x9c')
    if zlib_start!=-1:
        zlib_data = recevied[zlib_start:]
        decompressed = zlib.decompress(zlib_data)
        plaintext_bytes = e_des_decrypt(decompressed, b"Microsoft")
        plaintext = plaintext_bytes.decode('gbk')
        print(plaintext)
    else:
        print(recevied)
