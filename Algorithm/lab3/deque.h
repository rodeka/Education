#pragma once

typedef struct{
    int* data;      // массив для хранения элементов дека
    int front;      // индекс переднего элемента
    int rear;       // индекс последнего элемента
    int size;       // текущее кол-во элементов в деке
    int capacity;   // максимальное кол-во элементов в деке
} Deque;

// Инициализация дека
void initDeque(Deque* deque, int initialCapacity);

// Освобождение памяти дека
void freeDeque(Deque* deque);

// Изменение размера дека
void resizeDeque(Deque* deque);

// Проверка пуст ли дек
int isEmpty(Deque* deque);

// Проверка достиг ли последний элемент конца
int isEnd(Deque* deque);

// Проверка достиг ли передней элемент начала
int isStart(Deque* deque);

// Вставка элемента в конец дека
void pushBack(Deque* deque, int value);

// Вставить элемент в начало дека
void pushFront(Deque* deque, int value);

// Удаляет элемент с конца дека и возвращает его
int popBack(Deque* deque);

// Удаляет элемент с начала дека и возвращает его
int popFront(Deque* deque);

// Копирует элементы из массива в дек, добавляя их в конец
void assign(Deque* deque, int* arr, int n);

// Перемещает элементы из дека в массив, извлекая с начала дека.
void discharge(Deque* deque, int* arr, int n);

// Вывод элементов дека
void printDeque(Deque* deque);