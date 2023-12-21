#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/AAA"

int main() {
    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SOCKET_PATH);

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    char message[1024];
    printf("Enter message: ");
    fgets(message, sizeof(message), stdin);

    ssize_t bytes_sent = send(client_socket, message, strlen(message), 0);
    if (bytes_sent == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        char buffer[1024];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            printf("Received from server: %.*s\n", (int)bytes_received, buffer);
        }
        // Можно добавить условие выхода из цикла, например, по введенной команде пользователя

        // Если выход из цикла не предусмотрен, то можно оставить пустой while (1)
    }

    // Закрытие сокета клиента никогда не выполнится в данном коде из-за бесконечного цикла

    return 0;
}
