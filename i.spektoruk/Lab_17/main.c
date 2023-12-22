#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <ctype.h>
#include <unistd.h>
#define SIZE 40

char mas[SIZE+1];
char word[SIZE+2];
int main(){
	//setbuf(stdout, NULL);
	//printf("qwe\b");

	mas[SIZE] = '\0';
	struct termios newter;
	struct termios oldter;
        int in = fileno(stdin);
	if (in == -1){
		perror("error fileno");
		return 0;
	} 
	if (tcgetattr(in, &oldter) == -1){
		perror("error tcgetattr");
		return 0;
	}
	newter = oldter;
	newter.c_lflag &= ~(ICANON | ECHO);
	newter.c_cc[VMIN] = 1;
	if (tcsetattr(in, TCSAFLUSH, &newter)==-1){
		perror("error tcsetattr (setnew)");
		return 0;
	}
	setbuf(stdout, NULL);
	char c;
	int k=0;
	int flag;
	int wordk = 0;
	word[SIZE+1] = '\0';
        //read(in, &c, 1);
	//printf("%d", c);
	while(read(in, &c, 1)){
		if (c == 8){
			if (k > 0){
				k--;
				mas[k]=' ';
				printf("\b \b");
			}
			// add
		}
		else if (c==4 && k==0){
			printf("\n");
			break;
		}
		else if ( c == newter.c_cc[VKILL] ){
			while(k>0){
				k--;
				mas[k]=' ';
				printf("\b \b");
			}
		}
		else if (c == newter.c_cc[VWERASE]){
			if (k > 0){
				k--;
				while(k>=0 && mas[k] == ' '){
					k--;
					printf("\b");
				}
				while(k>=0 && mas[k] != ' '){
					mas[k]=' ';
					k--;
					printf("\b \b");
				}
				k++;
			}
		}
		else if (c=='\n'){
			printf("\n%s\n", mas);
			k=0;
			for (int i=0; i<SIZE; i++)
				mas[i]=' ';
		}
		else if(!isprint(c)){
			printf("\a");
		}
		//add
		else {
			if (k<SIZE){
				mas[k]=c;
				k++;
				printf("%c", c);
			}
			else{
				wordk=0;
				k--;
				while(k>=0 && mas[k]!=' '){
					word[SIZE-wordk-1] = mas[k];
					mas[k] = ' ';
					printf("\b \b");
					wordk++;
					k--;
				}
				if (k<0){
					word[SIZE]='\0';
					printf("%s\n%c", word, c);
					k=1;
					mas[0]=c; 
				}
				else{
					word[SIZE]=c;
					printf("\n%s", word+SIZE-wordk);
					k=wordk+1;
					for (int i = SIZE-wordk; i<=SIZE; i++)
						mas[i-(SIZE-wordk)] = word[i];
					for (int i=wordk+1; i<SIZE; i++)
						mas[i]=' ';
				}
			}
			
		}
	}
	if (tcsetattr(in, TCSAFLUSH, &oldter)==-1){
		perror("error tcsetattr (setold)");
		return 0;
	}
	return 0;
}