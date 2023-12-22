#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#define SOCKET_PATH "/tmp/multiplex_socket"
#define MAX_CLIENTS 5

void convertToUpper(char* str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int make_socket_non_blocking(int sfd) {
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1) {
        perror("fcntl");
        return -1;
    }

    return 0;
}
void handle_sigint(int sig) {
    fprintf(stdout, "Received SIGINT. Closing server.\n");
    unlink(SOCKET_PATH);
    exit(EXIT_SUCCESS);
}

int main() {
    int server_fd, client_fds[MAX_CLIENTS];
    socklen_t client_len;
    struct sockaddr_un server_addr, client_addr;
    struct pollfd poll_fds[MAX_CLIENTS + 1];
    
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
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

    // Make the server socket non-blocking
    if (make_socket_non_blocking(server_fd) == -1) {
        perror("make_socket_non_blocking");
        exit(EXIT_FAILURE);
    }

    // Initialize poll_fds array
    poll_fds[0].fd = server_fd;
    poll_fds[0].events = POLLIN;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        poll_fds[i + 1].fd = -1;
        poll_fds[i + 1].events = POLLIN;
    }

    while (1) {
        // Use poll for asynchronous I/O
        if (poll(poll_fds, MAX_CLIENTS + 1, -1) == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        // Check if the server socket is ready to accept a new connection
        if (poll_fds[0].revents & POLLIN) {
            // Accept a new connection
            client_len = sizeof(client_addr);
            int new_client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

            if (new_client_fd == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New client connected\n");

            // Make the new client socket non-blocking
            if (make_socket_non_blocking(new_client_fd) == -1) {
                perror("make_socket_non_blocking");
                exit(EXIT_FAILURE);
            }

            // Add the new client to the poll_fds array
            for (int i = 1; i < MAX_CLIENTS + 1; ++i) {
                if (poll_fds[i].fd == -1) {
                    poll_fds[i].fd = new_client_fd;
                    break;
                }
            }
        }

        // Check data from clients
        for (int i = 1; i < MAX_CLIENTS + 1; ++i) {
            int client_fd = poll_fds[i].fd;
            if (client_fd != -1 && (poll_fds[i].revents & POLLIN)) {
                char buffer[1024];
                memset(&buffer,0,sizeof(buffer));
                ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

                if (bytes_received > 0) {
                    // Process and print data from the client
                    if (buffer[0]=='-')
                    {
                        if (buffer[1]=='q')
                        {
                            // Close the server socket
                            close(server_fd);
                            // Remove the socket file
                             unlink(SOCKET_PATH);
                             return 0;
                        }
                        
                    }
                    
                    convertToUpper(buffer);
                    printf("Received from client %d: %s\n", i, buffer);
                    // Broadcast the message to all other clients
                    for (int j = 1; j < MAX_CLIENTS + 1; ++j) {
                        if (j != i && poll_fds[j].fd != -1) {
                            send(poll_fds[j].fd, buffer, bytes_received, 0);
                        }
                    }
                } else if (bytes_received == 0) {
                    // Connection closed by the client
                    printf("Client %d disconnected\n", i);
                    close(client_fd);
                    poll_fds[i].fd = -1;
                } else if (bytes_received == -1 && errno != EAGAIN) {
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