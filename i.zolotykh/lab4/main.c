#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node {
    char* data;
    struct node* next;
} node;

node* create(char* input) {
    node* q;
    q = malloc(sizeof(struct node));
    q->data = malloc(strlen(input) + 1);
    strcpy(q->data, input);
    q->next = NULL;
    return q;
}


int main() {
    char line[BUFSIZ];
    node* head, * here, * p;
    head = (node*)malloc(sizeof(node));
    head->next = NULL;
    here = head;

    printf("Enter text:\n");
    while (gets(line) != NULL) {
        if (line[0] == '.') break;
        here->next = create(line);
        here = here->next;
    }

    printf("\nYour text:\n");
    for (p = head->next; p != NULL; p = p->next) {
        puts(p->data);
    }
}