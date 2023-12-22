import asyncio
import time

connected_clients = []

async def handle_client(reader, writer):
    addr = writer.get_extra_info('peername')
    print(f"Подключение клиента {addr}")

    connected_clients.append(writer)
    print(len(connected_clients))
    
    while len(connected_clients) == 2 :
        message = input()
        current_time = time.strftime("%H:%M:%S", time.localtime()) # Получаем текущее время
        server_message = f"{current_time} - {message}" # Формируем сообщение сервера с временной меткой

# Отправляем сообщение всем клиентам
        for client in connected_clients:
            client.write(server_message.encode())

        print(f"Записано на сервере: {server_message}")
        writer.close()
        print(len(connected_clients))

async def main(host, port):
    server = await asyncio.start_server(handle_client, host, port)
    async with server:
        await server.serve_forever()

host = '127.0.0.1' # Адрес сервера
port = 12345 # Порт сервера

asyncio.run(main(host, port))