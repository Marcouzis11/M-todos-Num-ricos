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

    float matriz[5][5] = {
        {0, 0, 0, 0, 0},
        {0, 2, -1, 3, -1},
        {0, 1, 0, -2, 2},
        {0, 3, -4, -1, 1},
        {0,-1, -1, 3, 1} 
    };

    float matrizR[5][2] = {
        {0, 0},
        {0,5},
        {0,3},
        {0, -4},
        {0, 10}
    };

    float xNuevo[5][2] = {
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0}
    };



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

    //? ALGORITMO RETRO-SUSTITUCION

    xNuevo[4][1] = matrizR[4][1]/matriz[4][4];
    for(i = 3; i >= 1; i--)
    {
        sum = matrizR[i][1];
        for(j = i+1; j <= 4; j++)
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
    printf("x4: %f\n", xNuevo[4][1]);
}