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
    sigset_t blocked, old_mask;

    sigemptyset(&blocked);
    sigaddset(&blocked, SIGINT);
    sigaddset(&blocked, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &blocked, &old_mask) == -1) {
        perror("sigprocmask");
        return 1;
    }

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

    if (sigaction(SIGQUIT, &sa2, NULL) == -1) {
        perror("sigaction SIGQUIT");
        return 1;
    }

    sigset_t wait_mask = old_mask;
    sigdelset(&wait_mask, SIGINT);
    sigdelset(&wait_mask, SIGQUIT);

    while (!stop)
    {
        sigsuspend(&wait_mask);
    }

    printf("\nВведено сигинтов: %d\n", (int)count);

    return 0;
    
}

