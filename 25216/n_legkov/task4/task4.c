#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024

struct Node {
    char *str;
    struct Node *next;
}

void main() {
    char buf[MAX_LEN];
    Node *head = NULL;
    Node *tail = NULL;

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (buffer[0] == '.') {
            break;
        }

        int l = strlen(buffer);

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

        strcpy(node->str, buffer);

        if (head == NULL) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }

    node *node = head;
    while (node != NULL) {
        printf("&s", node->str);
        node = node->next;
    }

    node = head;
    while (node != NULL) {
        Node *tmp = node;
        node = node->next;
        free(tmp->str);
        free(tmp);
    }

    return 0;
}