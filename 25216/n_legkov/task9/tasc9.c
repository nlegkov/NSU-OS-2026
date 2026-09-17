#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

 main(int argc, char *argv[ ]) {
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) { 
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        execlp("cat", "cat", "file.txt", NULL);

        perror("exec error");
        exit(1);
    }

    wait(&status);


    printf("Process kill :)");
    
    return 0;
}