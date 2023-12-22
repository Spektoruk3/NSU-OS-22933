#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#define BUF_LEN 5000000

typedef struct File{
    unsigned int lines;
    unsigned int arrLen;
    unsigned long long lastPos;
    unsigned long long *stringPositions;
} File;

void timeoutAlarm(){
    printf("program was inactive for 5 seconds and it has been killed");
    exit(0);
}

void addToArr(unsigned long long *arr, unsigned int *pos, unsigned int *len, unsigned long long n){
    if(*pos >= *len){
        *len += 20;
        arr = (unsigned long long*) realloc(arr, sizeof(unsigned long long) * (*len));
    }
    arr[*pos] = n;
    *pos += 1;
}

void findLines(const char *buf, File *file, int maxLen){
//    while(buf[i] != '\000') {
    for(int i = 0; i < maxLen; i++)
        if (buf[i] == '\n')
            addToArr(file->stringPositions, &file->lines, &file->arrLen, i + file->lastPos);
    file->lastPos += maxLen;
}


int main(int argc, char *argv[])
{
    char inp[50];
    printf("Enter input file name below:");
    scanf("%s", inp);
    File file;
    file.lines = 0;
    file.arrLen = 20;
    file.lastPos = 0;
    file.stringPositions = (unsigned long long*) calloc(20, sizeof(unsigned long long));
    int fd = open(inp, O_RDONLY);
    if (fd == -1)
    {
        perror("File opening error");
        exit(1);
    }
    char *buf = (char*) calloc(BUF_LEN + 1, sizeof(char));
    int ret = read(fd, buf, BUF_LEN);
    while(ret > 0)
    {
        findLines(buf, &file, ret);
        ret = read(fd, buf, BUF_LEN);
    }
    int strNum = 1;
    while(strNum != 0)
    {
        signal(SIGALRM, timeoutAlarm);
        alarm(5);
        close(fd);
        printf("\n|Enter number of string, you want to see|\n");
        scanf("%d", &strNum);
        if(strNum < 0 || strNum > file.lines + 1)
        {
            fprintf(stderr, "String index out of range");
            continue;
        }
        unsigned long long resLen = strNum == 1 ? file.stringPositions[0]: file.stringPositions[strNum - 1] - file.stringPositions[strNum - 2] - 1;
        if(strNum > file.lines) resLen = BUF_LEN;
        if(strNum > 1 && file.stringPositions[strNum - 1] - file.stringPositions[strNum - 2] <= 1) continue;
        long offset = strNum == 1 ? 0 : (long) file.stringPositions[strNum - 2];
        offset += strNum == 1 ? 0 : strNum;
        free(buf);
        buf = (char*) calloc(resLen + 1, sizeof(char));
        fd = open(inp, O_RDONLY);
        lseek(fd, offset, SEEK_SET);
        read(fd, buf, resLen);
        printf("%s", buf);

    }
    return 0;
}