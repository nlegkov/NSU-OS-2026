#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>


int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Error from pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Error from fork");
        return 1;
    }

    if (pid == 0) {
        close(pipefd[1]);
        
        while (1) {
            char buf[1024];
            ssize_t bytesRead = read(pipefd[0], buf, sizeof(buf));
            if (bytesRead == -1) {
                perror("Error from read");
                exit(1);
            }

            if (bytesRead == 0) {
                close(pipefd[0]);
                break;
            }

            for (ssize_t i = 0; i < bytesRead; i++) {
                buf[i] = toupper((unsigned char)buf[i]);
            }
            
            ssize_t bytesWritten = write(STDOUT_FILENO, buf, bytesRead);
            if (bytesWritten == -1) {
                perror("Error from write");
                exit(1);
            }
        }

    } else {
        close(pipefd[0]);
        
        while(1) {
            char buf[1024];
            ssize_t bytesRead = read(STDIN_FILENO, buf, sizeof(buf));
            if (bytesRead == -1) {
                perror("Error from read");
                exit(1);
            }

            if (bytesRead == 0) {
                close(pipefd[1]);
                break;
            }

            ssize_t bytesWritten = write(pipefd[1], buf, bytesRead);
            if (bytesWritten == -1) {
                perror("Error from write");
                exit(1);
            }
        }

        waitpid(pid, NULL, 0);

    }

    return 0;
}