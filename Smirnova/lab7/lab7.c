#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

int timed_out = 0;

void timeout_handler(int signum) {
    timed_out = 1;
}

int main() {
    char filename[] = "file.txt";
    int fd, line_num, i;
    off_t *offsets;
    int *line_lengths;
    char *file_data;

    fd = open(filename, O_RDONLY);

    off_t file_size = lseek(fd, 0, SEEK_END);

    file_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

    int num_lines = 0;
    offsets = (off_t *)malloc(file_size * sizeof(off_t));
    line_lengths = (int *)malloc(file_size * sizeof(int));

    offsets[num_lines] = 0;

    for (i = 0; i < file_size; i++) {
        if (file_data[i] == '\n') {
            line_lengths[num_lines] = i - offsets[num_lines];
            num_lines++;
            offsets[num_lines] = i + 1;
        }
    }

    signal(SIGALRM, timeout_handler);

    while (1) {
        printf("String number: ");

        timed_out = 0;
        alarm(5);

        scanf("%d", &line_num);

        if (timed_out) {
            printf("Time is out:\n");

            for (i = 0; i < file_size; i++) {
                printf("%c", file_data[i]);
            }

            break;
        }

        if (line_num == 0) {
            break;
        }

        if (line_num < 0 || line_num > num_lines) {
            printf("Wrong number\n");
            continue;
        }

        for (i = offsets[line_num - 1]; i < offsets[line_num - 1] + line_lengths[line_num - 1]; i++) {
            printf("%c", file_data[i]);
        }
        printf("\n");
    }

    close(fd);
    munmap(file_data, file_size);

    return 0;
}