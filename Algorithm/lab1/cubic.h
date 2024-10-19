#pragma once

#define EPSILON 1e-3 // Точность
#define MAX_ITER 1e3 // Максимальное количество итераций
#define LEFT_SIDE -1e9 // Крайняя левая сторона
#define RIGHT_SIDE 1e9 // Крайняя правая сторона
#define PI 3.141592 // Число Пи

typedef struct 
{
    double a;
    double b;
    double c;
    double d;
} Cubic;

// инициализация Cubic
void initCubic(Cubic* cubic);

// освобождение памяти Cubic
void freeCubic(Cubic* cubic);

// куб. функция
double f(Cubic* cubic, double x);

// первая производная куб. функции
double df(Cubic* cubic, double x);

// вторая производная куб. функции
double ddf(Cubic* cubic, double x);

// комбинированный метод
double comb_method(Cubic* cubic, double x0, double x1);

// решение куб. уравнения комбинированным методом
void solveCubicHybrid(Cubic* cubic);

// решение куб. уравнения методом Кардано
void solveCubicKardano(Cubic* cubic);