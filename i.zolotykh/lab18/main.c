#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <libgen.h>
#include <limits.h>

#define MAX_DATE_LEN 100

void print_formatted_date(time_t date) {
    char timestr[MAX_DATE_LEN];
    struct tm tm_file = *localtime(&date);
    #ifdef IRTEGOV
           strftime(timestr, MAX_DATE_LEN, "%c", &tm_file);
    #else
        time_t t = time(NULL);
        struct tm tm_cur = *localtime(&t);
        // possible approximation
        if (abs(t - date) <= 60 * 60 * 24 * 180) {
        // if (tm_cur.tm_year == tm_file.tm_year) {
            strftime(timestr, MAX_DATE_LEN, "%b %e %H:%M", &tm_file);
        } else {
            strftime(timestr, MAX_DATE_LEN, "%b %e  %Y", &tm_file);
        }
    #endif
    printf("%s", timestr);
}

void list_fn(char* name) {
    struct stat statbuf;
    if (lstat(name, &statbuf) == -1) {
        perror(name);
        return;
    }
    
    char mode[20];

    int i = 0;

    if (S_ISREG(statbuf.st_mode)) {
        mode[i] = '-';
    } else if (S_ISDIR(statbuf.st_mode)) {
        mode[i] = 'd';
    } else if (S_ISLNK(statbuf.st_mode)) {
        mode[i] = 'l';
    } else {
        mode[i] = '?';
    }
    i++;

    mode[i] = (statbuf.st_mode & S_IRUSR) ? 'r' : '-'; i++;
    mode[i] = (statbuf.st_mode & S_IWUSR) ? 'w' : '-'; i++;
    mode[i] = (statbuf.st_mode & S_IXUSR) ? 'x' : '-'; i++;

    mode[i] = (statbuf.st_mode & S_IRGRP) ? 'r' : '-'; i++;
    mode[i] = (statbuf.st_mode & S_IWGRP) ? 'w' : '-'; i++;
    mode[i] = (statbuf.st_mode & S_IXGRP) ? 'x' : '-'; i++;

    mode[i] = (statbuf.st_mode & S_IROTH) ? 'r' : '-'; i++;
    mode[i] = (statbuf.st_mode & S_IWOTH) ? 'w' : '-'; i++;
    mode[i] = (statbuf.st_mode & S_IXOTH) ? 'x' : '-'; i++;

    mode[i] = '\0';

    printf("%s %3d", mode, statbuf.st_nlink);

    struct passwd *passwd_ptr = getpwuid(statbuf.st_uid);
    if (passwd_ptr == NULL) {
        printf(" %-8d", statbuf.st_uid);
    } else {
        printf(" %-8s", passwd_ptr->pw_name);
    }

    struct group *group_ptr  = getgrgid(statbuf.st_gid);
    if (group_ptr == NULL) {
        printf(" %-8d", statbuf.st_gid);
    } else {
        printf(" %-8s", group_ptr->gr_name);
    }
    
    #ifdef IRTEGOV
        if (S_ISREG(statbuf.st_mode)) {
            printf("\t%8d ", statbuf.st_size);
        } else {
            printf("\t         ", statbuf.st_size);
        }
    #else
        printf("%8d ", statbuf.st_size);
    #endif

    print_formatted_date(statbuf.st_mtime);

    #ifdef IRTEGOV
        printf(" %s", basename(name));
    #else
        printf(" %s", name);
        if (S_ISLNK(statbuf.st_mode)) {
            char buf[PATH_MAX];
            int read = readlink(name, buf, PATH_MAX);
		    buf[read] = '\0';
            printf(" -> %s", buf);
        }
    #endif

    printf("\n");
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        list_fn(".");
    } else {
        for (int i = 1; i < argc; i++) {
            list_fn(argv[i]);
        }
    }
    return 0;
}