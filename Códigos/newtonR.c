#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float f(float);
float fp(float);

int main(){
float xViejo = 0;
float xNuevo = 0;
int iteracion = 0;
float tolerancia = 0;
float error = 0;
int cantIt = 0;

printf("Introduzca xViejo:\n");
scanf("%f", &xViejo);
printf("Introduzca la cantidad de iteraciones: \n");
scanf("%d", &cantIt);
printf("Introduzca la tolerancia: \n");
scanf("%f", &tolerancia);

do
{
    iteracion++;
    xNuevo = xViejo - (f(xViejo)/fp(xViejo));
    if(fabs(fp(xNuevo))<pow(10,-10))
    {
        printf("La derivada es muy grande. \n");
        exit(1);
    }
    error = fabs(xNuevo-xViejo);
    xViejo = xNuevo;
}
while (error > tolerancia && iteracion<=cantIt );

if(f(xNuevo < tolerancia))
{
    printf("Raíz : %f +- %f\n Iteracion: %d", xNuevo, error, iteracion);
}
}


float f (float x)
{
    float r = 0;
    r = x * x;
    return r;
}

float fp (float x)
{
    float r = 0;
    r = 2 * x;
    return r;
}