#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024

typedef struct Node {
    char *str;
    struct Node *next;
} Node;

int main() {
    char buf[MAX_LEN];
    Node *head = NULL;
    Node *tail = NULL;

    int fl_line = 1;

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        if (fl_line && buf[0] == '.') {
            break;
        }

        int l = strlen(buf);

        if (fl_line) {
                Node *node = malloc(sizeof(Node));
            if (node == NULL) {
                perror("Error from malloc in node");
                return 1;
            }

            node->str = malloc(l + 1);
            if (node->str == NULL) {
                perror("Error to malloc for node.str");
                free(node);
                return 1;
            }

            strcpy(node->str, buf);
            node->next = NULL;

            if (head == NULL) {
                head = node;
                tail = node;
            } else {
                tail->next = node;
                tail = node;
            }м
        }
        else {
            int old_len = strlen(tail->str);
            char *new_str = realloc(tail->str, old_len + l + 1);
            if (!new_str) return 1;

            tail->str = new_str;
            strcpy(tail->str + old_len, buf);
        }

        if (l > 0 && buf[l - 1] == '\n') {
            fl_line = 1;
        } else {
            fl_line = 0; 
        }

        
    }

    Node *curr = head;
    while (curr != NULL) {
        printf("%s", curr->str);
        curr = curr->next;
    }

    curr = head;
    while (curr != NULL) {
        Node *tmp = curr;
        curr = curr->next;
        free(tmp->str);
        free(tmp);
    }

    return 0;
}