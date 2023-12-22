#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <spawn.h>
#include <unistd.h>
#include <errno.h>


int main(int argv, char* argc[])
{
    if (argv == 1)
    {
        printf("You need to put filepath in parameters of program\n");
        return 1;
    }
    printf("Pid of parent process is: %i\n", getpid());
    char *args_to_child[] = {"cat", argc[1], NULL};
    pid_t child_proc, child_dummy;
    int status;
    
    child_proc = fork();
    if (child_proc == (pid_t)-1)
    {
        printf("Error with process creating: %s\n", strerror(status));
        return 1;
    }
    if (!child_proc)
    {
        printf("Process has been created with pid: %i\n", getpid());
        execv("/bin/cat", args_to_child);
        exit(0);
    }
    printf("Text in the main process before waiting of child\n");
    do
    {
        child_dummy = waitpid(child_proc, &status, 0);
        if (child_dummy == (pid_t)-1 && errno != EINTR)
        {
            break;
        }
    } while(child_dummy != child_proc);
    printf("Text in the main process after waiting of child\n");
    return 0;
}