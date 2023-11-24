#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(){
	int pipefd[2];
	pid_t pid;

	if(pipe(pipefd) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if(pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if(pid == 0){
		close(pipefd[1]);

		char buffer[256];
		ssize_t numRead;

		while((numRead = read(pipefd[0], buffer, sizeof(buffer))) > 0){
			for(int i = 0; i < numRead; i++){
				buffer[i] = toupper(buffer[i]);
			}
			write(STDOUT_FILENO, buffer, numRead);
		}
		close(pipefd[0]);
		_exit(EXIT_SUCCESS);
	}else{
		close(pipefd[0]);

		char *text = "Hello world!\n";
		write(pipefd[1], text, strlen(text));
		close(pipefd[1]);
		wait(NULL);
		exit(EXIT_SUCCESS);
	}
	return 0;
}