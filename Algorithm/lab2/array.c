#include <stdio.h>
#include <stdlib.h>
#include "array.h"

void initArray(Array* arr, int initialCapacity, int dynamic){
    if(dynamic) {
        arr->data_dynamic = (int*)malloc(initialCapacity * sizeof(int));
        arr->capacity = initialCapacity;
        arr->data = arr->data_dynamic;
    }
    else {
        arr->data = arr->data_static;
        arr->capacity = STATIC_ARRAY_SIZE;
    }
    arr->dynamic = dynamic;
    return;
}

void freeArray(Array* arr) {
    if(arr->dynamic)
        free(arr->data);
    free(arr);
    return;
}

void resizeArray(Array* arr) {
    if(arr->dynamic){
        int newCapacity = arr->capacity * 2;
        arr->data = (int*)realloc(arr->data, newCapacity * sizeof(int));
        arr->capacity = newCapacity;
    }
    else {
        printf("Ошибка: невозможно увелечить размер статического массива\n");
        freeArray(arr);
        exit(1);
    }
    return;
}

void printArray(Array* arr, int n) {
    if(n > arr->capacity){
        printf("Ошибка: выход за размер массива\n");
        freeArray(arr);
        exit(1);
    }
    
    for(int i = 0; i < n; i++) {
        printf("%d ", arr->data[i]);
    } 
    printf("\n");
    return;
}