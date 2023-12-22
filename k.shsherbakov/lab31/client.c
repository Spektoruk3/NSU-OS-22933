#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define SOCKET_PATH "/tmp/multiplex_socket"

int main() {
    int client_fd;
    struct sockaddr_un server_addr;

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

    char input_buffer[1024];
    char output_buffer[1024];
    while (1) {
        // Read input from the user
        printf("Enter text (or 'exit' to quit): ");
        fgets(input_buffer, sizeof(input_buffer), stdin);

        // Check for exit command
        if (strncmp(input_buffer, "exit", 4) == 0) {
            break;
        }

        // Send input to the server
        send(client_fd, input_buffer, strlen(input_buffer), 0);
        ssize_t bytes_received = recv(client_fd, output_buffer, sizeof(output_buffer), 0);
        if (bytes_received > 0) {
            // Print the received data
            printf("Received from server: %.*s\n", (int)bytes_received, output_buffer);
        } else if (bytes_received == 0) {
            // Connection closed by the server
            printf("Server disconnected\n");
            break;
        } else {
            perror("recv");
            break;
        }
    }

    // Close the socket
    close(client_fd);

    return 0;
}