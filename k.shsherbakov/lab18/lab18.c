#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <libgen.h>

char* format_permissions(mode_t mode) {
    char* permissions = (char*)malloc(10 * sizeof(char));
    permissions[0] = (S_ISDIR(mode)) ? 'd' : (S_ISREG(mode)) ? '-' : '?';
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';
    return permissions;
}

char* get_owner_name(uid_t uid) {
    struct passwd* pwd = getpwuid(uid);
    if (pwd != NULL) {
        return pwd->pw_name;
    }
    return "";
}

char* get_group_name(gid_t gid) {
    struct group* grp = getgrgid(gid);
    if (grp != NULL) {
        return grp->gr_name;
    }
    return "";
}

void format_file_info(const char* path) {
    struct stat stat_info;
    if (lstat(path, &stat_info) != 0) {
        printf("Error: Failed to get file information for %s\n", path);
        return;
    }

    char* permissions = format_permissions(stat_info.st_mode);
    nlink_t link_count = stat_info.st_nlink;
    char* owner_name = get_owner_name(stat_info.st_uid);
    char* group_name = get_group_name(stat_info.st_gid);
    off_t size = (S_ISREG(stat_info.st_mode)) ? stat_info.st_size : 0;
    char modification_time[20];
    strftime(modification_time, sizeof(modification_time), "%Y-%m-%d %H:%M:%S", localtime(&stat_info.st_mtime));
    char* file_name = basename(path);

    printf("%s  %lu  %s  %s  %lld  %s  %s\n", permissions, link_count, owner_name, group_name, size, modification_time, file_name);

    free(permissions);
}
// пример запуска ./ls_ld test.txt testDir/
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: ./ls_ld <file1> [<file2> ...]\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        format_file_info(argv[i]);
    }

    return 0;
}