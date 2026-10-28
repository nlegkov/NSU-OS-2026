#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    off_t offset;
    size_t length;
} Line;

int main() {
    char file_name[] = "text.txt";
    
    int len_list = 16;
    Line *list_lseek_size = malloc(sizeof(Line) * len_list);

    int file = open(file_name, O_RDONLY);

    if (file == -1) {
        perror("open error");
        
        return 1;
    }

    char c;
    int index = 0;
    int size = 0;

    while (read(file, &c, 1) == 1) {
        if (c == '\n') {
            size++;

            if (index + 1 > len_list) {
                len_list *= 2;
                list_lseek_size = realloc(list_lseek_size, len_list * sizeof(Line));
            }

            off_t position = lseek(file, 0, SEEK_CUR);

            list_lseek_size[index].offset = position - size;
            list_lseek_size[index].length = size;
            index++;

            size = 0;
        }
        else {
            size++;
        }
    }

    if (size > 0) {
        if (index + 1 > len_list) {
            len_list *= 2;

            int *temp = realloc(
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

        off_t position = lseek(file, 0, SEEK_CUR);

        list_lseek_size[index].offset = position - size;
        list_lseek_size[index].length = size;
        index++;
    }

    while (1)
    {
        int n;

        if (scanf("%d", &n) != 1) {
            printf("Некорректный ввод\n");
            break;
        }

        if (n == 0) {
            free(list_lseek_size);
            close(file);
            return 0;
        }

        if (n < 1 || (n - 1) >= index / 2) {
            printf("Такой строки нет\n");
            continue;
        }
        
        int k = (n - 1) * 2;

        off_t start = list_lseek_size[k].offset;
        size_t size = list_lseek_size[k].length;

        lseek(file, start, SEEK_SET);

        char c;

        for (int i = 0; i < size; i++) {
            if (read(file, &c, 1) != 1) {
                break;
            }

            printf("%c", c);
        }
    }

    free(list_lseek_size);
    close(file);
    return 0;
}