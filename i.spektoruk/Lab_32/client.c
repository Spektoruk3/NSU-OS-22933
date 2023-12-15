#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>

#define MAX_SIZE 1024

bool connectionAlive = false;

static void sig_usr(int signo) {
    if (signo == SIGPIPE) {
        connectionAlive = false;
    }
}

int main() {

    if (sigset(SIGPIPE, sig_usr) == SIG_ERR) {
        perror("Err to sigset");
        exit(1);
    }


    int sock;
    struct sockaddr_un socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sun_family = AF_UNIX;
    char* socketPath = "./socket";
    strcpy(socketAddr.sun_path, socketPath);

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Err to socket");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) == -1) {
        perror("Err to connect");
        close(sock);
        return -1;
    }
    else {
        printf("connected\n");
        connectionAlive = true;
    }

    char readWriteBuf[MAX_SIZE];
    char tempBuf[MAX_SIZE];
    int readCount;
    int writeCount;

    while ((readCount = read(STDIN_FILENO, readWriteBuf, MAX_SIZE)) > 0) {
        if (!connectionAlive) {
            close(sock);
            printf("connection was closed\n");
            return 0;
        }
        while ((writeCount = write(sock, readWriteBuf, readCount)) < readCount) {
        }

        if (writeCount == 0) {
            return 0;
        }

        if (writeCount == -1) {
            perror("Err to write");
            return -1;
        }
    }

    if (readCount == -1) {
        perror("Err to write");
        return -1;
    }

    close(sock);
    return 0;
}

