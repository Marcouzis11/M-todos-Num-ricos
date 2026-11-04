#include <stdio.h>
#include <math.h>

// Euler explicito para TRES ecuaciones: y1' = f1, y2' = f2, y3' = f3.
// Ejemplo: y''' = -y' + 4*cos(x), con y1 = y, y2 = y', y3 = y''.
// CAMBIAR los return segun el enunciado. Todas reciben x, y1, y2, y3.
double f1(double x, double y1, double y2, double y3) {
    return y2;
}

double f2(double x, double y1, double y2, double y3) {
    return y3;
}

double f3(double x, double y1, double y2, double y3) {
    return -y2 + 4*cos(x);
}

int main(void) {
    // CAMBIAR estos datos. n es la cantidad de PASOS, no de puntos.
    // El punto final sera x[0] + n*h. Para ir hacia atras, usar h negativo.
    const int n = 200;
    double h = 0.01;
    double x[n+1], y1[n+1], y2[n+1], y3[n+1];
    x[0] = 0;
    y1[0] = 0;  // En el ejemplo de tercer orden: y(x0).
    y2[0] = 0;  // En el ejemplo de tercer orden: y'(x0).
    y3[0] = 0;  // En el ejemplo de tercer orden: y''(x0).

    // DESDE ACA se conserva el esquema Euler al cambiar de ejercicio.
    printf("Euler para 3 ecuaciones\n");
    printf("    i |              x |             y1 |             y2 |             y3\n");
    printf("%5d | %14.10f | %14.10f | %14.10f | %14.10f\n", 0, x[0], y1[0], y2[0], y3[0]);

    for (int i = 0; i < n; i++) {
        x[i+1] = x[i] + h;

        // Las TRES derivadas usan los valores del MISMO paso i.
        // No usar valores [i+1] dentro de f1, f2 ni f3.
        y1[i+1] = y1[i] + h*f1(x[i], y1[i], y2[i], y3[i]);
        y2[i+1] = y2[i] + h*f2(x[i], y1[i], y2[i], y3[i]);
        y3[i+1] = y3[i] + h*f3(x[i], y1[i], y2[i], y3[i]);

        printf("%5d | %14.10f | %14.10f | %14.10f | %14.10f\n", i+1, x[i+1], y1[i+1], y2[i+1], y3[i+1]);
    }
    return 0;
}
