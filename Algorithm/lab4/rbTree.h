// Структура для узла красно-черного дерева
typedef enum { RED, BLACK } NodeColor;

typedef struct RBNode {
    int data;
    NodeColor color;
    struct RBNode* left, *right, *parent;
} RBNode;

// Создание нового узла красно черного дерева
RBNode* createNode(int data);

// Поворот дерева налево для балансировки
void rotateLeft(RBNode** root, RBNode* x);

// Поворот дерева направо для балансировки
void rotateRight(RBNode** root, RBNode* x);

// Исправление нарушения при вставке узла
void fixViolation(RBNode** root, RBNode* z);

// Вставка нового узла в дерево
void insertRBTree(RBNode** root, int data);

// Вывод элементов красно-черного дерева
void inorderTraversal(RBNode* root);

// Освобождение памяти
void freeRBTree(RBNode* root);