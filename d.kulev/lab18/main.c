#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>


void print_file_info(const char *filename) {
    struct stat file_info = {0};
    if (lstat(filename, &file_info) == -1) {
        perror("Unable to get stat");
        return;
    }

    printf(S_ISDIR(file_info.st_mode) ? "d" : (S_ISREG(file_info.st_mode) ? "-" : "?"));

    printf((file_info.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_info.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_info.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_info.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_info.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_info.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_info.st_mode & S_IROTH) ? "r" : "-");
    printf((file_info.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_info.st_mode & S_IXOTH) ? "x" : "-");

    printf("   %5u", (unsigned int)file_info.st_nlink);

    char uid_str[16];
    char gid_str[16];

    struct passwd *owner = getpwuid(file_info.st_uid);
    struct group *group = getgrgid(file_info.st_gid);

    sprintf(uid_str, "%u", (unsigned int)file_info.st_uid);
    sprintf(gid_str, "%u", (unsigned int)file_info.st_gid);

    printf("  %16s  %16s", owner ? owner->pw_name : uid_str, group ? group->gr_name : gid_str);

    if (S_ISREG(file_info.st_mode)) {
        printf("  %12jd", (intmax_t)file_info.st_size);
    } else {
        printf("              ");
    }

    struct tm *tm = localtime(&file_info.st_mtime);
    printf("  %04d-%02d-%02d %02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);

    char *last_slash = strrchr(filename, '/');
    printf("  %s\n", last_slash ? last_slash + 1 : filename);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Using: %s <file1> [<file2> ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Permissions  Links       Owner             Group            Size       Last Modified          File      \n");
    printf("-----------  -----  ----------------  ----------------  ------------  ----------------  ----------------\n");

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}