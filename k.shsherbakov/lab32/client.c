#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <errno.h>
#define SOCKET_PATH "/tmp/multiplex_socket"

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

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    struct pollfd poll_fds[2];

    // Create socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // Set up server address structure
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Make the client socket non-blocking
    if (make_socket_non_blocking(client_fd) == -1) {
        perror("make_socket_non_blocking");
        exit(EXIT_FAILURE);
    }

    // Initialize poll_fds array
    poll_fds[0].fd = client_fd;
    poll_fds[0].events = POLLIN;

    poll_fds[1].fd = 0;  // STDIN
    poll_fds[1].events = POLLIN;
    printf("to disconnekt enter \'exit\'\n");
    printf("to turn off server enter \'-q\'\n");
    char input_buffer[1024];
    char receive_buffer[1024];

    while (1) {
        // Use poll for asynchronous I/O
        if (poll(poll_fds, 2, -1) == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        // Check if there is data from the server
        if (poll_fds[0].revents & POLLIN) {
            ssize_t bytes_received = recv(client_fd, receive_buffer, sizeof(receive_buffer), 0);

            if (bytes_received > 0) {
                printf("Received from server: %.*s\n", (int)bytes_received, receive_buffer);
            } else if (bytes_received == 0) {
                printf("Server disconnected\n");
                break;
            } else if (bytes_received == -1 && errno != EAGAIN) {
                perror("recv");
                break;
            }
        }

        // Check if there is data from stdin
        if (poll_fds[1].revents & POLLIN) {
            // Read input from the user
            // printf("Enter text:");
            fgets(input_buffer, sizeof(input_buffer), stdin);

            // Check for exit command
            if (strncmp(input_buffer, "exit", 4) == 0) {
                break;
            }

            // Send input to the server
            send(client_fd, input_buffer, strlen(input_buffer), 0);
        }
    }

    // Close the socket
    close(client_fd);

    return 0;
}