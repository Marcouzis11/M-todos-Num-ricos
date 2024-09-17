#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
float f(float);

int main(int argc, char *argv[])
{
float a = 0;
float b = 0;
float c = 0;
float tolerancia;
float error;
float Xr;
int errAbsExact;
int errAbsAprox;
int errPorcExact;
float errPorcAprox;
int opcion;

printf("Introduzca 'a': \n");
scanf("%f", &a);
printf("Introduzca 'b': \n");
scanf("%f", &b);
printf("Verificando si es posible calcular la raíz de la función.\n");
if(f(a)*f(b) < 0)
{
    printf("Es posible calcular la raíz.\n");
}
else
{
    printf("No es posible calcular la raíz.\n");
    exit(1);
}
/*printf("Introduzca la tolerancia: \n");
scanf("%f",&tolerancia);*/

do
{
    c = (a+b)/2;
    if((f(a)*f(c)) > 0)
    {
        a = c;
    }
    else
    {
        if((f(a)*f(c)) < 0)
        {
            b = c;
        }
        else
        {
            printf("Raíz exacta encontrada: %f", c);
            exit(0);
        }
    }
    error = (b-a)/2;
    errPorcAprox = error / c;
} while (errPorcAprox > 0.001);
printf("Raíz: %f +- %f\n", c,error);
printf("Error aprox porc: %f ", errPorcAprox);
    return 0;
    }


float f (float x)
{
    float r = 0; 
    r = -3.000001 + 4.233335*x -1.35*x*x+0.116667*x*x*x;
    return r;
};