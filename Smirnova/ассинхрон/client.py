import socket

# Создание сокета
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Подключение к серверу
server_address = ('localhost', 12345)
client_socket.connect(server_address)

while True:
    data = client_socket.recv(1024)
    print(f"Получено: {data.decode()}")

#client_socket.close()