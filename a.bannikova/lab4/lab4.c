#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
	int max_len = 8191;
	int cap = 8191;
	int len = 0;

	char** arr = (char**)malloc(cap * sizeof(char*));
	char* str = (char*)malloc(max_len * sizeof(char));
	
	str[0] = 'a';
	
	while (str[0] != '.') {
		fgets(str, max_len, stdin);
		len += strlen(str);
		if (len > cap) {
			cap *= 2;
			arr = (char**)realloc(arr, cap * sizeof(char*));
		}
		arr[len] = (char*)malloc(strlen(str) * sizeof(char));
		strcpy(arr[len], str);
	
	}
	
	for (int i = 0; i < len; i++) {
		printf("%str", arr[i]);
	}
	
	return 0;
}