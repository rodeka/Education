#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100  // Максимальный размер дека

// Структура для дека
typedef struct{
    int data[MAX_SIZE];
    int front;
    int rear;
    int size;
} Deque;

// Инициализация дека
void initDeque(Deque *deque){
    deque->front = -1;
    deque->rear = 0;
    deque->size = 0;
    return;
}

// Проверка, пуст ли дек
int isEmpty(Deque *deque){
    return deque->size == 0;
}

// Проверка, полон ли дек
int isFull(Deque *deque){
    return deque->size == MAX_SIZE;
}

// Вставка элемента в конец дека
void pushBack(Deque *deque, int value){
    if (isFull(deque)){
        printf("Ошибка: дек полон. Невозможно вставить элемент %d\n", value);
        exit(1);
    }
    deque->rear = (deque->rear + 1) % MAX_SIZE;
    deque->data[deque->rear] = value;
    if (deque->size == 0){
        deque->front = deque->rear;
    }
    deque->size++;
    return;
}

// Получение элемента из дека по индексу
int get(Deque *deque, int index){
    if (index < 0 || index >= deque->size){
        printf("Ошибка: некорректный индекс %d. Размер дека: %d\n", index, deque->size);
        exit(1);
    }
    return deque->data[(deque->front + index) % MAX_SIZE];
}

// Замена элемента по индексу
void set(Deque *deque, int index, int value){
    if (index < 0 || index >= deque->size){
        printf("Ошибка: некорректный индекс %d. Размер дека: %d\n", index, deque->size);
        exit(1);
    }
    deque->data[(deque->front + index) % MAX_SIZE] = value;
    return;
}

// Функция для обмена двух элементов в деке
void swap(Deque *deque, int i, int j){
    if (i < 0 || j < 0 || i >= deque->size || j >= deque->size){
        printf("Ошибка: некорректные индексы для обмена: %d и %d. Размер дека: %d\n", i, j, deque->size);
        exit(1);
    }
    int temp = get(deque, i);
    set(deque, i, get(deque, j));
    set(deque, j, temp);
    return;
}

// Коктейльная сортировка
void cocktailSort(Deque *deque);

// Чтение данных из файла
int read_numbers_from_file(Deque *deque, const char *filename);

int main(){
    char filename[255];
    printf("Введите имя файла: ");
    scanf("%s", filename);

    Deque deque;
    initDeque(&deque);

    if(read_numbers_from_file(&deque, filename) < 0){
        return 1; // Ошибка при чтении файла
    }

    if (deque.size == 0){
        printf("Ошибка: файл %s пуст или не содержит чисел.\n", filename);
        return 1;
    }

    printf("Числа из файла до сортировки:\n");
    for (int i = 0; i < deque.size; i++){
        printf("%d ", get(&deque, i));
    }
    printf("\n");

    cocktailSort(&deque);

    printf("Числа после коктейльной сортировки:\n");
    for (int i = 0; i < deque.size; i++){
        printf("%d ", get(&deque, i));
    }
    printf("\n");

    return 0;
}

void cocktailSort(Deque *deque){
    int swapped = 1;
    int start = 0;
    int end = deque->size - 1;

    while (swapped){
        swapped = 0;

        // Проход слева направо
        for (int i = start; i < end; i++){
            if (get(deque, i) > get(deque, i + 1)){
                swap(deque, i, i + 1);
                swapped = 1;
            }
        }

        // Если не было обменов, массив отсортирован
        if (!swapped)
            break;

        swapped = 0;
        end--;

        // Проход справа налево
        for (int i = end; i > start; i--){
            if (get(deque, i) > get(deque, i - 1)){
                swap(deque, i, i - 1);
                swapped = 1;
            }
        }
        start++;
    }
    return;
}

int read_numbers_from_file(Deque *deque, const char *filename){
    FILE *file = fopen(filename, "r");
    if (!file){
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return -1;
    }

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