#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define EPSILON 1e-9 // Точность
#define MAX_ITER 1e3 // Максимальное количество итераций
#define LEFT_SIDE -1e9 // Крайняя левая сторона
#define RIGHT_SIDE 1e9 // Крайняя правая сторона

// куб функция
double f(double x, double a, double b, double c, double d) {
    return a * x * x * x + b * x * x + c * x + d;
}

// производная куб функции
double df(double x, double a, double b, double c) {
    return 3 * a * x * x + 2 * b * x + c;
}

// вторая производная куб функции
double ddf(double x, double a, double b) {
    return 6 * a * x + 2 * b;
}

void swap_double(double *a, double *b) {
    double tmp = *a;
    *a = *b;
    *b = tmp;
    return;
}

// комбинированный метод
double comb_method(double a, double b, double c, double d, double x0, double x1) {
    int iter = 0;
    while (fabs(x0 - x1) > 2 * EPSILON && iter < MAX_ITER) {
        // метод хорд
        if (f(x0, a, b, c, d) * ddf(x0, a, b) < 0) {
            x0 = x0 - f(x0, a, b, c, d) * (x0 - x1) / (f(x0, a, b, c, d) - f(x1, a, b, c, d));
        } else if (f(x0, a, b, c, d) * ddf(x0, a, b) > 0) {
            x0 = x0 - f(x0, a, b, c, d) / df(x0, a, b, c);
        }

        // метод касательных
        if (f(x1, a, b, c, d) * ddf(x1, a, b) < 0) {
            x1 = x1 - f(x1, a, b, c, d) * (x1 - x0) / (f(x1, a, b, c, d) - f(x0, a, b, c, d));
        } else if (f(x1, a, b, c, d) * ddf(x1, a, b) > 0) {
            x1 = x1 - f(x1, a, b, c, d) / df(x1, a, b, c);
        }
    }
    printf("x: %lf\n", x1);
    return (x0 + x1) / 2;
}

void solveCubicHybrid(double a, double b, double c, double d){
    // поиск экстремумов
    double ex1, ex2;
    double da = 3 * a, db = 2 * b, dc = c;
    double disc = db * db - 4 * da * dc;
    if (disc > 0) {
        ex1 = (-db - sqrt(disc)) / (2 * da);
        ex2 = (-db + sqrt(disc)) / (2 * da);
        if (f(LEFT_SIDE, a, b, c, d) * f(ex1, a, b, c, d) <= 0)
            comb_method(a, b, c, d, LEFT_SIDE, ex1);
        if (f(ex1, a, b, c, d) * f(ex2, a, b, c, d) <= 0)
            comb_method(a, b, c, d, ex1, ex2);
        if (f(ex2, a, b, c, d) * f(RIGHT_SIDE, a, b, c, d) <= 0)
            comb_method(a, b, c, d, ex2, RIGHT_SIDE);
    } else {
        comb_method(a, b, c, d, LEFT_SIDE, RIGHT_SIDE);
    }
}

void solveCubicKardano(double a, double b, double c, double d) {
    // Приводим уравнение к виду x^3 + px + q = 0
    double p = (3 * a * c - b * b) / (3 * a * a);
    double q = (2 * b * b * b - 9 * a * b * c + 27 * a * a * d) / (27 * a * a * a);

    double discriminant = pow(q / 2, 2) + pow(p / 3, 3);

    if (discriminant > 0) {
        // Один вещественный корень и два комплексных
        double u = cbrt(-q / 2 + sqrt(discriminant));
        double v = cbrt(-q / 2 - sqrt(discriminant));
        double x1 = u + v - b / (3 * a);
        printf("x: %lf\n", x1);
    } else if (discriminant == 0) {
        // Все корни вещественные, два из них совпадают
        double u = cbrt(-q / 2);
        double x1 = 2 * u - b / (3 * a);
        double x2 = -u - b / (3 * a);
        printf("x: %lf\nx: %lf\n", x1, x2);
    } else {
        // Три вещественных корня
        double r = sqrt(pow(-p / 3, 3));
        double phi = acos(-q / (2 * r));
        double x1 = 2 * cbrt(r) * cos(phi / 3) - b / (3 * a);
        double x2 = 2 * cbrt(r) * cos((phi + 2 * M_PI) / 3) - b / (3 * a);
        double x3 = 2 * cbrt(r) * cos((phi + 4 * M_PI) / 3) - b / (3 * a);
        printf("x: %lf\nx: %lf\nx: %lf\n", x1, x2, x3);
    }
}

int main() {
    double a, b, c, d;
    // Ввод коэффициентов кубического уравнения
    printf("Введите коэффициенты уравнения ax^3 + bx^2 + cx + d = 0:\na b c d\n");
    if (scanf("%lf", &a) != 1 || a == 0) {
        printf("коэффициент a должен быть ненулевым числом.\n");
        return 1;
    }
    scanf("%lf", &b);
    scanf("%lf", &c);
    scanf("%lf", &d);
    b /= a;
    c /= a;
    d /= a;
    a /= a;

    // поиск корней уравнения комбинированным методом (хорд, касательных)
    printf("Комбинированный метод решения:\n");
    solveCubicHybrid(a, b, c, d);
    printf("Решение по формуле Кардано:\n");
    solveCubicKardano(a, b, c, d);
    return 0;
}