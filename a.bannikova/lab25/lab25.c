#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main() {
    int fildes[2];
    if (pipe(fildes) == -1) {
        return 1;
    }

    pid_t pid = fork();
    

    if (pid == 0) {
        close(fildes[1]);

        char c;
        while (read(fildes[0], &c, 1) != 0) {
            printf("%c", toupper(c));
        }

        close(fildes[0]);
    }

    else if (pid == -1) {
        return 1;
    }

    else {
        close(fildes[0]);
        int file_des = open("text.txt", O_RDONLY);
        char x;
        while (read(file_des, &x, 1) != 0) {
            write(pipe_filedescs[1], &x, 1);
        }
        
        close(fildes[1]);
        wait(NULL);
    }

    return 0;
}