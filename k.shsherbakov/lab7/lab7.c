#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>

int fd; 
int lineNum = 0; 

void MyAlarm(int var)
{
    struct stat fileStat;
    fstat(fd, &fileStat);
    off_t fileSize = fileStat.st_size;
    char* fileData = (char*)mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    fwrite(fileData, sizeof(char), fileSize, stdout);
    _Exit(0);
}

int main() {
    char filename[100];
    printf("Enter file name: ");
    scanf("%s", filename);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    struct stat fileStat;
    fstat(fd, &fileStat);
    off_t fileSize = fileStat.st_size;
    char* fileData = (char*)mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);


    signal(SIGALRM, MyAlarm);
    alarm(5);

    while (1) {
        scanf("%d", &lineNum);
        alarm(0);
        if (lineNum < 1 || lineNum > fileSize) printf("Invalid line number\n");
        else if (lineNum > 0) {
            int lineStart = 0;
            int lineLength = 0;
            int currentlineNum = 1;
            off_t i;
            for (i = 0; i < fileSize; i++) {
                if (fileData[i] == '\n') {
                    if (currentlineNum == lineNum) {
                        lineLength = i - lineStart;
                        break;
                    }
                    currentlineNum++;
                    lineStart = i + 1;
                }
            }

            if (lineNum > currentlineNum) printf("Invalid line number\n");
            else {
                if (lineLength > 0) {
                    fwrite((void*)(fileData + lineStart), sizeof(char), lineLength, stdout);
                }
                printf("\n");
            }
        }
    }

    munmap(fileData, fileSize);
    close(fd);
    return 0;
}
