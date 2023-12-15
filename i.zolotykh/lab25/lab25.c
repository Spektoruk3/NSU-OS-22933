#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>
#define MSGSIZE 100

int main(int argc, char** argv) {
    int filedesc[2];
    pid_t pid1, pid2;
    char str[MSGSIZE];
    int desc_file;
    if (argc != 2) {
        fprintf(stderr, "incorrect number of arguments\n");
        return 1;
    }
    if (pipe(filedesc) == -1) {
        perror("Error creating pipe");
        return 1;
    }

    pid1 = fork();

    if (pid1 == -1) {
        perror("cannot create fork1");
        close(filedesc[0]);
        close(filedesc[1]);
        _exit(EXIT_FAILURE);
    }
    if (pid1 == 0) {
        desc_file = open(argv[1], O_RDONLY);
        close(filedesc[0]);
        int len;
        while ((len = read(desc_file, str, MSGSIZE)) > 0) {
            if (write(filedesc[1], str, len) < 0) break;
        }
        close(filedesc[1]);
        close(desc_file);
        return 0;
    }

    pid2 = fork();
    if (pid2 == -1) {
        perror("cannot create fork2");
        close(filedesc[0]);
        close(filedesc[1]);
        _exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        close(filedesc[1]);
        int len;
        while (len = read(filedesc[0], str, MSGSIZE)) {
            for (int i = 0; i < len; i++) {
                str[i] = toupper(str[i]);
            }
            if (write(1, str, len) < 0) break;
        }
        close(filedesc[0]);
        return 0;
    }

    close(filedesc[0]);
    close(filedesc[1]);
    waitpid(pid1, 0, 0);
    waitpid(pid2, 0, 0);
    return 0;
}