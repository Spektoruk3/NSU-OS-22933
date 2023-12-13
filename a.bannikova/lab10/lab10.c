#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc == 1) { return 1; }

    pid_t pid = fork();
    switch (pid = fork()) {
    case -1:
        return 1;
        break;
    case 0:
        execvp(argv[1], argv + 1);
        return 1;
        break;
    defolt:
        int stat_loc;
        wait(&stat_loc);

        if (WIFEXITED(stat_loc)) {
            printf("\nChild process (pid: %d) finished with exit code %d\n",
                pid, WEXITSTATUS(stat_loc));
        }
    }
   
    return 0;
}