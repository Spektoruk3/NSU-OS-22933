#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	printf("Start to write all you want. Maximum length of string is 100 symbols. When you're done print .\n");
	int remp = 10, count = 0, len_max = 100;
	char** mass = (char**)malloc(remp*sizeof(char*));
	char* strr = (char*)malloc(len_max*sizeof(char));
	strr[0]='a';
	while(s[0]!='.'){
		fgets(strr, len_max, stdin);
		if (count==remp){
			remp*=2;
			mass = (char**)realloc(mass, remp*sizeof(char*));
		}
		mass[count] = (char*)malloc(strlen(s)*sizeof(char));
		strcpy(mass[count], strr);
		count++;
	}
	printf("End of input. Here all the strings:\n");
	printf("\n");

	for(int i = 0; i<count; i++){
		printf("%s", mass[i]);
	}
	
	printf("END");
	printf("\n");
	return 0;
}
