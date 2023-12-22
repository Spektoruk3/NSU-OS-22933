#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    printf("Real = %d\n", getuid());
    printf("Effective = %d\n", geteuid());

    FILE * file = fopen("file", "r");
    if (file == NULL)
    {
        perror("");
    } 
    else
    {
        printf("FINE\n");
        fclose(file);
    }                                                
    setuid(getuid());
    printf("Real = %d\n", getuid());
    printf("Effective = %d\n", geteuid());
    FILE * f = fopen("file", "r");
    if (f == NULL)
    {
	    perror("");
    }
    else
    {
	    puts("FINE");
        fclose(f);
    }
    return 0;
}