#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 256

char *socket_path = "socket";

int main(int argc, char *argv[]) {
    struct sockaddr_un addr;
    char buf[BUFSIZE];

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    unlink(socket_path);

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

    int res = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) {
        perror("bind");
        close(fd);
        exit(1);
    }

    res = listen(fd, 1);
    if (res == -1) {
        perror("listen");
        close(fd);
        exit(1);
    }

    int cl;
    while (1) {
        if ((cl = accept(fd, NULL, NULL)) == -1) {
            perror("accept error");
            continue;
        }

        int r;
        while ( (r = read(cl, buf, sizeof(buf))) > 0) {
            for (int i = 0; i < r; i++) {
                buf[i] = toupper(buf[i]);
            }
            printf("%.*s", r, buf);
        }
        if (r == -1) {
            perror("read");
            close(cl);
            exit(1);
        } else if (r == 0) {
            printf("EOF\n");
        }
        close(cl);
    }

    return 0;
}
