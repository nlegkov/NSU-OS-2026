 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <ctype.h>

struct termios old_settings;
static int printed_lines = 0;

void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
}

void handle_signal(int sig) {
    (void)sig;
    restore_terminal();
    _exit(0);
}

void redraw_line(const char *buf, int len) {
    for (int i = 0; i < printed_lines; i++) {
        printf("\r\033[2K\033[1A"); 
    }
    printf("\r\033[2K"); 

    printed_lines = 0;
    int col = 0;

    for (int i = 0; i < len; i++) {
        if (buf[i] != ' ' && (i == 0 || buf[i - 1] == ' ')) {
            int word_len = 0;
            while (i + word_len < len && buf[i + word_len] != ' ') {
                word_len++;
            }
            if (col > 0 && col + word_len > 40) {
                putchar('\n');
                printed_lines++;
                col = 0;
            }
        }

        putchar(buf[i]);
        col++;
    }    
    fflush(stdout);
}

int main(void) {
    struct termios new_settings;
    char ch;
    char buf[41];
    int len = 0;

    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "error: stdin no terminal\n");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &old_settings) != 0) {
        perror("error tcgetattr");
        return 1;
    }

    atexit(restore_terminal);

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    new_settings = old_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_settings) != 0) {
        perror("error tcsetattr");
        return 1;
    }

    buf[0] = '\0';

    while (1) {
        if (read(STDIN_FILENO, &ch, 1) <= 0) {
            break;
        }

        if (ch == old_settings.c_cc[VEOF]) {
            if (len == 0) {
                break;
            } else {
                putchar('\a');
                fflush(stdout);
                continue;
            }
        }

        if (ch == old_settings.c_cc[VERASE] || ch == 127 || ch == '\b') {
            if (len > 0) {
                len--;
                buf[len] = '\0';
                redraw_line(buf, len);
            } else {
                putchar('\a');
                fflush(stdout);
            } 
        } else if (ch == old_settings.c_cc[VKILL]) {
            len = 0;
            buf[0] = '\0';
            redraw_line(buf, len);
        } else if (ch == old_settings.c_cc[VWERASE]) {
            while (len > 0 && buf[len - 1] == ' ') {
                len--;
            } 
            while (len > 0 && buf[len - 1] != ' ') {
                len--;
            } 

            buf[len] = '\0';
            redraw_line(buf, len);
        } else if (ch == '\n' || ch == '\r') {
            putchar('\n');
            len = 0;
            buf[0] = '\0';
            printed_lines = 0;
            fflush(stdout);
        } else if (isprint(ch)) {
            if (len < 40) {
                buf[len++] = ch;
                buf[len] = '\0';
                redraw_line(buf, len);
            } else {
                putchar('\a');
                fflush(stdout);
            }
        } else {
            putchar('\a');
            fflush(stdout);
        }
    }

    printf("\nTerminal died.\n");
    return 0;
}
