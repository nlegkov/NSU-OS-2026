#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t count = 0;
volatile sig_atomic_t stop = 0;

void handler(int signo) {
    count++;
    const char bell = '\a';
    write(STDOUT_FILENO, &bell, 1);
}

void quit_handler(int signo) {
    stop = 1;
}

int main() {
    struct sigaction sa = {0};

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction SIGINT");
        return 1;
    }
    struct sigaction sa2 = {0};

    sa2.sa_handler = quit_handler;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;

    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("sigaction SIGQUIT");
        return 1;
    }

    while (1)
    {
        if (stop == 1) {
            printf("\nВведено сигинтов: %d\n", count);
            return 0;
        }
    }

    return 0;
    
}