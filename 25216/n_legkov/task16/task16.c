#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

static struct termios old_settings;
static int fl_save_setting = 0;

void restart_terminal(void) {
    if (fl_save_setting) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    }
}

void signal_handler(int sig) {
    restart_terminal();
    _exit(128 + sig);
}

int main(void) {
    struct termios new_settings;
    char ch;

    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "error: stdin no termenal\n");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &old_settings) != 0) {
        perror("error tcgetattr");
        return 1;
    }

    fl_save_setting = 1;
    atexit(restart_terminal);

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

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

    if (ch == 'm' || ch == 'M') {
        printf("\nYou're a man!\n");
    } else if (ch == 'w' || ch == 'W') {
        printf("\nYou're a woman!\n");
    } else {
        printf("\nAre you a cat?\n");
    }

    return 0;
}