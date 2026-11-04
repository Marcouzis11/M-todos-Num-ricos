#include <stdio.h>
#include <math.h>

// RK4 para UNA ecuacion de primer orden: y1' = f1(x, y1).
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

    // DESDE ACA se conserva el esquema RK4 al cambiar de ejercicio.
    double k11, k21, k31, k41;
    printf("RK4 para 1 ecuacion\n");
    printf("    i |              x |             y1\n");
    printf("%5d | %14.10f | %14.10f\n", 0, x[0], y1[0]);

    for (int i = 0; i < n; i++) {
        x[i+1] = x[i] + h;

        // k11: etapa 1, ecuacion 1. Los k son pendientes (sin multiplicar por h).
        k11 = f1(x[i], y1[i]);
        k21 = f1(x[i] + h/2, y1[i] + (h/2)*k11);
        k31 = f1(x[i] + h/2, y1[i] + (h/2)*k21);
        k41 = f1(x[i] + h, y1[i] + h*k31);

        y1[i+1] = y1[i] + (h/6)*(k11 + 2*k21 + 2*k31 + k41);

        printf("%5d | %14.10f | %14.10f\n", i+1, x[i+1], y1[i+1]);
    }
    return 0;
}
