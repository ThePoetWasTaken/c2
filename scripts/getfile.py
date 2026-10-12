import sys
import struct
import socket

MAGIC = 0x48A9B2F1E3C47058

s = socket.socket()

filename = sys.argv[2]
dst = sys.argv[3]
size = len(filename)
payload = struct.pack(f'>QQQ', MAGIC, 1, size) + filename.encode('ascii')

s.connect((sys.argv[1], 1337))

s.sendall(payload)


data = s.recv(4096)
f = open(dst, "wb")

while True:
    if not data:
        break

    f.write(data)

    if (len(data) < 4096):
        break
    data = s.recv(4096)

f.close()
s.close()
