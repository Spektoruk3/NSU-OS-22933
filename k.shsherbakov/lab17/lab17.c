#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 40 

int main() {
    struct termios old_settings, new_settings;
    tcgetattr(0, &old_settings); 
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(0, TCSANOW, &new_settings); 

    char line[MAX_LINE_LENGTH + 1] = ""; 
    int line_length = 0; 

    while (1) {
        char c;
        if (read(0, &c, 1) == 1) { 
            if (c == 13) { 
                if (line_length > 0) {
                    printf("\n"); 
                    line[0] = '\0'; 
                    line_length = 0; 
                }
            }
            else if (c == 0x04 && line_length == 0) {
                break;
            }
            else if (c == 0x17) {
                if (line_length > 0) {
                    int i = line_length - 1;
                    while (i >= 0 && line[i] == ' ') {
                        i--; 
                    }
                    while (i >= 0 && line[i] != ' ') {
                        line[i] = ' ';
                        i--;
                    }
                    line_length = i + 1;
                    printf("\r%s", line);
                    fflush(stdout);
                }
            }
            else if (c == 0x15) { 
                line_length = 0; 
                line[0] = '\0'; 
                printf("\r%s", line);
                fflush(stdout);
            }
            else if (c == 0x08) {
                if (line_length > 0) {
                    line_length--;
                    line[line_length] = '\0'; 
                    printf("\b \b"); 
                    fflush(stdout);
                }
            }
            else if (c >= 0x20 && c <= 0x7E) { 
                if (line_length < MAX_LINE_LENGTH) {
                    line[line_length] = c; 
                    line_length++; 
                    line[line_length] = '\0';
                    printf("%c", c);
                    fflush(stdout);
                }
            }
            else {
                printf("%c", 0x07); 
                fflush(stdout);
            }
        }
    }

    tcsetattr(0, TCSANOW, &old_settings);
    return 0;
}
