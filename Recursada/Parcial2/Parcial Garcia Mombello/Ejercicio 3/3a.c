#include <stdio.h>
#include <stdlib.h>

int main()
{

    //? DECLARACION VARIABLES

    float factor = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    float sum = 0;
    float resultado = 0;
    
    //? DECLARACION MATRICES

    float matriz[4][4] = {
        {0, 0, 0, 0},
        {0, 5, 1, -2},
        {0, 1, -4, 2},
        {0, 0, 2, 5}
    };

    float matrizR[4][2] = {
        {0, 0},
        {0,16},
        {0,-19},
        {0, -14}
    };

    float xNuevo[4][2] = {
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0}
    };



    //? ALGORITMO TRIANGULACION

    for(i = 1; i <= 2; i++)
    {
        for(j = i + 1; j <= 3; j++)
        {
            factor = matriz[j][i] / matriz[i][i];
            for(k = i; k <= 3; k++)
            {
                matriz[j][k] = matriz[j][k] - factor * matriz[i][k];  
            }
            matrizR[j][1] = matrizR[j][1] - factor * matrizR[i][1]; 
        }
    }



    //? ALGORITMO CHECK DETERMINANTE

    float prod = 1;
    for(i = 1; i <= 3; i++)
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

    //? ALGORITMO RETRO-SUSTITUCION

    xNuevo[3][1] = matrizR[3][1]/matriz[3][3];
    for(i = 2; i >= 1; i--)
    {
        sum = matrizR[i][1];
        for(j = i+1; j <= 3; j++)
        {
            sum = sum - matriz[i][j]*xNuevo[j][1];
        }
        sum = sum/matriz[i][i];
        xNuevo[i][1]=sum;
    }
    printf("Matriz resultante:\n");
    printf("x1 :%f\n", xNuevo[1][1]);
    printf("x2: %f\n", xNuevo[2][1]);
    printf("x3: %f\n", xNuevo[3][1]);
}