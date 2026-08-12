#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <math.h>

float f (float);

int main(int argc, char* argv[])
{
    FILE *fp;
    fp = fopen("datosPolinomioInterpolador.dat", "r");
    if (fp == NULL)
    {
        puts("No se puede abrir el archivo");
        return 1;
    }

    // Contador de filas
    int filas = 0;
    char c;

    while ((c = fgetc(fp)) != EOF)
    {
        if (c == '\n') {
            filas++;
        }
    }

    printf("Numero de filas = %i\n", filas);

    // Cerramos y reabrimos el archivo para resetear el puntero
    fclose(fp);
    fp = fopen("datosPolinomioInterpolador.dat", "r");
    double m[filas][2];  // Considerando que siempre hay 2 columnas

    // Cargo los datos leidos en el array
    int i, j;
    for (i = 0; i < filas; i++) {
        fscanf(fp, "%lf %lf", &m[i][0], &m[i][1]);
    }
    fclose(fp);

    // Imprimo la matriz para verificar que lo leyo correctamente
    printf("Datos leidos:\n");
    for (i = 0; i < filas; i++)
    {
        printf("%lf %lf\n", m[i][0], m[i][1]);
    }

    float sum = 0;
    float resultado = 0;
    float factor = 0;
    int k = 0;

    //? Polinomio Interpolador
    float matriz[filas][filas];
    float matrizR[filas];
    float xNuevo[filas];

    //? Llenar matriz para interpolar
    for (i = 0; i < filas; i++) {
        for (j = 0; j < filas; j++) {
            matriz[i][j] = pow(m[i][0], j);
        }
        matrizR[i] = m[i][1];
    }

    //? Triangulación
    for (i = 0; i < filas - 1; i++) {
        for (j = i + 1; j < filas; j++) {
            factor = matriz[j][i] / matriz[i][i];
            for (k = i; k < filas; k++) {
                matriz[j][k] = matriz[j][k] - factor * matriz[i][k];
            }
            matrizR[j] = matrizR[j] - factor * matrizR[i];
        }
    }

    //? Verificación del determinante
    float prod = 1;
    for (i = 0; i < filas; i++) {
        prod *= matriz[i][i];
    }

    if (prod == 0) {
        printf("El sistema no tiene solución única.\n");
        exit(2);
    } else {
        printf("Determinante: %f\n", prod);
    }

    //? Retro-sustitución
    xNuevo[filas - 1] = matrizR[filas - 1] / matriz[filas - 1][filas - 1];
    for (i = filas - 2; i >= 0; i--) {
        sum = matrizR[i];
        for (j = i + 1; j < filas; j++) {
            sum -= matriz[i][j] * xNuevo[j];
        }
        xNuevo[i] = sum / matriz[i][i];
    }

    printf("Coeficientes del polinomio:\n");
    for (i = 0; i < filas; i++) {
        printf("a%d = %f\n", i, xNuevo[i]);
    }

    float x;
    printf("Ingrese xcasita: \n");
    scanf("%f", &x);

    //? Evaluo el polinomio en xcasita
    resultado = 0;
    for (i = 0; i < filas; i++) {
        resultado += xNuevo[i] * pow(x, i);
    }
    printf("Resultado P(xcasita) = %f\n", resultado);
    float error = 0;
    error = fabs(resultado - f(x));
    printf("Error = %f",error);
    return 0;
}


float f (float x)
{
    return x + 2/x;
}