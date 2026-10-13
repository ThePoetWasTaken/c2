import socket
import sys
import os

ip = sys.argv[1]
filename = sys.argv[2]

port = 4444

s = socket.socket()

with open(filename, 'rb') as f:
    s.connect((ip, port))
    s.send(os.path.getsize(filename).to_bytes(4, 'big'))

    s.sendfile(f)
