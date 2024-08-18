#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float f(float);

int main(int argc, char *argv[])
{
float a = 0;
float b = 0;
float c = 0;
float tolerancia;
float error;
float Xr;

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
printf("Introduzca la tolerancia: \n");
scanf("%f",&tolerancia);

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
} while (error > tolerancia);
printf("Raíz: %f +- %f", c,error);
    return 0;
    }


float f (float x)
{
    float r = 0; 
    r = -2 + 7*x -5*pow(x,2) +6*pow(x,3);
    return r;
};