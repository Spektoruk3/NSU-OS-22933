#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../utils.h"


char *socket_path = "socket";

int main(int argc, char *argv[]) {
    struct sockaddr_un addr;
    char buf[100];
    int fd;

    DO_OR_DIE(fd = socket(AF_UNIX, SOCK_STREAM, 0), "socket");

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

    DO_OR_DIE(connect(fd, (struct sockaddr*)&addr, sizeof(addr)), "connect");

    int r;
    while ((r = read(0, buf, sizeof(buf))) > 0) {
        if (write(fd, buf, r) != r) {
            if (r > 0) {
                fprintf(stderr, "partial write");
            } else {
                perror("write");
                exit(1);
            }
        }
    }

  return 0;
}