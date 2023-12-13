#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_file_info(const char *filename){
	struct stat file_stat;
	if(stat(filename, &file_stat) == -1){
		perror("stat");
		return;
	}

	if(S_ISDIR(file_stat.st_mode)){
		printf("d");
	}
	else if(S_ISREG(file_stat.st_mode)){
		printf("-");
	}
	else{printf("?");}
	//Права доступа
	printf("%c%c%c", file_stat.st_mode & S_IRUSR ? 'r' : '-',
					 file_stat.st_mode & S_IWUSR ? 'w' : '-',
					 file_stat.st_mode & S_IXUSR ? 'x' : '-');
	printf("%c%c%c", file_stat.st_mode & S_IRGRP ? 'r' : '-',
					 file_stat.st_mode & S_IWGRP ? 'w' : '-',
					 file_stat.st_mode & S_IXGRP ? 'x' : '-');
	printf("%c%c%c", file_stat.st_mode & S_IROTH ? 'r' : '-',
					 file_stat.st_mode & S_IWOTH ? 'w' : '-',
					 file_stat.st_mode & S_IXOTH ? 'x' : '-');

	printf("%4ld", file_stat.st_nlink);

	struct passwd *owner = getpwuid(file_stat.st_uid);
	struct group *group = getgrgid(file_stat.st_gid);
	printf("%10s %10s", owner->pw_name, group->gr_name);

	if(S_ISREG(file_stat.st_mode)){
		printf("%10ld", file_stat.st_size);
	}
	else{
		printf("%10s", "");
	}
	char date_string[20];
	strftime(date_string, sizeof(date_string), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
	printf(" %s", date_string);
	printf(" %s\n", filename);
}

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	for(int i = 1; i < argc; i++){
		print_file_info(argv[i]);
	}
	return 0;
}