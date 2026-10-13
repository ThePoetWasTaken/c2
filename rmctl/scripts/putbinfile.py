import os
import sys
import struct
import socket

MAGIC = 0x48A9B2F1E3C47058

s = socket.socket()

dst = sys.argv[3]
filename = sys.argv[2]

s.connect((sys.argv[1], 1337))

name_size = len(dst)
file_size = os.path.getsize(filename)

payload = struct.pack(f'>QQQ', MAGIC, 3, 16) + struct.pack('>QQ', name_size, file_size) + dst.encode('ascii')
s.send(payload)
with open(filename, 'rb') as f:
    s.sendfile(f, 0, file_size)

s.close()