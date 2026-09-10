#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int main(void) {
    struct termios old_settings, new_settings;
    char ch;

    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "error: stdin no termenal\n");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &old_settings) != 0) {
        perror("error tcgetattr");
        return 1;
    }

    new_settings = old_settings;

    new_settings.c_lflag &= ~ICANON;

    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_settings) != 0) {
        perror("error tcsetattr");
        return 1;
    }

    printf("Are you a man or a woman? (m/w): ");
    fflush(stdout);
    
    if (read(STDIN_FILENO, &ch, 1) < 1) {
        ch = '\0';
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

    if (ch == 'm' || ch == 'M') {
        printf("\nYou're a man!\n");
    } else if (ch == 'w' || ch == 'W') {
        printf("\nYou're a woman!\n");
    } else {
        printf("\nAre you a cat?\n");
    }

    return 0;
}