#include <sys/types.h>          
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


char socket_path[] = "socket";
int MY_BUF_SIZE = 2;


int main(int argc, char *argv[]) {

	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("socket goes wrong");
		exit(0);
	}
	
	struct sockaddr_un addr;

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
		
	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    	perror("connect error");
    	exit(-1);
  	}
	
	char buf[MY_BUF_SIZE];
	
	int rc = read(STDIN_FILENO, buf, sizeof(buf));
	while (rc > 0) {
		int cntWrited = 0;
		while (rc != 0) {
			int wc = write(fd, buf + cntWrited, rc);
			if (wc == -1) {
				perror("write goes wrong");
        		exit(0);
			}
			rc -= wc;
			cntWrited += wc;
		}
		rc = read(STDIN_FILENO, buf, sizeof(buf));
	}
	
	if (rc == -1) {
		perror("read goes wrong");
        exit(0);
	}

	exit(0);		
}
