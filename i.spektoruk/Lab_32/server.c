#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aio.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>

#define CLIENT_NUMBER 7
#define MAX_SIZE 1024

int activeClientID = -1;
sigjmp_buf toread;

struct AIO {
    int ClientID;
    struct aiocb block;
    char buffer[MAX_SIZE];
};

struct AIO clients[CLIENT_NUMBER];

int getNewID() {
    for (int i = 1; i < CLIENT_NUMBER; i++) {
        if (clients[i].ClientID == 0) {
            return i;
        }
    }
    return -1;
}

void init_Aio(int i, int fileDesc)
{
    clients[i].ClientID = i;
    clients[i].block.aio_fildes = fileDesc;
    clients[i].block.aio_offset = 0;
    clients[i].block.aio_buf = clients[i].buffer;
    clients[i].block.aio_nbytes = MAX_SIZE;
    clients[i].block.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    clients[i].block.aio_sigevent.sigev_signo = SIGIO;
    clients[i].block.aio_sigevent.sigev_value.sival_int = i;
}

void asyncRead(int i, int fileDesc) {
    init_Aio(i, fileDesc);
    aio_read(&clients[i].block);
}

static void SIGIO_handler(int signo, siginfo_t* siginfo, void* context) {
    if (signo != SIGIO || siginfo->si_signo != SIGIO) {
        return;
    }
    int index = siginfo->si_value.sival_int;
    if (aio_error(&clients[index].block) == 0) {
        activeClientID = index;
        siglongjmp(toread, 1);
    }
}


int main() {
    memset(clients, 0, CLIENT_NUMBER * sizeof(struct AIO));

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

    unlink("./socket");
    if (bind(sock, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) < 0) {
        perror("Err to bind");
        close(sock);
        return -1;
    }


    if (listen(sock, CLIENT_NUMBER) != 0) {
        perror("Err to listen");
        unlink(socketAddr.sun_path);
        close(sock);
        return -1;
    }

    sigset_t sigIO;
    sigaddset(&sigIO, SIGIO);

    struct sigaction sigiohandleraction;
    sigemptyset(&sigiohandleraction.sa_mask);
    sigiohandleraction.sa_sigaction = SIGIO_handler;
    sigiohandleraction.sa_mask = sigIO;
    sigiohandleraction.sa_flags = SA_SIGINFO;
    sigaction(SIGIO, &sigiohandleraction, NULL);

    if (sigsetjmp(toread, 1) == 1 && activeClientID > 0) {
        size_t size = aio_return(&clients[activeClientID].block);
        if (size == 0) {
            close(clients[activeClientID].block.aio_fildes);
            clients[activeClientID].ClientID = 0;
        }
        if (size > 0) {
            char msg[11];
            sprintf(msg, "client %d: ", activeClientID);
            write(STDOUT_FILENO, msg, sizeof(msg));
            for (size_t i = 0; i < size; i++) {
                clients[activeClientID].buffer[i] = toupper(clients[activeClientID].buffer[i]);
            }
            write(STDOUT_FILENO, clients[activeClientID].buffer, size);
            aio_read(&clients[activeClientID].block);
        }
    }

    while (true) {
        int newClientFD;
        if ((newClientFD = accept(sock, NULL, NULL)) == -1) {
            if (errno == EINTR) {
                printf("interrupted\n");
                continue;
            }
            else {
                perror("Err to accept");
                continue;
            }
        }
        int newClientID;
        if ((newClientID = getNewID()) != -1) {
            asyncRead(newClientID, newClientFD);
        }
        else {
            printf("server is busy\n");
        }
    }
}



