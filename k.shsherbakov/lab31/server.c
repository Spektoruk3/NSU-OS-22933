#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <string.h>

#define SOCKET_PATH "/tmp/multiplex_socket"
#define MAX_CLIENTS 5

void convertToUpper(char* str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int server_fd, client_fds[MAX_CLIENTS];
    socklen_t client_len;
    struct sockaddr_un server_addr, client_addr;
    fd_set read_fds, active_fds;

    // Initialize client file descriptors array
    memset(client_fds, 0, sizeof(client_fds));

    // Create socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s\n", SOCKET_PATH);

    FD_ZERO(&active_fds);
    FD_SET(server_fd, &active_fds);

    while (1) {
        read_fds = active_fds;

        // Use select for multiplexing
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // Check if the server socket is ready to accept a new connection
        if (FD_ISSET(server_fd, &read_fds)) {
            // Accept a new connection
            client_len = sizeof(client_addr);
            int new_client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

            if (new_client_fd == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New client connected\n");

            // Add the new client to the set of active file descriptors
            int i;
            for (i = 0; i < MAX_CLIENTS; ++i) {
                if (client_fds[i] == 0) {
                    client_fds[i] = new_client_fd;
                    FD_SET(new_client_fd, &active_fds);
                    break;
                }
            }

            // Check if we reached the maximum number of clients
            if (i == MAX_CLIENTS) {
                fprintf(stderr, "Too many clients. Connection rejected.\n");
                close(new_client_fd);
            }
        }

        // Check data from clients
        int i;
        for (i = 0; i < MAX_CLIENTS; ++i) {
            int client_fd = client_fds[i];
            if (client_fd > 0 && FD_ISSET(client_fd, &read_fds)) {
                char buffer[1024];
                memset(&buffer,0,sizeof(buffer));
                ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

                if (bytes_received > 0) {
                    // Process and print data from the client
                    if (buffer[0]=='-'){
                            if (buffer[1]=='q'){
                            close(server_fd);

                            // Remove the socket file
                            unlink(SOCKET_PATH);

                            return 0;}
                    }
                    convertToUpper(buffer);
                    printf("Received from client %d: %s\n", i + 1, buffer);

                    // Broadcast the message to all other clients
                    int j;
                    for (j = 0; j < MAX_CLIENTS; ++j) {
                        if (j != i && client_fds[j] > 0) {
                            send(client_fds[j], buffer, bytes_received, 0);
                        }
                    }
                } else if (bytes_received == 0) {
                    // Connection closed by the client
                    printf("Client %d disconnected\n", i + 1);
                    close(client_fd);
                    FD_CLR(client_fd, &active_fds);
                    client_fds[i] = 0;
                } else {
                    perror("recv");
                }
            }
        }
    }

    // Close the server socket
    close(server_fd);

    // Remove the socket file
    unlink(SOCKET_PATH);

    return 0;
}