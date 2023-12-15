#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *socket_path = "socket";

int main(int argc, char *argv[]) {
    struct sockaddr_un addr;
    char buf[100];

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

    int res = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) {
        perror("connect");
        close(fd);
        exit(1);
    }

    int r;
    while ((r = read(0, buf, sizeof(buf))) > 0) {
        if (write(fd, buf, r) != r) {
            if (r > 0) {
                fprintf(stderr, "partial write");
            } else {
                perror("write");
                close(fd);
                exit(1);
            }
        }
    }

  return 0;
}