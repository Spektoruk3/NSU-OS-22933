#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if(argc == 1)
    {
        perror("Incorrect args");
    }

    char** argument_list = (char**)malloc(argc - 1);
    argument_list[0] = (char*)malloc(sizeof(argv[1]));
    argument_list[0] = argv[1];
    int i = 1, j = 2;

    while(j < argc) 
    {
        argument_list[i] = (char*)malloc(sizeof(argv[j]));
        argument_list[i] = argv[j];
        i++; j++;
    }

    pid_t process_id = fork();
    int status;

    if(process_id < 0) {
        perror("bad fork");
        exit(1);
    }
    if(process_id == 0) {
        execvp(argv[1], argument_list);
        exit(0);
    }
    
    waitpid(process_id, &status, 0);
    if(WIFEXITED(status))
        printf("exit code: %d\n", WEXITSTATUS(status));
    return 0;
}