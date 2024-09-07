#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define n 5

int main()
{
    float tolerancia = 0;
    int i, j, k;
    float factor = 0;
    float suma = 0;
    float error = 0;
    float errort = 0;
    int iteracion = 0;

    float matriz[n][n]={
        {0,0,0,0,0},
        {0,4,-1,2,3},
        {0,0,-2,7,-4},
        {0,0,0,6,5},
        {0,0,0,0,3}
    };

    float matrizR[n]={
        0, 20, -7, 4, 6
    };

    float xViejo[n] = {0, matriz[1][1], matriz[2][1], matriz[3][1], matriz[4][1]};
    float xNuevo[n] = {0};

    printf("Ingrese la tolerancia: \n");
    scanf("%f", &tolerancia);

    //? ALGORITMO TRIANGULACION
    for(i = 1; i < n-1; i++)
    {
        for(j = i + 1; j < n; j++)
        {
            factor = matriz[j][i] / matriz[i][i];
            for(k = i; k < n; k++)
            {
                matriz[j][k] = matriz[j][k] - factor * matriz[i][k];  
            }
            matrizR[j] = matrizR[j] - factor * matrizR[i]; 
        }
    }

    //? ALGORITMO CHECK DETERMINANTE
    float prod = 1;
    for(i = 1; i < n; i++)
    {
        prod *= matriz[i][i];
    }
    if(prod == 0)
    {
        printf("El sistema no tiene solución única.\n");
        exit(2);
    }
    else
    {
        printf("Determinante: %f\n", prod);
    }

    //? ALGORITMO DIAGONALMENTE DOMINANTE
    for(i = 1; i < n-1; i++)
    {
        suma = 0;
        for(j = 1; j < n-1; j++)
        {
            if(j != i)
            {
                suma += fabs(matriz[i][j]);
            }
        }
        if(fabs(matriz[i][i]) <= suma)
        {
            printf("La matriz no es diagonalmente dominante, el método podría no converger. \n");
        }
    }

    //? INICIO DEL METODO ITERATIVO DE JACOBI
    iteracion = 0;
    do
    {
        iteracion++;
        // Calcular nuevos valores para xNuevo
        for(i = 1; i < n-1; i++)
        {
            suma = 0;
            for(j = 1; j < n-1; j++)
            {
                if(j != i)
                {
                    suma += matriz[i][j] * xViejo[j];
                }
            }
            xNuevo[i] = (matrizR[i] - suma) / matriz[i][i];
        }

        // Calcular el error
        suma = 0;
        for(i = 1; i < n-1; i++)
        {
            suma += pow((xNuevo[i] - xViejo[i]), 2);
        }
        error = sqrt(suma);

        if(iteracion >= 2 && error > errort)
        {
            printf("El método no converge. \n");
            exit(404);
        }
        errort = error;

        // Actualizar xViejo con los valores de xNuevo
        for(i = 1; i < n-1; i++)
        {
            xViejo[i] = xNuevo[i];
        }

    } while (error > tolerancia && iteracion <= 1000);

    // Imprimir resultados
    printf("Soluciones:\n");
    for(i = 1; i < n; i++)
    {
        printf("x%d : %f\n", i, xNuevo[i]);
    }

    return 0;
}
