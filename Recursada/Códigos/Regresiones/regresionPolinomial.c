#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

float f (float);

int main(int argc, char* argv[])
{
    FILE *fp;
    char ch;
    fp = fopen("datosInterpolador.dat","r");
    if ( fp == NULL )
    {
        puts ( "No se puede abrir el archivo");
    }

    //contador de filas
    int filas=0;
    char c;
    int maxValues = 0;
	int columnas;
    
    while((c = fgetc(fp)) != EOF)
    {
        if(c == '\n'){
            filas++;
		}
    }

    printf("numero de filas=%i\n",filas);

    //cerramos y reabrimos el archivo para resetear el puntero
    fclose(fp);
    fp = fopen("datosInterpolador.dat","r");
    double m[filas][100];


    //Cargo los datos leidos en el array
    int i, j;
	for(i = 0; i < filas; i++) {
		j = 0;
		do {
        fscanf(fp, "%lf", &(m[i][j]));
        j++;
			} while((c = fgetc(fp)) != '\n');
	}

	columnas = j;
	
	printf("numero de columnas= %i\n",columnas);
    //imprimo la matriz para verificar que lo leyo correctamente
    
        for(i=0;i<filas;i++)
    {
        for(j=0;j<columnas;j++)
        {
            printf("%lf ",m[i][j]);
        }
        printf("\n");
    }

    //? REGRESION POLINOMIAL

    int orden = 0;
    float sumaxy = 0;
    float sumax = 0;
    int f,r = 0;
    float matrizR[filas];
    float matriz[filas][columnas];

    printf("Ingrese el orden del polinomio:\n");
    scanf("%d", &orden);
    if(orden < 1)
    {
        printf("No se puede orden 0 o negativo. \n");
        exit(1);
    }

    if(filas < orden+1)
    {
        printf("No hay datos suficientes. \n");
        exit (1);
    }

    for(f = 0;f<=orden;f++)
    {
        sumaxy = 0;
        
        for(i = 0;i<filas;i++)
        {
            sumaxy = sumaxy + m[i][1]*pow(m[i][0],f);
        }

        matrizR[f] = sumaxy;

        for(r = 0; r<=orden; r++)
        {
            sumax = 0;
            for(i = 0; i<filas; i++)
            {
                sumax = sumax + pow(m[i][0],f+r);
            }
            matriz[f][r] = sumax;
        }
    }


    //? Imprimir matriz a pivotear
    for(i = 0; i<=orden;i++)
    {
        for(r = 0; r<=orden;r++)
        {
            printf("%f ", matriz[i][r]);
        }
        printf("| %f", matrizR[i]);
        printf("\n");
    }

}