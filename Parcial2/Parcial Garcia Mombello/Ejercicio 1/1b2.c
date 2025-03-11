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
int iteracion = 0;

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
    iteracion++;
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
    if(iteracion == 9 || iteracion == 10 || iteracion == 11)
    {
        printf("Raíz: %f +- %f\n", c,error);
        printf("Iteracion N° %d\n",iteracion);
    }
    //errPorcAprox = error / c;
} while (iteracion != 1000);
//printf("Error aprox porc: %f ", errPorcAprox);
    return 0;
    }


float f (float x)
{
    float r = 0; 
    r = log(x*x + 1) - sin(x); //log solo es logaritmo natural, log10 es log base 10
    return r;
};