#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#define BUFFSIZE 512

int main() {
    int fd[2];
    char buff_in[BUFFSIZE];
    char buff_out[BUFFSIZE];

    int k = pipe(fd);
    if (k == -1) {
	perror("pipe failed");
	exit(1);
    }

    pid_t pid = fork();
    if (pid > 0) {
	//parent
	close(fd[0]);

	int l;
	while((l = read(fileno(stdin), buff_in, BUFFSIZE)) > 0) {
	    write(fd[1], buff_in, l);
	}

	close(fd[1]);
    }
    else if (pid == 0) {
	//child
	close(fd[1]);

	int l;
	while ((l = read(fd[0], buff_out, BUFFSIZE)) > 0) {
	    for (int i = 0; i < l; i++) {
		buff_out[i] = toupper(buff_out[i]);
	    }
	    write(fileno(stdout), buff_out, l);
	}

	close(fd[0]);
    }
    else {
	perror("fork failed");
	exit(1);
    }

    return 0;

}
