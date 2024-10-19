#include "deque.h"
#include <stdlib.h>
#include <stdio.h>

void initDeque(Deque* deque, int initialCapacity) {
    deque->data = (int*)malloc(initialCapacity * sizeof(int));
    if (!deque->data) {
        printf("Ошибка: не удалось выделить память\n");
        exit(1);
    }
    deque->front = initialCapacity / 2;
    deque->rear = initialCapacity / 2;
    deque->size = 0;
    deque->capacity = initialCapacity;
    return;
}

void resizeDeque(Deque* deque) {
    int newCapacity = deque->capacity * 2;
    int* new_data = (int*)malloc(newCapacity * sizeof(int));
     if (!new_data) {
        printf("Ошибка: не удалось выделить память\n");
        freeDeque(deque);
        exit(1);
    }
    for(int i = deque->front; i <= deque->rear; i++){
        new_data[newCapacity / 4 - deque->front + i] = deque->data[i];
    }
    free(deque->data);
    deque->data = new_data;
    deque->front = newCapacity / 4;
    deque->rear = deque->front + deque->size - 1;
    deque->capacity = newCapacity;
    return;
}

int isEmpty(Deque* deque){
    return deque->size == 0;
}

int isEnd(Deque* deque) {
    return deque->rear == deque->capacity - 1;
}

int isStart(Deque* deque) { 
    return deque->front == 0;
}

void pushBack(Deque* deque, int value) {
    if (isEnd(deque)) {
        resizeDeque(deque);
    }
    deque->rear++;
    deque->data[deque->rear] = value;
    if(isEmpty(deque)){
        deque->front = deque->rear;
    }
    deque->size++;
    return;
}

void pushFront(Deque* deque, int value) {
    if (isStart(deque)) {
        resizeDeque(deque);
    }
    deque->front--;
    deque->data[deque->front] = value;
    if(isEmpty(deque)){
        deque->rear = deque->front;
    }
    deque->size++;
    return;
}

int popBack(Deque* deque) {
    if(isEmpty(deque)){
        printf("Ошибка: дек пуст");
        freeDeque(deque);
        exit(1);
    }
    deque->size--;
    return deque->data[deque->rear--];
}

int popFront(Deque* deque) {
    if(isEmpty(deque)){
        printf("Ошибка: дек пуст");
        freeDeque(deque);
        exit(1);
    }
    deque->size--;
    return deque->data[deque->front++];
}

void assign(Deque* deque, int* arr, int n) {
    for(int i = 0; i < n; i++) {
        pushBack(deque, arr[i]);
    }
    return;
}

void discharge(Deque* deque, int* arr, int n) {
    for(int i=0; i<n; i++){
        arr[i] = popFront(deque);
    }
    return;
}

void freeDeque(Deque* deque) {
    free(deque->data);
    free(deque);
    return;
}

void printDeque(Deque* deque) {
    for(int i = deque->front; i <= deque->rear; i++){
        printf("%d ", deque->data[i]);
    }
    printf("\n");
    return;
}
