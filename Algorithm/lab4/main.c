#include <stdio.h>
#include <stdlib.h>
#include "circularQueue.h"
#include "rbTree.h"
#include "sort.h"

int read_numbers_from_file(const char *filename, CircularQueue* queue){
    FILE *file = fopen(filename, "r");
    if (!file){
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return -1;
    }

    // Проверка на пустой файл
    if (fgetc(file) == EOF) {
        printf("Ошибка: файл пустой\n");
        fclose(file);
        return -1;
    }

    // Возвращаем указатель на начало файла
    rewind(file);

    int value;
    while (!feof(file)){
        if(fscanf(file, "%d", &value) != 1){
            printf("Ошибка: обнаружены некорректные данные в файле\n");
            fclose(file);
            return -1;
        }
        enqueue(queue, value);
    }

    fclose(file);
    return queue->size;
}


// Основная функция
int main() {
    char filename[255];
    printf("Введите имя файла: ");
    scanf("%s", filename);

    CircularQueue* queue = (CircularQueue*)malloc(sizeof(CircularQueue));
    initQueue(queue, 32);

    int size = read_numbers_from_file(filename, queue);
    if(size < 0){
        return 1;
    }
    printf("Числа в очереди до сортировки\n");
    printQueue(queue);

    int* unsorted_data = (int*)malloc(size * sizeof(int));
    discharge(queue, unsorted_data, size);
    cocktail_sort(unsorted_data, size);
    assign(queue, unsorted_data, size);
    
    printf("Числа в очереди после сортировки\n");
    printQueue(queue);

    RBNode* rbTree = NULL;
    for(int i = 0; i < size; i++){
        insertRBTree(&rbTree, unsorted_data[i]);
    }
    printf("Числа из красно-черного дерева с цветом\n");
    inorderTraversal(rbTree);
    printf("\n");

    free(unsorted_data);
    freeQueue(queue);
    freeRBTree(rbTree);
    return 0;
}
