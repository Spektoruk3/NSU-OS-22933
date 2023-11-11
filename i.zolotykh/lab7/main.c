#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include <string.h>

#define BUFSIZE 512
#define ENTRYSTEP 10

struct LineEntry {
    off_t offset;
    ssize_t length;
};

void check(void* ptr) {
    if (ptr == NULL) {
        perror("very bad");
        exit(1);
    }
}

void print_line(char* start, struct LineEntry* entries, int linen) {
    char* lbuf = malloc(entries[linen].length + 1);
    check(lbuf);
    memcpy(lbuf, start + entries[linen].offset, entries[linen].length);
    lbuf[entries[linen].length] = '\0';
    printf("%s\n", lbuf);
    free(lbuf);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("no filename specified\n");
        return 1;
    }

    int tty_fd = open("/dev/tty", O_RDONLY);
    if (tty_fd == -1) {
        perror("/dev/tty");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror(argv[1]);
        return 1;
    }

    struct stat sbuf;
    if (fstat(fd, &sbuf) == -1) {
        perror("stat");
        return 1;
    }

    char* filestart;

    if ((filestart = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == (void*)(-1)) {
        perror("mmap");
        return 1;
    }

    int entrymaxn = 100;
    struct LineEntry* entries = malloc(entrymaxn * sizeof(struct LineEntry));
    check(entries);
    entries[0].offset = 0;
    int entryno = 0;

    for (off_t i = 0; i <= sbuf.st_size; i++) {
        if ((filestart[i] == '\n')
            || (i == sbuf.st_size)) {
            entries[entryno].length = i - entries[entryno].offset;
            entryno++;
            entries[entryno].offset = i + 1;
        }
        if (entryno == entrymaxn - 1) {
            entrymaxn += ENTRYSTEP;
            entries = realloc(entries, entrymaxn * sizeof(struct LineEntry));
            check(entries);
        }
    }

    for (int i = 0; i < entryno; i++) {
        printf("offt: %d len: %d\n", entries[i].offset, entries[i].length);
    }

    char buf[BUFSIZE];
    int linen;
    char* endptr;
    while (1) {
        printf("Enter line index (or zero to exit): \n");
        // sleep(5);
        int result = read(tty_fd, buf, BUFSIZE);
        if (result == -1) {
            perror("read error");
        }
        if (result == 0) {
            printf("Nothing entered, printing all lines:\n");
            for (int i = 0; i < entryno; i++) {
                print_line(filestart, entries, i);
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
            print_line(filestart, entries, linen);
        }
    };

    close(fd);
    close(tty_fd);
    munmap(filestart, sbuf.st_size);

    free(entries);
}