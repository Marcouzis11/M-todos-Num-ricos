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
        return 1;
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
    double m[filas][2]; // Solo 2 columnas: x e y

    // Cargo los datos leidos en el array
    int i, j;
	for(i = 0; i < filas; i++) {
		j = 0;
		do {
            fscanf(fp, "%lf", &(m[i][j]));
            j++;
		} while(j < 2 && (c = fgetc(fp)) != '\n'); // Solo se esperan 2 columnas
	}

	columnas = 2; // Solo 2 columnas: x e y
	
	printf("numero de columnas= %i\n",columnas);

    // imprimo la matriz para verificar que lo leyo correctamente
    for(i = 0; i < filas; i++) {
        for(j = 0; j < columnas; j++) {
            printf("%lf ", m[i][j]);
        }
        printf("\n");
    }

    //? REGRESION POLINOMIAL
    int orden;
    printf("Ingrese el orden del polinomio:\n");
    scanf("%d", &orden);

    if(orden < 1) {
        printf("No se puede usar orden 0 o negativo.\n");
        exit(1);
    }

    if(filas < orden + 1) {
        printf("No hay datos suficientes.\n");
        exit(1);
    }

    double matriz[orden+1][orden+1];
    double matrizR[orden+1];


    //Relleno con ceros las matrices que voy a utilizar
    for(i = 0; i <= orden; i++) 
    {
        matrizR[i] = 0;
        for(j = 0; j <= orden; j++) 
        {
            matriz[i][j] = 0;
        }
    }

    for(i = 0; i <= orden; i++) 
    {
        for(j = 0; j < filas; j++) 
        {
            matrizR[i] += m[j][1] * pow(m[j][0], i);
        }
        for(j = 0; j <= orden; j++) 
        {
            for(int k = 0; k < filas; k++) 
            {
                matriz[i][j] += pow(m[k][0], i + j);
            }
        }
    }

    // Matriz del sistema
    printf("\nMatriz del sistema:\n");
    for(i = 0; i <= orden; i++) 
    {
        for(j = 0; j <= orden; j++) 
        {
            printf("%lf ", matriz[i][j]);
        }
        printf("| %lf\n", matrizR[i]);
    }


    return 0;
}

