#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ulimit.h>
#include <sys/resource.h>

extern char** environ;

int main(int argc, char* argv[]) {
    char options[] = "ispuU:cC:dvV:";
    int command = 0;
    char buf[256];
    struct rlimit rlim;
    char** p;

    if (argc < 2)
    {
        printf("write arguments\n");
        exit(0);
    }

    while ((command = getopt(argc, argv, options)) != EOF) 
    {
        switch (command) 
        {
            case 's': 
            {
                setpgid(0, 0);
                break;
            }
            case 'i': 
            {
                printf("real userId: %d\n", getuid());
                printf("effect userId: %d\n", geteuid());
                printf("real groupId: %d\n", getgid());
                printf("effect groupId: %d\n", getegid());
                break;
            }
            case 'p': 
            {
                printf("curr procId: %d\n", getpid());
                printf("parent procId: %d\n", getppid());
                printf("group procId: %d\n", getpgid(0));
                break;
            }
            case 'u': 
            {
                printf("ulimit = %ld\n", ulimit(UL_GETFSIZE, 0));
                break;
            }
            case 'U': 
            {
                if (ulimit(UL_SETFSIZE, atol(optarg)) == -1)
                    printf("error\n");
                break;
            }
            case 'c': 
            {
                getrlimit(RLIMIT_CORE, &rlim);
                printf("core size = %ld\n", rlim.rlim_cur);
                break;
            }
            case 'C': 
            {
                getrlimit(RLIMIT_CORE, &rlim);
                rlim.rlim_cur = atol(optarg);
                if (setrlimit(RLIMIT_CORE, &rlim) == -1)
                {
                    printf("error");
                }
                else
                {
                    printf("core size = %ld\n", rlim.rlim_cur);
                }
                break;
            }
            case 'v': 
            {
                for (p = environ; *p; p++)
                {
                    printf("%s\n", *p);
                }
                break;
            }
            case 'V': 
            {
                putenv(optarg);
                for (p = environ; *p; p++)
                {
                    printf("%s\n", *p);
                }
                break;
            }
            case 'd': 
            {
                printf("curr dir: %s\n", getcwd(buf, 255));
                break;
            }
            default:
                printf("wrong command\n");
        }
    }
    return 0;
}