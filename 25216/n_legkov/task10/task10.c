#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char *argv[ ]) {
    pid_t pid;
    int status;

    if (argc < 2) {
        printf("Передано недостаточно аргументов\n");
        exit(1);
    }

    if ((pid = fork()) < 0) { 
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        execvp(argv[1], &argv[1]);

        perror("exec error");
        exit(1);
    }

    wait(&status);

    if (WIFEXITED(status)) {
        printf("%d\n", WEXITSTATUS(status));
    }
    
    return 0;
}