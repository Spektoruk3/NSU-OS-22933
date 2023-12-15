#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <time.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

#define CLIENT_NUMBER 7
#define MAX_SIZE 1024

int main(int argc, char *argv[] ) {
    printf("init sockaddr\n");
    struct sockaddr_un un;
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    char* name = "./socket";
    strcpy(un.sun_path, name);
    long N = strtol(argv[1], NULL, 10);
    double SLEEPING_TIME = strtod(argv[2], NULL);
    int sockets[CLIENT_NUMBER];

    printf("init connections\n");
    for (int i = 0; i < CLIENT_NUMBER; ++i) {
        if ((sockets[i] = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
            fprintf(stderr, "Socket #%d creation exception", i);
            continue;
        }
        if (connect(sockets[i], (struct sockaddr* )&un, sizeof(un)) == -1){
            fprintf(stderr, "Connection exception with #%d socket", i);
            close(sockets[i]);
            continue;
        }
    }

    printf("sending...\n");
    char buff[MAX_SIZE];

                for (int i = 0; i < CLIENT_NUMBER; ++i) {
        sprintf(buff, "%d", i);
        printf("buff: %s; strlen: %d\n", buff, strlen(buff) + 1);
        ssize_t written = write(sockets[i], buff, strlen(buff) + 1);
        printf("Written\n");
        sleep(SLEEPING_TIME / 1000.);
        if(written == -1) {
            fprintf(stderr, "Writing error with #%d socket", i);
            close(sockets[i]);
            continue;
        }
    }

    printf("closing...\n");
    for (int i = 0; i < CLIENT_NUMBER; ++i) {
        close(sockets[i]);
    }

    return 0;
}

