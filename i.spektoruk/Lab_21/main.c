#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int count = 0;

void sigcatch(int sig){
   if (sig == SIGQUIT){
      char buf[40];
      snprintf(buf, 40, "\nCount of SIGINT: %d\n", count);
      write(STDOUT_FILENO, buf, strlen(buf));
      exit(1);
   }
   write(STDOUT_FILENO, "\a", sizeof(char));
   count++;
}


int main(){
   sigset(SIGINT, sigcatch);
   sigset(SIGQUIT, sigcatch);
   while(1) {
      pause();
   }
}
