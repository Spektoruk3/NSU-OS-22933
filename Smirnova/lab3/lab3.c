#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>

int func() {
    uid_t eff = geteuid();
    uid_t real = getuid();
    printf("Effective: %d\n Real:%d\n", eff, real);
    if (eff == real) return 1;
    else return 0;
}

int main()
{
    func();
    FILE* fp;
    const char* file = (char*)malloc(sizeof(char) * 10000);
    scanf("%s", &file[0]);

    fp = fopen(file, "r");
    if (fp == NULL) {
        perror("Erro0r: ");
        return(-1);
    }
    fclose(fp);
    func();
    setuid(getuid());
    func();
    fp = fopen(file, "r");
    if (fp == NULL) {
        perror("Error: ");
        return(-1);
    }
    fclose(fp);
    printf("Programm finished\n");
}