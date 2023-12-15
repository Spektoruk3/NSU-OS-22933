#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>

#define BUFSIZE 256

int main() {
    int fd[2], pid;
    
    char data[] = "this wIlL be uppercase\n";

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    if ((pid = fork()) > 0) {
    } else if (pid == 0) {
        close(fd[0]);
        printf("child 1: writing to pipe\n");
        if (write(fd[1], data, sizeof(data)) == -1) {
            perror("write to pipe");
            return 1;
        }
        return 0;
    } else if (pid == -1) {
        perror("fork 1");
        return 1;
    }


    if ((pid = fork()) > 0) {
        close(fd[0]);
        close(fd[1]);
        printf("parent: closing both ends and exiting\n");
    } else if (pid == 0) {
        close(fd[1]);
        printf("child 2: reading from pipe and uppercasing\n");
        char buf[BUFSIZE];
        int r;
        while ((r = read(fd[0], buf, sizeof(buf))) > 0) {
            for (int i = 0; i < r; i++) {
                buf[i] = toupper(buf[i]);
            }
            write(1, buf, r);
        }
        if (r == -1) {
            perror("read from pipe");
            return 1;
        }
        return 0;
    } else if (pid == -1) {
        perror("fork 2");
        return 1;
    }
}