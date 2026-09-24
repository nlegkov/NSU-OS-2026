#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

 main(int argc, char *argv[ ]) {
    pid_t pid;
    int status;

    if (argc < 2) {
        printf("Не хватает аргументов\n");
        exit(1);
    }

    if ((pid = fork()) < 0) { 
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        execlp("cat", "cat", argv[1], NULL);

        perror("exec error");
        exit(1);
    }

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid error");
        exit(1);
    }

    printf("Procces kill\n");
    
    return 0;
}