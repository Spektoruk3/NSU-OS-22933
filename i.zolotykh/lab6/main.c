#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/select.h>
#include <poll.h>

#define BUFSIZE 512
#define ENTRYSTEP 10

void check(void* ptr) {
    if (ptr == NULL) {
        perror("very bad");
        exit(1);
    }
}

struct LineEntry {
    off_t offset;
    ssize_t length;
};

void print_line(int fd, struct LineEntry* entries, int linen) {
    lseek(fd, entries[linen].offset, SEEK_SET);
    char* lbuf = malloc(entries[linen].length + 1);
    check(lbuf);
    read(fd, lbuf, entries[linen].length);
    lbuf[entries[linen].length] = '\0';
    printf("%s\n", lbuf);
    free(lbuf);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("no filename specified\n");
        return 1;
    }

    int tty_fd = open("/dev/tty", O_RDONLY | O_NDELAY);
    if (tty_fd == -1) {
        perror("/dev/tty");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror(argv[1]);
        return 1;
    }

    int entrymaxn = 100;

    off_t total = 0;
    ssize_t scanned;
    char buf[BUFSIZE];
    struct LineEntry* entries = malloc(entrymaxn * sizeof(struct LineEntry));
    check(entries);
    entries[0].offset = 0;
    int entryno = 0;
    while (scanned = read(fd, buf, BUFSIZE)) {
        for (ssize_t i = 0; i < scanned; i++) {
            if ((buf[i] == '\n')
                || (scanned < BUFSIZE && i == scanned - 1)) {
                entries[entryno].length = total + i - entries[entryno].offset;
                entryno++;
                entries[entryno].offset = total + i + 1;
            }
            if (entryno == entrymaxn - 1) {
                entrymaxn += ENTRYSTEP;
                entries = realloc(entries, entrymaxn * sizeof(struct LineEntry));
                check(entries);
            }
        }
        total += scanned;
    }

    for (int i = 0; i < entryno; i++) {
        printf("offt: %d len: %d\n", entries[i].offset, entries[i].length);
    }

    int linen;
    char* endptr;

    fd_set set;
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    struct pollfd pollarg;
    pollarg.fd = tty_fd;
    pollarg.events = POLLIN;

    while (1) {
        printf("Enter line index in 5 seconds (or zero to exit): \n");
        int res = poll(&pollarg, 1, 5 * 1000);
        if (res == -1) {
            perror("select or poll");
        }
        else if (res == 0) {
            printf("Nothing entered, printing all lines:\n");
            for (int i = 0; i < entryno; i++) {
                print_line(fd, entries, i);
            }
            break;
        }
        else {
            printf("pollarg.revents: %d\n", pollarg.revents);
            if (pollarg.revents & POLLERR) {
                fprintf(stderr, "very bad, error\n");
                continue;
            }
            int read_result = read(tty_fd, buf, BUFSIZE);
            if (read_result == 0) {
                printf("Nothing entered, printing all lines:\n");
                for (int i = 0; i < entryno; i++) {
                    print_line(fd, entries, i);
                }
                break;
            }
            linen = strtoul(buf, &endptr, 10);
            if ((linen > entryno || linen < 0) || (endptr == buf)) {
                printf("invalid line index\n");
            }
            else {
                if (linen == 0) {
                    break;
                }
                linen--;
                print_line(fd, entries, linen);
            }
        }
    };

    close(fd);
    close(tty_fd);

    free(entries);
}