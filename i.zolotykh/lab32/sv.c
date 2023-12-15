#define _POSIX_C_SOURCE 199309L
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
#include <aio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#include "../utils.h"

const char *socket_path = "socket";

#define BUFSIZE 256
#define ENTSIZE 256

struct bufentry {
    struct aiocb request;
    int status;
    char buf[BUFSIZE];
};

struct bufentry entries[ENTSIZE];

void async_read(int idx, int fd) {
    entries[idx].status = 1;
    entries[idx].request.aio_buf = entries[idx].buf;
    entries[idx].request.aio_fildes = fd;
    entries[idx].request.aio_nbytes = BUFSIZE;
    entries[idx].request.aio_offset = 0;
    entries[idx].request.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    entries[idx].request.aio_sigevent.sigev_signo = SIGALRM;
    entries[idx].request.aio_sigevent.sigev_value.sival_int = idx;

    aio_read(&entries[idx].request);
}

// can rewrite to just status change and then process in main loop
void aio_read_handler(int signal, siginfo_t *info, void* uap) {
    int idx = info->si_value.sival_int;
    int res = aio_return(&entries[idx].request);
    if (res < 0) {
        perror("aio_read");
        close(entries[idx].request.aio_fildes);
        entries[idx].status = 0;
    } else if (res == 0) {
        // eof
        close(entries[idx].request.aio_fildes);
        entries[idx].status = 0;
    } else {
        for (int i = 0; i < res; i++) {
            entries[idx].buf[i] = toupper(entries[idx].buf[i]);
        }
        // mb should swap next two statements
        write(1, entries[idx].buf, res);
        async_read(idx, entries[idx].request.aio_fildes);
    }
}

int main() {
    int listen_fd = -1;

    DO_OR_DIE(listen_fd = socket(AF_UNIX, SOCK_STREAM, 0), "socket");

    unlink(socket_path);

    struct sockaddr_un addr;

    memset(&addr, 0, sizeof(addr));

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    addr.sun_family = AF_UNIX;

    DO_OR_DIE(bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)), "bind");

    DO_OR_DIE(listen(listen_fd, 32), "listen 32");
    
    
    struct sigaction read_action;
    read_action.sa_sigaction = aio_read_handler;
    sigemptyset(&read_action.sa_mask);
    read_action.sa_flags = SA_SIGINFO;
    sigaction(SIGALRM, &read_action, NULL);

    memset(entries, 0, sizeof(struct bufentry) * ENTSIZE);

    while (1) {
        int new_conn = accept(listen_fd, NULL, NULL);
        if (new_conn == -1) {
            continue;
        }
        
        int found = 0;
        for (int i = 0; i < ENTSIZE; i++) {
            if (entries[i].status == 0) {
                found = 1;
                async_read(i, new_conn);
                break;
            }
        }
        if (!found) {
            // should never pass
            fprintf(stderr, "overflow... refusing connection\n");
        }
    }
}