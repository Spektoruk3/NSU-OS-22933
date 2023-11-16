#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main() {
    uid_t ruid, euid;
    FILE *in;

    in = fopen("test.txt", "w+");
    if (in == NULL) {
        printf("Реальный идентификатор: %d\n", getuid());
        printf("Эффективный идентификатор: %d\n", geteuid());
        perror("File open error");
        exit(EXIT_FAILURE);
    }

    printf("Реальный идентификатор: %d\n", getuid());
    printf("Эффективный идентификатор: %d\n", geteuid());

    if (setuid(geteuid()) == -1) {
        perror("Ошибка при установке идентификатора пользователя");
        exit(EXIT_FAILURE);
    }

    printf("Реальный идентификатор: %d\n", getuid());
    printf("Эффективный идентификатор: %d\n", geteuid());

    fclose(in);

    return 0;
}