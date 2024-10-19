// Структура для кольцевой очереди
typedef struct {
    int* data;
    int head;
    int tail;
    int size;
    int capacity;
} CircularQueue;

// Инициализация кольцевой очереди
void initQueue(CircularQueue* queue, int initialCapacity);

// Изменение размера кольцевой очереди
void resizeQueue(CircularQueue* queue);

// Освобождение памяти кольцевой очереди
void freeQueue(CircularQueue* queue);

// Проверка полна ли очередь
int isFull(CircularQueue* q);

// Проверка пуста ли очередь
int isEmpty(CircularQueue* q);

// Вставка элемента в конец очереди
void enqueue(CircularQueue* queue, int value);

// Получение первого элемента очереди
int dequeue(CircularQueue* queue);

// Копирование n чисел из массива в очередь
void assign(CircularQueue* queue, int* arr, int n);

// Взятие n чисел из очереди в массив
void discharge(CircularQueue* q, int* arr, int n);

// Вывод очереди
void printQueue(CircularQueue* queue);