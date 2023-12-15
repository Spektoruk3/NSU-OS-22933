#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>


char socket_path[] = "socket";
const int MY_BUF_SIZE = 2;


int main(int argc, char *argv[]) {

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket goes wrong");
        exit(0);
    }

    struct sockaddr_un addr;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

	unlink(socket_path);
    
    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind goes wrong");
        exit(0);
    }
	
    if (listen(socket_fd, 1) == -1) {
        perror("listen goes wrong");
        exit(0);
    }


    int cl = accept(socket_fd, NULL, NULL);
    if (cl == -1) {
        perror("accept goes wrong");
        exit(0);
    }

    char buf[MY_BUF_SIZE];

    int rc = read(cl, buf, sizeof(buf));
    while (rc > 0) {
        for (int i = 0; i < rc; ++i) {
            buf[i] = toupper(buf[i]);
            printf("%c", buf[i]);
        }
		fflush(stdout);
        rc = read(cl, buf, sizeof(buf));
    }

    if (rc == -1) {
        perror("read goes wrong");
        exit(0);
    }
	printf("\nEOF readed\n");
    close(cl);

    exit(0);
}
