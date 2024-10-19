#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include "sort.h"

int read_numbers_from_file(const char *filename, Array* arr){
    FILE *file = fopen(filename, "r");
    if (!file){
        printf("Ошибка: не удалось открыть файл '%s'\n", filename);
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

    int temp, index = 0;

    while (!feof(file)){
        // Проверяем, удалось ли считать число
        if (fscanf(file, "%d", &temp) != 1){
            printf("Ошибка: обнаружены некорректные данные в файле\n");
            fclose(file);
            return -1;
        }

        // Если массив заполнен, увеличиваем его размер
        if (index >= arr->capacity){
            resizeArray(arr);
        }
        arr->data[index++] = temp;
    }

    fclose(file);
    return index;
}

int main(){
    char filename[255];
    printf("Введите имя файла: ");
    scanf("%s", filename);

    int choice;
    printf("Выберите тип массива:\n1. Статический (макс %d элементов)\n2. Динамический\n", STATIC_ARRAY_SIZE);
    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)){
        printf("Ошибка: неверный ввод, программа завершена.\n");
        return 1;
    }

    Array* array = (Array*)malloc(sizeof(Array));
    initArray(array, 32, (choice == 2) ? 1 : 0);

    int num_elements = read_numbers_from_file(filename, array);

    if (num_elements < 0){
        freeArray(array);
        return 1; // Ошибка при чтении файла
    }

    printf("Числа из файла до сортировки:\n");
    printArray(array, num_elements);

    cocktail_sort(array->data, num_elements);

    printf("Числа после коктейльной сортировки:\n");
    printArray(array, num_elements);

    // Освобождаем память
    freeArray(array);
    return 0;
}