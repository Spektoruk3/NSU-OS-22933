#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
int main(int argc, char *argv[]){
	struct stat buf;
	struct passwd* user;
	struct group* group;
	int boolf;
	int boold;
        char *time;
	char mode[] = "-rw-rw-rw-";
	for(int i=1; i<argc; i++){
		if (-1 == stat(argv[i], &buf)){
			perror(argv[i]);
			//printf("%s: No such file or directory\n", argv[i]);
			continue;
		}		
		boolf=1;
		boold=1;
		if ((buf.st_mode & S_IFMT) == S_IFREG){
			mode[0]='-';
			boolf=0;
		}
		if ((buf.st_mode & S_IFMT) == S_IFDIR){
			mode[0]='d';
			boold=0;
		}
		if (boolf && boold)
			mode[0]='?';
		if (S_IRGRP & buf.st_mode) mode[1]='r'; else mode[1]='-';
		if (S_IWRITE & buf.st_mode) mode[2]='w'; else mode[2]='-';
		if (S_IEXEC & buf.st_mode) mode[3]='x'; else mode[3]='-';
		if (S_IRGRP & buf.st_mode) mode[4]='r'; else mode[4]='-';
		if (S_IWGRP & buf.st_mode) mode[5]='w'; else mode[5]='-';
		if (S_IXGRP & buf.st_mode) mode[6]='x'; else mode[6]='-';
		if (S_IROTH & buf.st_mode) mode[7]='r'; else mode[7]='-';
		if (S_IWOTH & buf.st_mode) mode[8]='w'; else mode[8]='-';
		if (S_IXOTH & buf.st_mode) mode[9]='x'; else mode[9]='-';
		printf("%s%4d ", mode, buf.st_nlink); 
		user = getpwuid(buf.st_uid);
		group = getgrgid(buf.st_gid);
		if (user != NULL)
			printf("%16s ", user->pw_name);
		else 
			printf("%16d ", buf.st_uid);
		if (group != NULL)
			printf("%16s ", group->gr_name);
		else
			printf("%16d ", buf.st_gid);
		if (mode[0]=='-')
			printf("%7d ", buf.st_size);
		else
			printf("        ");
                time = ctime(&buf.st_mtime);
                time[24]='\0';
		printf("%s ", time+4);
                //printf("aaa");
                printf("%s",argv[i]);
		printf("\n");
	}
	return 0;
}