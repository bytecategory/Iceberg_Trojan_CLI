import zlib
from Crypto.Cipher import DES
import sys
from pathlib import Path
g=Path(sys.argv[1])
r=g.read_bytes()[8:]
r=zlib.decompress(r)
s=b"iceberg_plu_cut_text_@"
p=r.split(s)[0][8:]
e=zlib.decompress(p)
c=DES.new(bytearray(8),DES.MODE_ECB)
l=c.decrypt(e)[4:]
g.with_suffix('.exe').write_bytes(l)
