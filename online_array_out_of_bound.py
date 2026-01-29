import socket
import zlib
import binascii
import sys
t=r"a1192.168.1.1|8.8.8.8|root||"
t=t.encode('utf-8')
t=zlib.compress(t)
f=binascii.unhexlify("0d0f3e034a000000")
sys.stdout.buffer.write(f+t)
sys.stdout.buffer.flush()
