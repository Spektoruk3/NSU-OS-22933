#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <poll.h>


char socket_path[] = "socket";
const int MY_BUF_SIZE = 2;
const int backlog = 5;
const int MAX_CLIENTS_CNT = 2;

void addConnection(struct pollfd *poll_list, int fd) {
    for (int i = 1; i < MAX_CLIENTS_CNT + 1; i++) {
        if (poll_list[i].fd < 0) {
            poll_list[i].fd = fd;
            poll_list[i].events = POLLIN;
            return;
        }
    }
}


int main(int argc, char *argv[]) {

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket goes wrong");
        exit(0);
    }

    struct sockaddr_un addr;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    unlink(socket_path);

    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind goes wrong");
        exit(0);
    }

	int clients_cnt = 0;

    if (listen(fd, backlog) == -1) {
        perror("listen goes wrong");
        exit(0);
    }

    struct pollfd poll_fds[MAX_CLIENTS_CNT + 1];

    for (int i = 0; i < MAX_CLIENTS_CNT + 1; ++i) {
        poll_fds[i].fd = -1;
        poll_fds[i].events = POLLIN;
    }

    poll_fds[0].fd = fd;

    char buf[MY_BUF_SIZE];

    while (1) {
        if (poll(poll_fds, MAX_CLIENTS_CNT + 1, -1) == -1) {
            perror("poll goes wrong");
            exit(0);
        }

		// check socket and connections error
        for (int i = 0; i < MAX_CLIENTS_CNT + 1; ++i) {
            if (poll_fds[i].fd < 0) {
                continue;
            }

            // POLLERR - An exceptional condition has occurred on the device or socket
            
            if (poll_fds[i].revents & POLLERR) {
                close(poll_fds[i].fd);
                poll_fds[i].fd = -1;

                if (i == 0) {
                    printf("Server error");
                    exit(-1);
                } else {
                    printf("\n\nClosing connection after error\n\n");
					--clients_cnt;
                }
            }
        }

		// add connection
        if (poll_fds[0].revents & POLLIN && clients_cnt < MAX_CLIENTS_CNT) {
            int cl = accept(fd, NULL, NULL);
            if (cl == -1) {
                perror("accept goes wrong");
                exit(0);
            }

            addConnection(poll_fds, cl);
			++clients_cnt;
			
            printf("\n\nAdd connection\n\n");
        }

		// read data from connections or close them
        for (int i = 1; i <= MAX_CLIENTS_CNT; ++i) {
            if (poll_fds[i].fd < 0) {
                continue;
            }

            if (poll_fds[i].revents & POLLIN) {

                int rc = read(poll_fds[i].fd, buf, MY_BUF_SIZE);
                for (int j = 0; j < rc; ++j) {
                    buf[j] = toupper(buf[j]);
                    printf("%c", buf[j]);
                }
                fflush(stdout);

                if (rc == -1) {
                    perror("read goes wrong");
                    exit(0);
                }

                if (rc == 0) {
					close(poll_fds[i].fd);
                    poll_fds[i].fd = -1;
					--clients_cnt;

                    printf("\n\nEOF reached, closing connection\n\n");
                }

            } else {

				// POLLHUP - The device or socket has been disconnected

                if (poll_fds[i].revents & POLLHUP) {
					close(poll_fds[i].fd);
                    poll_fds[i].fd = -1;
					--clients_cnt;

                    printf("\n\nClosing connection\n\n");
                }
            }
        }
    }
}

