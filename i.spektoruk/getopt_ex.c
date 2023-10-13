#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/resource.h>
#include <ulimit.h>
#include <unistd.h>
#include <string.h>

extern char* optarg;
extern char** environ;

void parse_commands(int argc, char *argv[], const char options[]) {
	char c = getopt(argc, argv, options);
	char* actual_optarg = optarg;
	struct rlimit rlim;
	if (c != EOF) {
		parse_commands(argc, argv, options);
                switch(c) {
                	case 'i':
                        	printf("uid=%u, euid=%u, gid=%u, egid=%u\n", getuid(), geteuid(), getgid(), getegid());
                        	break;
                	case 's':
                        	setpgid(getpid(), getpgid(getpid()));
                        	break;
                	case 'p':
                        	printf("pid=%d, ppid=%d, pgrp=%d\n", getpid(), getppid(), getpgrp());
                        	break;
                	case 'u':
                        	printf("max file size ulimit=%ld\n", ulimit(UL_GETFSIZE));
 	                	break;
        	        case 'U': 
			{
                	        long new_ulimit = atol(actual_optarg);
	                        ulimit(UL_SETFSIZE, new_ulimit);
			}
        	                break;
	                case 'c':
        	                getrlimit(RLIMIT_CORE, &rlim);
	                        printf("core-file size: %d\n", rlim.rlim_cur);
	                        break;
        	        case 'C':
                	        getrlimit(RLIMIT_CORE, &rlim);
	                        rlim.rlim_cur = atol(actual_optarg);
        	                setrlimit(RLIMIT_CORE, &rlim);
                	        break;
	                case 'd':
        	        {
	                        char cwd[PATH_MAX];
        	                if (NULL != getcwd(cwd, sizeof(cwd))) {
                	                printf("working dir: %s\n", cwd);
	                        } else {
        	                        perror("getcwd");
                	                exit(1);
	                        }
        	        }
	                        break;
        	        case 'v':
                	        while (NULL != *environ) {
                        	        printf("%s\n", *environ);
	                                ++environ;
        	                }
                	        break;
	                case 'V':
        	                putenv(actual_optarg);
                	        break;
	                case '?':
        	                printf("invalid option is %c\n", optopt); 	
                       		break;
	                case ':':
        	                printf("option %c without args\n", optopt);
                	        break;
		}
        }
}

int main(int argc, char *argv[]) {
	const char options[] = ":ispuU:cC:dvV:";
	parse_commands(argc, argv, options);

	return 0;
}
