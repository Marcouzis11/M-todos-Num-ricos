#include <stdio.h>
#include <math.h>

// Euler explicito para UNA ecuacion de primer orden: y1' = f1(x, y1).
// CAMBIAR esta funcion por la derivada despejada del enunciado.
double f1(double x, double y1) {
    return x*sqrt(y1);
}

int main(void) {
    // CAMBIAR estos datos. n es la cantidad de PASOS, no de puntos.
    // El punto final sera x[0] + n*h. Para ir hacia atras, usar h negativo.
    const int n = 200;
    double h = 0.01;
    double x[n+1], y1[n+1];
    x[0] = 0;
    y1[0] = 1;  // Condicion inicial y(x0).

    // DESDE ACA se conserva el esquema Euler al cambiar de ejercicio.
    printf("Euler para 1 ecuacion\n");
    printf("    i |              x |             y1\n");
    printf("%5d | %14.10f | %14.10f\n", 0, x[0], y1[0]);

    for (int i = 0; i < n; i++) {
        x[i+1] = x[i] + h;
        y1[i+1] = y1[i] + h*f1(x[i], y1[i]);

        printf("%5d | %14.10f | %14.10f\n", i+1, x[i+1], y1[i+1]);
    }
    return 0;
}
