#pragma once

#define STATIC_ARRAY_SIZE 100 // Размер статического массива

typedef struct
{
    int* data_dynamic;
    int data_static[STATIC_ARRAY_SIZE];
    int* data;
    int capacity;
    int dynamic;
} Array;

// инициализация массива
void initArray(Array* arr, int initialCapacity, int dynamic);

// освобождение памяти массива
void freeArray(Array* arr);

// увеличение массива вдвое при использовании дин.массива
void resizeArray(Array* arr);

// вывод n элементов массива
void printArray(Array* arr, int n);