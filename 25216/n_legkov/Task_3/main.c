#include <stdio.h>
#include <unistd.h>

void check_file(const char *file) {
    printf("Real UID: %ld\n", (long)getuid());
    printf("Effective UID: %ld\n", (long)geteuid());

    FILE *file_f = fopen(file, "r");

    if (file_f == NULL) {
        perror("fopen");
    } else {
        printf("File opened successfully\n");
        fclose(file_f);
    }
}

int main(void) {
    check_file("data.txt");

    if (setuid(getuid()) == -1) {
        perror("setuid");
        return 1;
    }

    check_file("data.txt");

    return 0;
}
