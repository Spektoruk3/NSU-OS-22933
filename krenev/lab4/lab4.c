#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	printf("Start to write all you want. Maximum length of string is 100 symbols. When you're done print .\n");
	int cap = 10;
	int cnt = 0;
	int max_len = 100;
	char** arr = (char**)malloc(cap*sizeof(char*));
	char* s = (char*)malloc(max_len*sizeof(char));
	s[0]='a';
	while(s[0]!='.'){
		fgets(s, max_len, stdin);
		if (cnt==cap){
			cap*=2;
			arr = (char**)realloc(arr, cap*sizeof(char*));
		}
		arr[cnt] = (char*)malloc(strlen(s)*sizeof(char));
		strcpy(arr[cnt], s);
		cnt++;
	}
	printf("End of input. Here all the strings:\n\n");\
	for(int i = 0; i<cnt; i++){
		printf("%s", arr[i]);
	}
	printf("END\n");
	return 0;
}
