
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

typedef struct {
    off_t offset;
    size_t length;
} Line;

int main(void) {
    char file_name[] = "text.txt";

    size_t len_list = 16;
    Line *list_lseek_size = malloc(sizeof(Line) * len_list);

    if (list_lseek_size == NULL) {
        perror("malloc");
        return 1;
    }

    int file = open(file_name, O_RDONLY);

    if (file == -1) {
        perror("open");
        free(list_lseek_size);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    size_t index = 0;
    size_t size = 0;
    off_t position = 0;

    while ((bytes_read = read(file, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            size++;

            if (buffer[i] == '\n') {
                if (index == len_list) {
                    len_list *= 2;

                    Line *temp = realloc(
                        list_lseek_size,
                        len_list * sizeof(Line)
                    );

                    if (temp == NULL) {
                        perror("realloc");
                        free(list_lseek_size);
                        close(file);
                        return 1;
                    }

                    list_lseek_size = temp;
                }

                list_lseek_size[index].offset = position + i + 1 - size;
                list_lseek_size[index].length = size;
                index++;

                size = 0;
            }
        }

        position += bytes_read;
    }

    if (bytes_read == -1) {
        perror("read");
        free(list_lseek_size);
        close(file);
        return 1;
    }

    if (size > 0) {
        if (index == len_list) {
            len_list *= 2;

            Line *temp = realloc(
                list_lseek_size,
                len_list * sizeof(Line)
            );

            if (temp == NULL) {
                perror("realloc");
                free(list_lseek_size);
                close(file);
                return 1;
            }

            list_lseek_size = temp;
        }

        list_lseek_size[index].offset = position - size;
        list_lseek_size[index].length = size;
        index++;
    }

    while (1) {
        int n;

        printf("Введите номер строки: ");

        if (scanf("%d", &n) != 1) {
            printf("Некорректный ввод\n");
            break;
        }

        if (n == 0) {
            break;
        }

        if (n < 1 || (size_t)(n - 1) >= index) {
            printf("Такой строки нет\n");
            continue;
        }

        Line line = list_lseek_size[n - 1];

        if (lseek(file, line.offset, SEEK_SET) == (off_t)-1) {
            perror("lseek");
            break;
        }

        size_t remaining = line.length;

        while (remaining > 0) {
            size_t chunk = remaining < sizeof(buffer)
                         ? remaining : sizeof(buffer);

            ssize_t count = read(file, buffer, chunk);

            if (count <= 0) {
                if (count == -1) {
                    perror("read");
                } else {
                    fprintf(stderr, "Неожиданный конец файла\n");
                }
                break;
            }

            fwrite(buffer, 1, (size_t)count, stdout);
            remaining -= (size_t)count;
        }

        if (remaining > 0) {
            break;
        }
    }

    free(list_lseek_size);
    close(file);

    return 0;
}
