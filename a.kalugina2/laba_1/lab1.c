#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <ulimit.h>
#include <sys/resource.h>

extern char** qwerty;

int main(int argc, char *argv[])
 {
     char opt[] = "cipsdvuU:C:V:";  /* valid opt */
     int c; char abc[PATH_MAX]; struct rlimit relame;
     
     printf("argc equals %d\n\n", argc);

     while ((c = getopt(argc, argv, opt)) != EOF) {
      switch (c) {
      
      case 'C':
          getrlimit(RLIMIT_FSIZE, &relame);
          relame.r_limit_current = atol(optarg);

          if(!setrlimit(RLIMIT_FSIZE, &relame)){
               printf("Succsesfully changed to %ld\n\n", relame.r_limit_current);
          }

          else{
               perror("strrlimit() error");
               return 1;
          }

          break;

      case 'V':
           putenv(optarg);
           printf("Added new env %s\n\n", optarg);
           break;

      case 'U':
          if (ulimit(UL_SETFSIZE, atol(optarg)) == -1){
               perror("Wasn't able to change file limit\n");
               return 1;
          }
      
          printf("Size changed to %ld\n\n", atol(optarg));
          break;

      case 'd':
          if (getcwd(abc, sizeof(abc)) != NULL) {
              printf("Current working dir: %s\n\n", abc);
          }

          else {
              perror("getcwd() error");
              return 1;
          }

          break;

      case 's':
          printf("Changing leader of the group\n");
          printf("\n");
          setpgid(0, getgid());
          break;

      case 'i':
          printf("Effective User ID = %d\n", geteuid());
          printf ("Real User ID = %d\n",getuid());
          printf("Effective Group ID = %d\n", getegid());
          printf ("Real Group ID = %d\n\n",getgid());
          break;

      case 'p':
          printf("Process ID = %d\n", getpid());
          printf("Parent Process ID = %d\n", getppid());
          printf("Group Process ID = %d\n\n", getpgrp());
          break;

      case 'c':
          getrlimit(RLIMIT_FSIZE, &relame);
          printf("Current core file size limit = %ld\n", relame.r_limit_current);
          printf("Maximum allowed = %4ld\n\n", relame.r_limit_maxim);
          break;
      
      case 'u':
          printf("Size of ulimit is %ld\n\n", ulimit(UL_GETFSIZE));
          break;
      
      case 'v':
           printf("Current envs\n");
           for(int i = 0; qwerty[i]!=0; i++){
                printf("%d    %s\n", i, qwerty[i]);
           }
      
           printf("\n");
           printf("\n");
           break;

      case '?':
          printf("invalid option is %c\n", optopt);
      }

     }
     return 0;
 }
