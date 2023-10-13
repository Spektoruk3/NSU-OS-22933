#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ulimit.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <limits.h>
#include <errno.h>

extern char** environ;

int main(int argc, char *argv[]){
    char options[ ] = "ispuU:cC:dvV:f:dg:";  /* valid options */
    int c, invalid = 0, dflg = 0, fflg = 0, gflg = 0;
    extern int optind, optopt;
    extern char *optarg;
    char *f_ptr, *g_ptr;
    char *cwd;
    long ulim;
    struct rlimit core_lim;
    printf("argc equals %d\n", argc);
    while ((c = getopt(argc, argv, options)) != EOF) {
        switch (c) {
            case 'i':
                printf("Real user id: %d\n", getuid());
                printf("Effective user id: %d\n", geteuid());
                printf("Real group id: %d\n", getgid());
                printf("Effective group id: %d\n", getegid());
                break;

            case 's':
                setpgid(0, getgid());
                printf("The process became a leader of the group\n");
                break;
            case 'p':
                printf("Process id: %d\n", getpid());
                printf("Parent-process id: %d\n", getppid());
                printf("Group of processes id: %d\n", getpgid(0));
                break;

            case 'u':
                printf("ulimit = %ld\n",ulimit(UL_GETFSIZE,0));
                break;

            case 'U':
                ulim = atol(optarg); 
                if (ulim % 512 != 0) {
                    fprintf(stderr, "New ulimit value is not divisible by 512\n");
                    continue;
                }
                ulim = ulimit(UL_SETFSIZE, ulim / 512);
                if (ulim == -1) {
                    perror("Failed to set ulimit"); 
                    continue;
                }
                break;

            case 'c':
                printf("Max size of possible core-file: %d\n", RLIMIT_CORE);
                break;

            case 'C':
                if (getrlimit(RLIMIT_CORE, &core_lim) != 0)
                    perror("Failed to get core limit");
                core_lim.rlim_cur = atol(optarg); 
                if (setrlimit(RLIMIT_CORE, &core_lim) != 0)
                    perror("Failed to set core limit");
                break;

            case 'd':
                cwd = getcwd(NULL, 256);
                if (cwd == NULL) {
                    perror("Failed to get cwd");
                    continue;
                }
                printf("Current working directory: %s\n", cwd);
                free(cwd);
                cwd = NULL;
                break;

            case 'v':
                for(int i = 0; environ[i]!=0; i++)
                {
                    printf("%s\n", environ[i]);
                }
                break;

            case 'V':
                putenv(optarg);
                break;

            case 'f':
                fflg++;
                f_ptr = optarg;
                break;
            case 'g':
                gflg++;
                g_ptr = optarg;
                break;
            case '?':
                printf("invalid option is %c\n", optopt);
                invalid++;
        }
    }
    printf("dflg equals %d\n", dflg);
    if(fflg)
        printf("f_ptr points to %s\n", f_ptr);
    if(gflg)
        printf("g_ptr points to %s\n", g_ptr);
        printf("invalid equals %d\n", invalid);
        printf("optind equals %d\n", optind);
    if(optind < argc)
        printf("next parameter = %s\n", argv[optind]);
}