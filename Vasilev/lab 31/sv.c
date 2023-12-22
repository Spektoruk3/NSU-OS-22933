#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include "../utils.h"

char *socket_path = "socket";

#define BUFSIZE 256

int main() {
    int listen_fd = -1;
    int end_server = 0;
    int close_conn = 0;
    char buf[BUFSIZE];

    int timeout = 10 * 60 * 1000;
    
    struct pollfd fds[200];
    int nfds = 1;

    DO_OR_DIE(listen_fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0), "socket");

    unlink(socket_path);

    struct sockaddr_un addr;

    memset(&addr, 0, sizeof(addr));

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    addr.sun_family = AF_UNIX;

    DO_OR_DIE(bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)), "bind");

    DO_OR_DIE(listen(listen_fd, 32), "listen 32");

    memset(fds, 0, sizeof(fds));

    fds[0].fd = listen_fd;
    fds[0].events = POLLIN;

    while (!end_server) {
        int poll_res = poll(fds, nfds, timeout);

        if (poll_res < 0) {
            perror("poll");
            break;
        }

        if (poll_res == 0) {
            fprintf(stderr, "poll timeout\n");
            break;
        }

        int current_size = nfds;
        for (int i = 0; i < current_size; i++) {
            if (fds[i].revents == 0) {
                continue;
            }

            if ((fds[i].revents & POLLIN) == 0) {
                fprintf(stderr, "bad fds[i].revents: %d\n", fds[i].revents);
                end_server = 1;
                break;
            }

            if (fds[i].fd == listen_fd) {
                int new_conn = -1;
                while ((new_conn = accept(listen_fd, NULL, NULL)) != -1) {
                   if (new_conn < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("accept");
                            end_server = 1;
                        } else {
                            fprintf(stderr, "EWOULDBLOCK ???\n");
                        }
                        break;
                    }
                    
                    fds[nfds].fd = new_conn;
                    fds[nfds].events = POLLIN;
                    nfds++;
                }
            } else {
                close_conn = 0;

                while (1) {
                    int r = read(fds[i].fd, buf, sizeof(buf));
                    if (r < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("recv");
                            close_conn = 1;
                        }
                        break;
                    }

                    if (r == 0) {
                        printf("connection %d closed\n", fds[i].fd);
                        close_conn = 1;
                        break;
                    }

                    for (int pos = 0; pos < r; pos++) {
                        buf[pos] = toupper(buf[pos]);
                    }
                    printf("%.*s", r, buf);
                }

                if (close_conn) {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
            }
        }
    }
}