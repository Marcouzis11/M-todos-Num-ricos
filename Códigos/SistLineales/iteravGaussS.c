#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define n 4

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
        {2, -1, 3, -1},
        {3, -4, -1, 1},
        {-1, -1, 3, 1}, 
        {1, 0, -2, 2}
    };


    float matrizR[n]={
        5,-4,10,3
    };

    float xViejo[n]=
        {0,0,0,0};

    float xNuevo[n]=
        {0,0,0,0};


    printf("Ingrese la tolerancia: \n");
    scanf("%f", &tolerancia);



    //? ALGORITMO TRIANGULACION

    for(i = 0; i <= 2; i++)
    {
        for(j = i + 1; j <= 3; j++)
        {
            factor = matriz[j][i] / matriz[i][i];
            for(k = i; k <= 3; k++)
            {
                matriz[j][k] = matriz[j][k] - factor * matriz[i][k];  
            }
            matrizR[j] = matrizR[j] - factor * matrizR[i]; 
        }
    }

    //? ALGORITMO CHECK DETERMINANTE

    float prod = 1;
    for(i = 0; i <= 3; i++)
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
    printf("%f,%f,%f,%f\n",matriz[0][0],matriz[0][1],matriz[0][2],matriz[0][3]);
    printf("%f,%f,%f,%f\n",matriz[1][0],matriz[1][1],matriz[1][2],matriz[1][3]);
    printf("%f,%f,%f,%f\n",matriz[2][0],matriz[2][1],matriz[2][2],matriz[2][3]);
    printf("%f,%f,%f,%f\n",matriz[3][0],matriz[3][1],matriz[3][2],matriz[3][3]);
    int counter = 0;
	for(int i = 0; i <= n-1 ; i++){
		suma = 0; 
		counter++;
		for(int j = 0 ; j <= n-1 ; j++){
			if(j!=i){
                printf("j: %d\n", j);
				suma+= fabs(matriz[i][j]);
                printf("suma: %f \n", suma);
			}
		}
		
		if(fabs(matriz[i][i]) < suma)
        {
			printf("\nLa matriz no es diagonalmente dominante. Fila: %d\n",counter);
        }
        else
        {
            printf("\nLa matriz es diagonalmente dominante. Fila: %d\n", counter);
        }
	}

    //////////////////////////////////
    iteracion = 0;
    do
    {
        
        iteracion++;
        for(i = 0; i<=3;i++)
        {
            suma = 0;
            if(i == 0)
            {
                for(j=1;j<=n-1;j++)
                {
                    suma = suma + matriz[i][j]*xNuevo[j];
                }
                xNuevo[i] = (matrizR[i]-suma)/matriz[i][i];
            }
            for(j = 0; j<=i-1;j++)
            {
                suma = suma + matriz[i][j]*xNuevo[j];
            }
            for(j = i+1; j<=3;j++)
            {
                suma = suma + matriz[i][j]*xViejo[j];
            }
            xNuevo[i] = (matrizR[i] - suma)/ matriz[i][i];
        }
        
            suma = 0;
            for(i = 0; i<= n-1;i++)
            {
                suma = suma + pow((xNuevo[i]-xViejo[i]),2);
            }
            error = sqrt(suma);

            if(iteracion >= 2 && error>errort)
            {
                printf("El metodo no converge. \n");
                exit(404);
            }
            errort = error;

            for(i = 0; i<= n-1; i++)
            {
                xViejo[i] = xNuevo[i];
        }
    } while (error>tolerancia && iteracion <= 1000);
    printf("Matriz resultante:\n");
    printf("x1 :%f\n", xNuevo[0]);
    printf("x2: %f\n", xNuevo[1]);
    printf("x3: %f\n", xNuevo[2]);
    printf("x4: %f\n", xNuevo[3]);

    printf("Error: +-%f\n", error);
    printf("Iteraciones: %d\n", iteracion);
}