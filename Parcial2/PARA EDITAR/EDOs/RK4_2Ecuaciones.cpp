#include <stdio.h>
#include <math.h>

// RK4 para DOS ecuaciones: y1' = f1(x,y1,y2), y2' = f2(x,y1,y2).
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

    // DESDE ACA se conserva el esquema RK4 al cambiar de ejercicio.
    // Primer indice del k: etapa. Segundo indice: ecuacion.
    double k11, k21, k31, k41, k12, k22, k32, k42;
    printf("RK4 para 2 ecuaciones\n");
    printf("    i |              x |             y1 |             y2\n");
    printf("%5d | %14.10f | %14.10f | %14.10f\n", 0, x[0], y1[0], y2[0]);

    for (int i = 0; i < n; i++) {
        x[i+1] = x[i] + h;

        // Etapa 1: las dos pendientes usan el mismo punto actual.
        k11 = f1(x[i], y1[i], y2[i]);
        k12 = f2(x[i], y1[i], y2[i]);

        // Etapa 2: las dos ecuaciones usan las pendientes de la etapa 1.
        k21 = f1(x[i] + h/2, y1[i] + (h/2)*k11, y2[i] + (h/2)*k12);
        k22 = f2(x[i] + h/2, y1[i] + (h/2)*k11, y2[i] + (h/2)*k12);

        // Etapa 3: se usan las pendientes de la etapa 2.
        k31 = f1(x[i] + h/2, y1[i] + (h/2)*k21, y2[i] + (h/2)*k22);
        k32 = f2(x[i] + h/2, y1[i] + (h/2)*k21, y2[i] + (h/2)*k22);

        // Etapa 4: se usan las pendientes de la etapa 3.
        k41 = f1(x[i] + h, y1[i] + h*k31, y2[i] + h*k32);
        k42 = f2(x[i] + h, y1[i] + h*k31, y2[i] + h*k32);

        // Actualizar ambas incognitas despues de calcular todas las etapas.
        y1[i+1] = y1[i] + (h/6)*(k11 + 2*k21 + 2*k31 + k41);
        y2[i+1] = y2[i] + (h/6)*(k12 + 2*k22 + 2*k32 + k42);

        printf("%5d | %14.10f | %14.10f | %14.10f\n", i+1, x[i+1], y1[i+1], y2[i+1]);
    }
    return 0;
}
