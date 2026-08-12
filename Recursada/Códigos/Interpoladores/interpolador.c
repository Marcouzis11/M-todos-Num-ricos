#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
//#include <iostream>

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


    //? LAGRANGE
    double x = 0;
    double suma = 0;
    double error = 0;
    int k = 0;
    int z = 0;
    double prod = 0;
    double divisor = 0;
    printf("Introduzca valor a interpolar.\n");
    scanf("%lf",&x); // x casita

    suma = 0;

    for(k = 0; k<=filas-1;k++)
    {
        prod = 1;
        for(z = 0; z<=filas-1;z++)
        {
            if(z != k)
            {
            divisor = m[k][0]-m[z][0];
            prod = prod * (x-m[z][0])/divisor;
            printf("Div= %lf\n", divisor);
            printf("Prod= %lf\n", prod);
            }
        }
    suma = suma + m[k][1] * prod;
    printf("Suma= %lf\n", suma);
    }
    printf("Suma / Pn(xcasita) = %lf con valor interpolado %lf\n", suma, x);
    error = fabs(suma-f(x));
    printf("Error = %f\n", error);
}



float f (float x)
{
    return x + 2/x;
}

