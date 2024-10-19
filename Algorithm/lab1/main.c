#include <stdio.h>
#include <stdlib.h>
#include "cubic.h"

int main() {
    Cubic* cubic = (Cubic*)malloc(sizeof(Cubic));
    if(!cubic){
        printf("Ошибка: не удалось выделить память\n");
        return 1;
    }
    initCubic(cubic);
    
    // Ввод коэффициентов кубического уравнения
    printf("Введите коэффициенты уравнения ax^3 + bx^2 + cx + d = 0:\na b c d\n");
    if (scanf("%lf %lf %lf %lf", &cubic->a, &cubic->b, &cubic->c, &cubic->d) != 4) {
        printf("Ошибка ввода. Убедитесь, что вы вводите числа типа double.\n");
        return 1;
    }
    if(cubic->a==0){
        printf("Коэфициент a должен быть не нулевым.\n");
        return 1;
    }

    cubic->b /= cubic->a;
    cubic->c /= cubic->a;
    cubic->d /= cubic->a;
    cubic->a /= cubic->a;

    printf("Комбинированный метод решения:\n");
    solveCubicHybrid(cubic);
    printf("Решение по формуле Кардано:\n");
    solveCubicKardano(cubic);
    freeCubic(cubic);
    return 0;
}