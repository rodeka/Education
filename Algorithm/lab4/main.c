#include <stdio.h>
#include <stdlib.h>

#define QUEUE_SIZE 100

// Структура для кольцевой очереди
typedef struct {
    int data[QUEUE_SIZE];
    int head;
    int tail;
    int size;
} CircularQueue;

// Структура для узла красно-черного дерева
typedef enum { RED, BLACK } NodeColor;

typedef struct RBNode {
    int data;
    NodeColor color;
    struct RBNode *left, *right, *parent;
} RBNode;

// Функции для работы с кольцевой очередью
void initQueue(CircularQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->size = 0;
}

int isFull(CircularQueue *q) {
    return q->size == QUEUE_SIZE;
}

int isEmpty(CircularQueue *q) {
    return q->size == 0;
}

void enqueue(CircularQueue *q, int value) {
    if (!isFull(q)) {
        q->data[q->tail] = value;
        q->tail = (q->tail + 1) % QUEUE_SIZE;
        q->size++;
    } else {
        printf("Очередь переполнена!\n");
    }
}

int dequeue(CircularQueue *q) {
    if (!isEmpty(q)) {
        int value = q->data[q->head];
        q->head = (q->head + 1) % QUEUE_SIZE;
        q->size--;
        return value;
    } else {
        printf("Очередь пуста!\n");
        return -1;
    }
}

// Функции для коктейльной сортировки
void cocktailSort(int arr[], int n) {
    int swapped = 1;
    int start = 0;
    int end = n - 1;

    while (swapped) {
        swapped = 0;

        // Проход слева направо
        for (int i = start; i < end; i++) {
            if (arr[i] > arr[i + 1]) {
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swapped = 1;
            }
        }

        // Если ничего не поменялось, выходим
        if (!swapped) break;

        swapped = 0;
        end--;

        // Проход справа налево
        for (int i = end - 1; i >= start; i--) {
            if (arr[i] > arr[i + 1]) {
                int temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swapped = 1;
            }
        }

        start++;
    }
}

// Вспомогательные функции для красно-черного дерева
RBNode *createNode(int data) {
    RBNode *node = (RBNode *)malloc(sizeof(RBNode));
    node->data = data;
    node->color = RED;
    node->left = node->right = node->parent = NULL;
    return node;
}

void rotateLeft(RBNode **root, RBNode *x) {
    RBNode *y = x->right;
    x->right = y->left;
    if (y->left != NULL) y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rotateRight(RBNode **root, RBNode *x) {
    RBNode *y = x->left;
    x->left = y->right;
    if (y->right != NULL) y->right->parent = x;

    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

void fixViolation(RBNode **root, RBNode *z) {
    while (z != *root && z->parent->color == RED) {
        RBNode *grandparent = z->parent->parent;
        if (z->parent == grandparent->left) {
            RBNode *y = grandparent->right;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(root, z);
                }
                z->parent->color = BLACK;
                grandparent->color = RED;
                rotateRight(root, grandparent);
            }
        } else {
            RBNode *y = grandparent->left;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(root, z);
                }
                z->parent->color = BLACK;
                grandparent->color = RED;
                rotateLeft(root, grandparent);
            }
        }
    }
    (*root)->color = BLACK;
}

void insertRBTree(RBNode **root, int data) {
    RBNode *z = createNode(data);
    RBNode *y = NULL;
    RBNode *x = *root;

    while (x != NULL) {
        y = x;
        if (z->data < x->data) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->parent = y;
    if (y == NULL) {
        *root = z;
    } else if (z->data < y->data) {
        y->left = z;
    } else {
        y->right = z;
    }

    fixViolation(root, z);
}

// Основная функция
int main() {
    CircularQueue queue;
    initQueue(&queue);

    FILE *file = fopen("numbers.txt", "r");
    if (!file) {
        printf("Не удалось открыть файл.\n");
        return 1;
    }

    int number;
    while (fscanf(file, "%d", &number) == 1) {
        enqueue(&queue, number);
    }
    fclose(file);

    // Коктейльная сортировка
    int sortedNumbers[QUEUE_SIZE];
    int i = 0;
    while (!isEmpty(&queue)) {
        sortedNumbers[i++] = dequeue(&queue);
    }
    cocktailSort(sortedNumbers, i);

    // Вставка в красно-черное дерево
    RBNode *rbTree = NULL;
    for (int j = 0; j < i; j++) {
        insertRBTree(&rbTree, sortedNumbers[j]);
    }

    printf("Сортировка завершена и результаты сохранены в красно-черное дерево.\n");
    return 0;
}
