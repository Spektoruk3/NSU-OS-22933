import socket

sock = socket.socket()
sock.connect(('localhost', 9090))

p = "1"
while p != " ":
    p = str(input())
    b = bytes(p, 'utf-8')
    sock.send(b)
    data = sock.recv(1024)
    print(data.decode('utf-8'))

sock.close()
