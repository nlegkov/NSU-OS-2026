 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

struct termios old_settings;

typedef struct NodeLine {
    char *str;
    struct NodeLine *next;
    int len;
    struct NodeLine *old;
} NodeLine;

NodeLine *init_line(void) {
    NodeLine *line = malloc(sizeof(NodeLine));
    if (!line) return NULL;

    line->len = 0;
    line->next = NULL;
    line->str = malloc(41);
    if (!line->str) {
        free(line);
        return NULL;
    }
    line->old = NULL;
    line->str[0] = '\0';

    return line;
}

void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
}

void handle_signal(int sig) {
    (void)sig;
    restore_terminal();
    _exit(0);
}

int get_terminal_width(void) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) {
        return w.ws_col;
    }
    return 80;
}

void erase_one_char(int curr_len) {
    int term_width = get_terminal_width();

    if (curr_len > 0 && (curr_len % term_width == 0)) {
        printf("\033[1A\033[999C \b"); //подняться наверх сдвинуть в конец и затереть пробелом
    } else {
        printf("\b \b");
    } 
    fflush(stdout); //?
}

void free_lines(NodeLine *head) {
    while (head != NULL) {
        NodeLine *tmp = head;
        head = head->next;
        free(tmp->str);
        free(tmp);
    }
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

    NodeLine *line = init_line();

    NodeLine *head = line;
    NodeLine *tail = line;

    while (1) {
        int bytes_read = read(STDIN_FILENO, &ch, 1);

        if (bytes_read <= 0) {
            break;
        }

        if (ch == '\n' || ch == '\r' || ch == 27 || ch == old_settings.c_cc[VEOF]) {
            break;
        }

        if (ch == old_settings.c_cc[VKILL]) {
            while (tail != NULL) {
                while(tail->len > 0) {
                    erase_one_char(tail->len);
                    tail->len--;
                }
                tail->str[0] = '\0';

                if (tail->old != NULL) {
                    NodeLine *tmp = tail;
                    tail = tail->old;
                    tail->next = NULL;
                    free(tmp->str);
                    free(tmp);
                } else {
                    break;
                }
            }
        } else if (ch == old_settings.c_cc[VWERASE]) {
            while(tail != NULL && tail->len >0 && tail->str[tail->len - 1] == ' ') {
                erase_one_char(tail->len);
                tail->len--;
                tail->str[tail->len] = '\0';

                if (tail->len == 0 && tail->old != NULL) {
                    NodeLine *tmp = tail;
                    tail = tail->old;
                    tail->next = NULL;
                    free(tmp->str);
                    free(tmp);
                }
            }
            while(tail != NULL && tail->len >0 && tail->str[tail->len - 1] != ' ') {
                erase_one_char(tail->len);
                tail->len--;
                tail->str[tail->len] = '\0';

                if (tail->len == 0 && tail->old != NULL) {
                    NodeLine *tmp = tail;
                    tail = tail->old;
                    tail->next = NULL;
                    free(tmp->str);
                    free(tmp);
                }
            }
        } else if (ch == old_settings.c_cc[VERASE]) {
            if (tail->len >0) {
                erase_one_char(tail->len);
                tail->len--;
                tail->str[tail->len] = '\0';
            } else if (tail->old != NULL) {
                NodeLine *tmp = tail;
                tail = tail->old;
                tail->next = NULL;
                free(tmp->str);
                free(tmp);

                if (tail->len > 0) {
                    erase_one_char(tail->len);
                    tail->len--;
                    tail->str[tail->len] = '\0';
                }
            }
        } else if (ch == '\n' || ch == '\r') {
            putchar('\n');
            fflush(stdout);

            NodeLine *new_node = init_line();
            if (new_node) {
                tail->next = new_node;
                new_node->old = tail;
                tail = new_node;
            }
        } else if (ch >= 32 && ch <= 126) {
            if (tail->len >= 40) {
                NodeLine *new_node = init_line();
                if (new_node) {
                    tail->next = new_node;
                    new_node->old = tail;
                    tail = new_node;
                }
            }

            tail->str[tail->len++] = ch;
            tail->str[tail->len] = '\0';

            putchar(ch);
            fflush(stdout);
        }
    }

    printf("\nTerminal died.\n");

    free_lines(head);

    return 0;
}
