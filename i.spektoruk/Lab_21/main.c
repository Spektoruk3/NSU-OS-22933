#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int count = 0;

void handleSIGINT(int sig) {
    printf("\a");
    fflush(stdout);
    count++;
	signal(SIGINT, &handleSIGINT);
}

void handleSIGQUIT(int sig) {
    printf("\nThe signal sounded %d times.\n", count);
    exit(0);
}

int main() {
    signal(SIGINT, &handleSIGINT);
    signal(SIGQUIT, &handleSIGQUIT);

    while (1);
}
