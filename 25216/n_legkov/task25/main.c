#include <errno.h>
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
                if (errno == EINTR) {
                    continue;
                }

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

            ssize_t totalWritten = 0;

            while (totalWritten < bytesRead) {
                ssize_t bytesWritten = write(pipefd[1],
                                buf + totalWritten,
                                bytesRead - totalWritten);

                if (bytesWritten == -1) {
                    if (errno == EINTR) {
                        continue;
                    }

                    perror("Error from write");
                    exit(1);
                }

                totalWritten += bytesWritten;
            }
        }

    } else {
        close(pipefd[0]);
        
        while(1) {
            char buf[1024];
            ssize_t bytesRead = read(STDIN_FILENO, buf, sizeof(buf));
            if (bytesRead == -1) {
                if (errno == EINTR) {
                    continue;
                }

                perror("Error from read");
                exit(1);
            }

            if (bytesRead == 0) {
                close(pipefd[1]);
                break;
            }

            ssize_t totalWritten = 0;

            while (totalWritten < bytesRead) {
                ssize_t bytesWritten = write(pipefd[1],
                                buf + totalWritten,
                                bytesRead - totalWritten);

                if (bytesWritten == -1) {
                    if (errno == EINTR) {
                        continue;
                    }

                    perror("Error from write");
                    exit(1);
                }

                totalWritten += bytesWritten;
            }
        }

        while (waitpid(pid, NULL, 0) == -1) {
            if (errno == EINTR) {
                continue;
            }

            perror("Error from waitpid");
            exit(1);
        }

    }

    return 0;
}