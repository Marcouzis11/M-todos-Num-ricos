#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define n 5

int main()
{
    float tolerancia = 0;
    int i = 0;
    int j = 0;
    int k = 0;
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

    float matrizR[n][2]={
        {0,0},
        {0,20},
        {0,-7},
        {0,4},
        {0,6}
    };

    float xViejo[n][2]={
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0}
    };

    float xNuevo[n][2]={
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0}
    };

    printf("Ingrese la tolerancia: \n");
    scanf("%f", &tolerancia);



    //? ALGORITMO TRIANGULACION

    for(i = 1; i <= 3; i++)
    {
        for(j = i + 1; j <= 4; j++)
        {
            factor = matriz[j][i] / matriz[i][i];
            for(k = i; k <= 4; k++)
            {
                matriz[j][k] = matriz[j][k] - factor * matriz[i][k];  
            }
            matrizR[j][1] = matrizR[j][1] - factor * matrizR[i][1]; 
        }
    }

    //? ALGORITMO CHECK DETERMINANTE

    float prod = 1;
    for(i = 1; i <= 4; i++)
    {
        prod = prod * matriz[i][i];
    }
    if(prod == 0)
    {
        printf("El sistema no tiene solución única.\n");
        exit(2);
    }else{
        printf("Determinante: %f\n", prod);
    }

    //? ALGORITMO DIAGONALMENTE DOMINANTE
    for(i = 1;i <= n-1 ;i++)
    {
        suma = 0;
        for(j = 1; j <= n-1; j++)
        {
            if(j!=i)
            {
                suma = suma + fabs(matriz[i][j]);
            }
        }
        if(fabs(matriz[i][i]) <= suma)
        {
            printf("La matriz no es DG, el método podría no converger. \n");
        }
    }
    iteracion = 0;
    do
    {
        iteracion++;
        for(i=1; i<=n-1; i++)
        {
            suma = 0;
            for(j = 1; j <= n-1; j++)
            {
                if(j!=i)
                {
                    suma = suma + matriz[i][j]* xNuevo[j][1];
                }
            }
            xNuevo[i][1]= (matrizR[i][1]-suma)/matriz[i][i];
        }

        suma = 0;
        for(i = 1; i<= n-1;i++)
        {
            suma = suma + pow((xNuevo[i][1]-xViejo[i][1]),2);
        }
        error = sqrt(suma);

        if(iteracion >= 2 && error>errort)
        {
            printf("El metodo no converge. \n");
            exit(404);
        }
        errort = error;

        for(i = 1; i<= n-1; i++)
        {
            xViejo[i][1] = xNuevo[i][1];
        }
    } while (error>tolerancia && iteracion <= 1000);
    printf("Matriz resultante:\n");
    printf("x1 :%f\n", xNuevo[1][1]);
    printf("x2: %f\n", xNuevo[2][1]);
    printf("x3: %f\n", xNuevo[3][1]);
    printf("x4: %f\n", xNuevo[4][1]);
}