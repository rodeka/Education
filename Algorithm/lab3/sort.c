#include "sort.h"

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void cocktail_sort(int* arr, int n){
    int swapped = 1;
    int start = 0;
    int end = n - 1;

    while (swapped){
        swapped = 0;

        // Проходим слева направо
        for (int i = start; i < end; i++){
            if (arr[i] > arr[i + 1]){
                swap(&arr[i], &arr[i+1]);
                swapped = 1;
            }
        }

        // Если ничего не поменяли, значит массив отсортирован
        if (!swapped)
            break;

        swapped = 0;
        end--;

        // Проходим справа налево
        for (int i = end; i > start; i--){
            if (arr[i] < arr[i - 1]){
                swap(&arr[i], &arr[i-1]);
                swapped = 1;
            }
        }
        start++;
    }
    return;
}