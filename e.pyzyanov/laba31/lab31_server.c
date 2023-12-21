#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define SOCKET_PATH "/tmp/AAA"

int main() {
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SOCKET_PATH);
    unlink(SOCKET_PATH);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    int client_sockets[MAX_CLIENTS];
    memset(client_sockets, 0, sizeof(client_sockets));

    printf("Server is waiting for connections...\n");

    while (1) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);

        int max_fd = server_socket;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
                max_fd = (client_sockets[i] > max_fd) ? client_sockets[i] : max_fd;
            }
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_socket, &read_fds)) {
            int client_socket = accept(server_socket, NULL, NULL);
            if (client_socket == -1) {
                perror("Accept failed");
                continue;
            }

            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_socket;
                    break;
                }
            }

            printf("New client connected, socket fd is %d\n", client_socket);
        }

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (client_sockets[i] > 0 && FD_ISSET(client_sockets[i], &read_fds)) {
                char buffer[1024];
                ssize_t bytes_received = recv(client_sockets[i], buffer, sizeof(buffer), 0);
                if (bytes_received <= 0) {
                    close(client_sockets[i]);
                    client_sockets[i] = 0;
                } else {
                    for (ssize_t j = 0; j < bytes_received; ++j) {
                        buffer[j] = toupper(buffer[j]);
                    }

                    printf("Received from client %d: %.*s\n", client_sockets[i], (int)bytes_received, buffer);

                    for (int j = 0; j < MAX_CLIENTS; ++j) {
                        if (client_sockets[j] > 0 && client_sockets[j] != client_sockets[i]) {
                            send(client_sockets[j], buffer, bytes_received, 0);
                        }
                    }
                }
            }
        }
    }

    close(server_socket);

    return 0;
}
