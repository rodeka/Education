#include <stdio.h>
#include <stdlib.h>
#include "circularQueue.h"

// Функции для работы с кольцевой очередью
void initQueue(CircularQueue* q, int initialCapacity) {
    q->data = (int *)malloc(initialCapacity * sizeof(int));
    q->head = 0;
    q->tail = 0;
    q->size = 0;
    q->capacity = initialCapacity;
    return;
}

void resizeQueue(CircularQueue* q) {
    int newCapacity = q->capacity * 2;
    int* newData = (int*)malloc(newCapacity * sizeof(int));

    // Перенос элементов с учетом кольцевой структуры
    for (int i = 0; i < q->size; i++) {
        newData[i] = q->data[(q->head + i) % q->capacity];
    }

    free(q->data);
    q->data = newData;
    q->head = 0;
    q->tail = q->size;
    q->capacity = newCapacity;
    return;
}

void freeQueue(CircularQueue* q) {
    free(q->data);
    free(q);
    return;
}

int isFull(CircularQueue* q) {
    return q->size == q->capacity;
}

int isEmpty(CircularQueue* q) {
    return q->size == 0;
}

void enqueue(CircularQueue* q, int value) {
    if (isFull(q)) {
        resizeQueue(q);
    }
    q->data[q->tail] = value;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
    return;
}

int dequeue(CircularQueue* q) {
    if (!isEmpty(q)) {
        int value = q->data[q->head];
        q->head = (q->head + 1) % q->capacity;
        q->size--;
        return value;
    } else {
        printf("Ошибка: очередь пуста\n");
        exit(1);
    }
}

int get(CircularQueue* q, int index) {
    if (index < 0 || index >= q->size) {
        printf("Ошибка: индекс вне границ очереди\n");
        exit(1);
    }
    return q->data[(q->head + index) % q->capacity];
}

void set(CircularQueue* q, int index, int value) {
    if (index < 0 || index >= q->size) {
        printf("Ошибка: индекс вне границ очереди\n");
        exit(1);
    }
    q->data[(q->head + index) % q->capacity] = value;
    return;
}

void printQueue(CircularQueue* queue) {
    for (int i = 0; i < queue->size; i++) {
        printf("%d ", get(queue, i));
    }
    printf("\n");
    return;
}

void assign(CircularQueue* q, int *arr, int n) {
    for (int i = 0; i < n; i++) {
        enqueue(q, arr[i]);
    }
    return;
}

void discharge(CircularQueue* q, int *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = dequeue(q);
    }
    return;
}
