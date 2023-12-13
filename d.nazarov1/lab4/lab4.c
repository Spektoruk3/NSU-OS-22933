#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node_s {
    char *string;
    struct Node_s *next;
} Node;

Node *first, *tail;

void init() {
    first =(Node*) malloc(sizeof(Node));
    first->string = NULL;
    first->next = NULL;
    tail = first;
}

void push(char *string) {
    unsigned long len = strlen(string) + 1;
    char *copyPtr = (char*) calloc(len, sizeof(char));
    strcpy(copyPtr, string);
    tail->string = copyPtr;
    tail->next =(Node*) calloc(1, sizeof(Node));
    tail = tail->next;
}

void printl() {
    Node *ptr = first;
    while (ptr != NULL) {
        if (ptr->string) {
            printf("%s\n", ptr->string);
        }
    ptr = ptr->next;
    }
}

int main() {
    char Inbuf[256] = {0};

    init();

    while (fgets(Inbuf, 256, stdin) != NULL) {
        if (Inbuf[0] == '.') {
            printl();
            return 0;
        }

        char *lineEnd = strchr(Inbuf, '\n');
        if (lineEnd == NULL) {
            int buf_ech = 0;
            int buf_zan = 256;
            char *buf = (char*) malloc(256);

            memcpy(buf, Inbuf, 256);
            buf_ech += 256 - 1;

            while (fgets(Inbuf, 256, stdin) != NULL) {
                buf_zan += 256;
                buf = (char*) realloc(buf, buf_zan);

                memcpy(buf + buf_ech, Inbuf, 256);
                buf_ech += 256 - 1;

                lineEnd = strchr(buf, '\n');
                if (lineEnd) {
                    *lineEnd = '\0';
                    push(buf);
                    free(buf);
                    break;
                }
            }
        }
        else {
            *lineEnd = '\0';
           push(Inbuf);
        }
    }

    return 0;
}