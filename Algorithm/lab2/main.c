#include <stdio.h>
#include <stdlib.h>

#define STATIC_ARRAY_SIZE 100 // Размер статического массива

// Функция свапа чисел через указатели
void swap(int* a, int* b);

// Функция для коктейльной сортировки (shaker sort)
void cocktail_sort(int **arr, int n);

// Функция для чтения чисел из файла в массив
int read_numbers_from_file(const char *filename, int **arr, int *size, int use_dynamic);

int main(){
    int choice;
    printf("Выберите тип массива:\n1. Статический (макс %d элементов)\n2. Динамический\n", STATIC_ARRAY_SIZE);
    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)){
        printf("Ошибка: неверный ввод, программа завершена.\n");
        return 1;
    }

    char filename[255];
    printf("Введите имя файла: ");
    scanf("%s", filename);

    int arr_static[STATIC_ARRAY_SIZE]; // Статический массив
    int *arr_dynamic = NULL; // Указатель для динамического массива
    int *arr = (choice == 1) ? arr_static : arr_dynamic;
    int size = (choice == 1) ? STATIC_ARRAY_SIZE : 0;

    int num_elements = read_numbers_from_file(filename, &arr, &size, choice == 2);

    if (num_elements < 0){
        return 1; // Ошибка при чтении файла
    }

    printf("Числа из файла до сортировки:\n");
    for (int i = 0; i < num_elements; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");

    cocktail_sort(&arr, num_elements);

    printf("Числа после коктейльной сортировки:\n");
    for (int i = 0; i < num_elements; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Освобождаем память, если использовали динамический массив
    if (choice == 2){
        free(arr);
    }

    return 0;
}

void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
    return;
}

void cocktail_sort(int **arr, int n){
    int swapped = 1;
    int start = 0;
    int end = n - 1;

    while (swapped){
        swapped = 0;

        // Проходим слева направо
        for (int i = start; i < end; i++){
            if ((*arr)[i] > (*arr)[i + 1]){
                swap(&(*arr)[i], &(*arr)[i+1]);
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
            if ((*arr)[i] < (*arr)[i - 1]){
                swap(&(*arr)[i], &(*arr)[i-1]);
                swapped = 1;
            }
        }
        start++;
    }
    return;
}

int read_numbers_from_file(const char *filename, int **arr, int *size, int use_dynamic){
    FILE *file = fopen(filename, "r");
    if (!file){
        printf("Ошибка: не удалось открыть файл '%s'\n", filename);
        return -1;
    }

    int temp, index = 0;

    if (use_dynamic){
        // Изначально динамически выделяем небольшой массив
        *size = 10;
        *arr = (int*)malloc(sizeof(int) * (*size));
        if (!*arr){
            printf("Ошибка: не удалось выделить память\n");
            fclose(file);
            return -1;
        }
    }

    while (!feof(file)){
        // Проверяем, удалось ли считать число
        if (fscanf(file, "%d", &temp) != 1){
            printf("Ошибка: обнаружены некорректные данные в файле\n");
            fclose(file);
            return -1;
        }

        // Если массив заполнен, увеличиваем его размер
        if (index >= *size){
            if (use_dynamic){
                // Увеличиваем размер динамического массива вдвое
                *size *= 2;
                *arr = (int*)realloc(*arr, sizeof(int) * (*size));
                if (!*arr){
                    printf("Ошибка: не удалось расширить память\n");
                    fclose(file);
                    return -1;
                }
            } else{
                printf("Ошибка: размер статического массива превышен\n");
                fclose(file);
                return -1;
            }
        }
        (*arr)[index++] = temp;
    }

    fclose(file);
    return index;
}