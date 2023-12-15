#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define ERASE write(1, "\b \b", 3)

int text_editor(struct termios settings, int first_str){
    char line[41];
    int pos = 0;
    while (read(0, &line[pos], 1) == 1) {
        if (line[pos] == settings.c_cc[VERASE]) {
            if (first_str == 0 && pos == 0) {
                return 0;
            }
            else if (pos > 0){
                ERASE;
                --pos;
            }
        }
        else if (line[pos] == settings.c_cc[VKILL]) {
            while (pos > 0) {
                ERASE;
                --pos;
            }
        }
        else if (line[pos] == settings.c_cc[VWERASE]) {
            while (pos > 0 && !isspace(line[pos - 1])) {
                ERASE;
                --pos;
            }
            while (pos > 0 && isspace(line[pos - 1])) {
                ERASE;
                --pos;
            }
        }
        else if (pos == 0 && line[pos] == settings.c_cc[VEOF]) {
            return 1;
        }
        else if (!isprint(line[pos]))
            write(1, "\a", 1);
        else if (pos == 40) {
            write(1, "\n", 1);
            ERASE;
            if (text_editor(settings, 0) == 1) return 1;
            int k = 0;
            while (pos > 0 && !isspace(line[pos - 1])) {
                ERASE;
                --pos;
            }
            for (int i = pos; i < 40; ++i) {
                write(1, &line[i], 1);
                ++k;
            }
            pos = k;
        }
        else {
            write(1, &line[pos], 1);
            pos++;
        }
    }
    return 0;
}

int main() {
    struct termios settings;
    struct termios saved_settings;
    tcgetattr(0, &settings);
    memcpy(&saved_settings, &settings, sizeof(saved_settings));
    saved_settings = settings;
    settings.c_lflag &= ~(ICANON | ECHO);
    settings.c_cc[VMIN] = 1;
    settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &settings);

    text_editor(settings, 1);

    tcsetattr(0, TCSANOW, &saved_settings);
    return 0;
}