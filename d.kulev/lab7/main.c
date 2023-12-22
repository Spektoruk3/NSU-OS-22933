#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 100

struct LineInfo {
    off_t offset;
    int length;
};

void display(char* file_data, struct LineInfo* lines, int line_count) {
    for (int i = 0; i < line_count; i++) {
        printf("%.*s\n", lines[i].length, file_data + lines[i].offset);
    }
}

int main() {
    char filename[100];
    printf("Enter file name: ");
    scanf("%s", filename);
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("File opening error");
        exit(1);
    }
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("Getting file data error");
        exit(1);
    }
    off_t file_size = file_stat.st_size;
    char* file_data = (char*)mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("Mapping file to memory error");
        exit(1);
    }
    struct LineInfo lines[MAX_LINE_LENGTH];
    int line_count = 0;
    lines[0].offset = 0;
    for (int i = 0; i < file_size; i++) {
        if (file_data[i] == '\n') {
            lines[line_count].length = i - lines[line_count].offset;
            line_count++;
            lines[line_count].offset = i + 1;
        }
    }
    lines[line_count].length = file_size - lines[line_count].offset;
    line_count++;
    char input[10];
    int line_number;
    struct timeval start_time, current_time, elapsed_time;
    gettimeofday(&start_time, NULL);
    while (1) {
        printf("Enter line number (0 to exit): ");
        gettimeofday(&current_time, NULL);
        timersub(&current_time, &start_time, &elapsed_time);
        if (elapsed_time.tv_sec >= 5) {
            printf("Time is up.Outputting the components of the file:\n");
            display(file_data, lines, line_count);
            break;
        }
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Input error\n");
            continue;
        }
        if (sscanf(input, "%d", &line_number) != 1) {
            printf("Invalid input\n");
            continue;
        }

        if (line_number == 0) {
            break;
        }

        if (line_number < 0 || line_number > line_count) {
            printf("Invalid line number\n");
            continue;
        }

        printf("%.*s\n", lines[line_number - 1].length, file_data + lines[line_number - 1].offset);
    }
    munmap(file_data, file_size);
    close(fd);
    return 0;
}