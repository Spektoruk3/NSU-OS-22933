#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv){
	printf("Effective User ID = %d\n", geteuid());
    printf ("Real User ID = %d\n", getuid());
	printf("There is %d args\n", argc);

	if (argc == 2){
		FILE* file = fopen(argv[1], "r");
		if (NULL == file){ perror("Cant open the file"); return 1; }
		printf("Succsessfully oppened %s\n", argv[1]);
		
		fclose(file);
		
		setuid(geteuid());
		
		file = fopen(argv[1], "r");
                if (NULL == file){ perror("Cant open the file"); return 1; }
                printf("Succsessfully oppened again %s\n", argv[1]);
                fclose(file);
	}
	return 0;
}    
