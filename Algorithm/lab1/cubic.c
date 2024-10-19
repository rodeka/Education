#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cubic.h"

void initCubic(Cubic* cubic) {
    cubic->a=1;
    cubic->b=0;
    cubic->c=0;
    cubic->d=0;
    return;
}

void freeCubic(Cubic* cubic) {
    free(cubic);
    return;
}

double f(Cubic* cubic, double x) {
    return cubic->a * x * x * x + cubic->b * x * x + cubic->c * x + cubic->d;
}

double df(Cubic* cubic, double x) {
    return 3 * cubic->a * x * x + 2 * cubic->b * x + cubic->c;
}

double ddf(Cubic* cubic, double x) {
    return 6 * cubic->a * x + 2 * cubic->b;
}

double comb_method(Cubic* cubic, double x0, double x1) {
    int iter = 0;
    while (fabs(x0 - x1) > 2 * EPSILON && iter < MAX_ITER) {
        if (f(cubic, x0) * ddf(cubic, x0) < 0) {
            x0 = x0 - f(cubic, x0) * (x0 - x1) / (f(cubic, x0) - f(cubic, x1));
        } else if (f(cubic, x0) * ddf(cubic, x0) > 0) {
            x0 = x0 - f(cubic, x0) / df(cubic, x0);
        }

        if (f(cubic, x1) * ddf(cubic, x1) < 0) {
            x1 = x1 - f(cubic, x1) * (x1 - x0) / (f(cubic, x1) - f(cubic, x0));
        } else if (f(cubic, x1) * ddf(cubic, x1) > 0) {
            x1 = x1 - f(cubic, x1) / df(cubic, x1);
        }
        iter++;
    }
    printf("x: %lf\n", (x0 + x1) / 2);
    return (x0 + x1) / 2;
}

void solveCubicHybrid(Cubic* cubic){
    // поиск экстремумов
    double ex1, ex2;
    double da = 3 * cubic->a, db = 2 * cubic->b, dc = cubic->c;
    double disc = db * db - 4 * da * dc;
    if (disc > 0) {
        ex1 = (-db - sqrt(disc)) / (2 * da);
        ex2 = (-db + sqrt(disc)) / (2 * da);
        if (f(cubic, LEFT_SIDE) * f(cubic, ex1) < 0)
            comb_method(cubic, LEFT_SIDE, ex1);
        if (f(cubic, ex1) * f(cubic, ex2) <= 0)
            comb_method(cubic, ex1, ex2);
        if (f(cubic, ex2) * f(cubic, RIGHT_SIDE) < 0)
            comb_method(cubic, ex2, RIGHT_SIDE);
    } else {
        comb_method(cubic, LEFT_SIDE, RIGHT_SIDE);
    }
    return;
}

void solveCubicKardano(Cubic* cubic) {
    // Приводим уравнение к виду x^3 + px + q = 0
    double p = (3 * cubic->a * cubic->c - cubic->b * cubic->b) / (3 * cubic->a * cubic->a);
    double q = (2 * cubic->b * cubic->b * cubic->b - 9 * cubic->a * cubic->b * cubic->c + 27 * cubic->a * cubic->a * cubic->d) / (27 * cubic->a * cubic->a * cubic->a);

    double Q = pow(q / 2, 2) + pow(p / 3, 3);

    if (Q > 0) {
        // Один вещественный корень и два комплексных
        double u = cbrt(-q / 2 + sqrt(Q));
        double v = cbrt(-q / 2 - sqrt(Q));
        double x1 = u + v - cubic->b / (3 * cubic->a);
        printf("x: %lf\n", x1);
    } else if (Q == 0) {
        double u = cbrt(-q / 2);
        if (p == 0 && q == 0) {
            double x1 = 2 * u - cubic->b / (3 * cubic->a);
            printf("x: %lf\n", x1); // Все корни вещественные, три из них кратны
        }
        else {
            double x1 = 2 * u - cubic->b / (3 * cubic->a);
            double x2 = -u - cubic->b / (3 * cubic->a);
            printf("x: %lf\nx: %lf\n", x1, x2); // Все корни вещественные, два из них совпадают
        }
            
    } else if (Q < 0) {
        // Три вещественных корня
        double r = sqrt(pow(-p / 3, 3));
        double phi = acos(-q / (2 * r));
        double x1 = 2 * cbrt(r) * cos(phi / 3) - cubic->b / (3 * cubic->a);
        double x2 = 2 * cbrt(r) * cos((phi + 2 * PI) / 3) - cubic->b / (3 * cubic->a);
        double x3 = 2 * cbrt(r) * cos((phi + 4 * PI) / 3) - cubic->b / (3 * cubic->a);
        printf("x: %lf\nx: %lf\nx: %lf\n", x1, x2, x3);
    }
    return;
}