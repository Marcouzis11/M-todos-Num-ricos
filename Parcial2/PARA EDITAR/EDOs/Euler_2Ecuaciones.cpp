#include <stdio.h>
#include <math.h>

// Euler explicito para DOS ecuaciones: y1' = f1(x,y1,y2), y2' = f2(x,y1,y2).
// Ejemplo: y'' = -y + 4*cos(x), con y1 = y, y2 = y'.
// CAMBIAR los return segun el enunciado. Ambas funciones reciben todas las variables.
double f1(double x, double y1, double y2) {
    return y2;
}

double f2(double x, double y1, double y2) {
    return -y1 + 4*cos(x);
}

int main(void) {
    // CAMBIAR estos datos. n es la cantidad de PASOS, no de puntos.
    // El punto final sera x[0] + n*h. Para ir hacia atras, usar h negativo.
    const int n = 200;
    double h = 0.01;
    double x[n+1], y1[n+1], y2[n+1];
    x[0] = 0;
    y1[0] = 0;  // En el ejemplo de segundo orden: y(x0).
    y2[0] = 0;  // En el ejemplo de segundo orden: y'(x0).

    // DESDE ACA se conserva el esquema Euler al cambiar de ejercicio.
    printf("Euler para 2 ecuaciones\n");
    printf("    i |              x |             y1 |             y2\n");
    printf("%5d | %14.10f | %14.10f | %14.10f\n", 0, x[0], y1[0], y2[0]);

    for (int i = 0; i < n; i++) {
        x[i+1] = x[i] + h;

        // Las DOS derivadas usan los valores del MISMO paso i.
        // No usar y1[i+1] al calcular y2[i+1].
        y1[i+1] = y1[i] + h*f1(x[i], y1[i], y2[i]);
        y2[i+1] = y2[i] + h*f2(x[i], y1[i], y2[i]);

        printf("%5d | %14.10f | %14.10f | %14.10f\n", i+1, x[i+1], y1[i+1], y2[i+1]);
    }
    return 0;
}
