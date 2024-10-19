#include <stdio.h>
#include <stdlib.h>
#include "deque.h"
#include "sort.h"

// Чтение данных из файла
int read_numbers_from_file(const char* filename, Deque* deque);

int main() {
    char filename[255];
    printf("Введите имя файла: ");
    scanf("%s", filename);

    Deque* deque = (Deque*)malloc(sizeof(Deque));
    initDeque(deque, 32); // Инициализируем дек с начальной емкостью 32

    int size = read_numbers_from_file(filename, deque);
    if (size < 0) {
        freeDeque(deque);
        return 1; // Ошибка при чтении файла
    }

    if (isEmpty(deque)) {
        printf("Ошибка: файл %s пуст или не содержит чисел.\n", filename);
        freeDeque(deque);
        return 1;
    }

    printf("Числа из файла до сортировки:\n");
    printDeque(deque);

    int* unsorted_data = (int*)malloc(size * sizeof(int));
    discharge(deque, unsorted_data, size);
    cocktail_sort(unsorted_data, size);

    printf("Числа после коктейльной сортировки:\n");
    assign(deque, unsorted_data, size);
    printDeque(deque);

    free(unsorted_data);
    freeDeque(deque);
    return 0;
}

int read_numbers_from_file(const char* filename, Deque* deque){
    FILE* file = fopen(filename, "r");
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
        pushBack(deque, value);
    }

    fclose(file);
    return deque->size;
}