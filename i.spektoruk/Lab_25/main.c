#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int fd[2];

    if (pipe(fd) == -1) {
        perror("");
        exit(0);
    }

    int textfd = open("text.txt", O_RDONLY);
    const int BUFSIZE = 3;
    char buf[BUFSIZE];

    pid_t pid = fork();
    if (pid > 0) {
		// process - parent
        close(fd[0]);

        while (1) {
            int cnt = read(textfd, buf, BUFSIZE);
            if (cnt == 0) {
                break;
            } else if (cnt < 0) {
                perror("");
                exit(0);
            } else {
                write(fd[1], buf, cnt);
            }
        }
    } else if (pid == 0) {
		// process - child
        close(fd[1]);

        while (1) {
            int cnt = read(fd[0], buf, BUFSIZE);
            if (cnt == 0) {
                break;
            }
            if (cnt < 0) {
                perror("");
                exit(0);
            }
            for (int i = 0; i < cnt; ++i) {
                putc(toupper(buf[i]), stdout);
            }
		}
		putc('\n', stdout);
    } else {
		// cannot fork
		perror("");
	}
    return 0;
}
