#define _CRT_SECURE_NO_WARNINGS
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char* tzname[];

int main()
{
    time_t now;
    struct tm* sp;

    tzset();
    if (time(&now) == -1) {
        perror("Program crashed. Time error: ");
        exit(2);
    }

    if (ctime(&now) == NULL) {
        perror("Program crashed. Ctime error: ");
        exit(3);
    } else printf("%s", ctime(&now));

    sp = localtime(&now);

    if (sp == NULL) {
        perror("Program crashed. Sp error: ");
        exit(4);
    }
    else {
        printf("%d/%d/%02d %d:%02d %s\n",
            sp->tm_mon + 1, sp->tm_mday,
            sp->tm_year - 100, sp->tm_hour,
            sp->tm_min, tzname[sp->tm_isdst]);
    }
    
    exit(0);
}