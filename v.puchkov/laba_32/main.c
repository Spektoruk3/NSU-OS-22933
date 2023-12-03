#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <poll.h>
#include <sys/mman.h>
#include <aio.h>

#define SOCKFILE_NAME "./32.socket"
#define CONNCNT 15

#define flag char

void myexit(const char *msg, int sd) {
    perror(msg);
    unlink(SOCKFILE_NAME);
    close(sd);
    exit(EXIT_FAILURE);
}

struct sockaddr_un buildsockname() {
    struct sockaddr_un sockname;
    sockname.sun_family = PF_UNIX;
    strcpy(sockname.sun_path, SOCKFILE_NAME);
    return sockname;
}

int sockcreate() {
    errno = 0;
    int sd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(sd == -1) {
        myexit("socket isn't created", -1);
    }
    return sd;
}

void sockconnect(int sd) {
    struct sockaddr_un sockname = buildsockname();

    errno = 0;
    if(connect(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1) {
        myexit("connection failed", sd);
    }
}

void sockwritemsg(int sd, char *msg) {
    errno = 0;
    if(write(sd, msg, strlen(msg) + 1) == -1) {
        myexit("writing failed", sd);
    }
}

void sockbind(int sd) {
    struct sockaddr_un sockname = buildsockname();

    errno = 0;
    if(bind(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1) {
        myexit("socket binding failed", sd);
    }
}

void socklisten(int sd, int cnt) {
    errno = 0;
    if(listen(sd, cnt) == -1) {
        myexit("listen failed", sd);
    }
}

int sockaccept(int sd) {
    errno = 0;
    int clientsd = accept(sd, NULL, NULL);
    if(clientsd == -1) {
        myexit("accept failed", sd);
    }
    return clientsd;
}

void msguppercase(union sigval sival) {
    struct aiocb *readrq = (struct aiocb *)sival.sival_ptr;
    printf("msguppercase\n");
    int sd = readrq->aio_fildes;
    char *buf = readrq->aio_buf;

    if(*buf == 0) {
        return;
    }

    printf("A message \"%s\" was received\n", buf);
    char *ptr;
    for(ptr = buf; *ptr; ptr++) {
        *ptr = toupper(*ptr);
    }
    printf("The message was uppercased: \"%s\"\n", buf);

    *buf = 0;

    if(aio_read(readrq) == -1) {
        perror("aio_read");
    }
}

void intclose(int sig) {
    unlink(SOCKFILE_NAME);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

    //--------------------------Client-----------------------------
    if(argc > 1 && !strcmp(argv[1], "client")) {
        int clientsd = sockcreate();
        printf("Socket is created\n");

        sockconnect(clientsd);
        printf("Connected to a server\nPrint the message:\n");

        char msg[256] = "Default message";
        while(gets(msg)) {
            sockwritemsg(clientsd, msg);
            printf("Message \"%s\" is sent\n", msg);
        }

        close(clientsd);
        printf("client socket is closed\n");
    }

    //--------------------------Server-----------------------------

    else {
        int sockdes = sockcreate();
        printf("Socket is created\n");

        sockbind(sockdes);
        printf("Socket is bound\n");

        signal(SIGINT, intclose);

        socklisten(sockdes, CONNCNT);
        printf("Program is listening for a socket. Waiting for connections...\n");

        int rqcnt = 0;

        char buf[CONNCNT][BUFSIZ];
        // struct aiocb *listrq[CONNCNT];

        while(1) {
            struct aiocb *readrq = malloc(sizeof(struct aiocb));
            readrq->aio_fildes = sockaccept(sockdes);
            printf("sockaccept\n");

            readrq->aio_offset = 0;
            readrq->aio_buf = buf[rqcnt];
            readrq->aio_nbytes = BUFSIZ;
            readrq->aio_reqprio = 0;

            struct sigevent sigevent;
            sigevent.sigev_notify = SIGEV_THREAD;
            sigevent.sigev_signo = SIGUSR1;
            sigevent.sigev_value = (union sigval){(void *) readrq};
            sigevent.sigev_notify_function = msguppercase;
            sigevent.sigev_notify_attributes = 0;

            readrq->aio_sigevent = sigevent;
            readrq->aio_lio_opcode = 0;

            // listrq[rqcnt] = readrq;

            rqcnt++;

            if(aio_read(readrq) == -1) {
                perror("aio_read");
            }

        }

        unlink(SOCKFILE_NAME);
        printf("%s is removed\n", SOCKFILE_NAME);

    }

    return 0;

}
