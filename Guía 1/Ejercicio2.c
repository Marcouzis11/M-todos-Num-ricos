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
int iteracion = 0;
float Xr;
int opcion;


printf("Elija:\n -1 Biseccion. \n -2 Regula Falsi\n");
scanf("%d", &opcion);

switch(opcion)
{
    case 1:
    printf("# BISECCION #\n");
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
                printf("Raíz exacta encontrada: %f\n", c);
                printf("Cantidad de iteraciones: %d\n", iteracion);
                exit(0);
            }
        }
        error = (b-a)/2;
    } while (error > tolerancia);
    printf("Raíz: %f +- %f", c,error);
    printf("Cantidad de iteraciones: %d\n", iteracion);
    break;

    case 2:
    printf("# REGULA FALSI #\n");
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
        iteracion++;
        c = ((a*f(b))-(b*f(a)))/(f(b)-f(a));
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
                printf("Raíz exacta encontrada: %f\n", c);
                printf("Cantidad de iteraciones: %d\n", iteracion);
                exit(0);
            }
        }
        error = fabs(b-a)/2;
    } while (error > tolerancia && iteracion<1500000);
    printf("Raíz: %f +- %f", c,error);
    printf("Cantidad iteraciones: %d\n", iteracion);
    break;
    }
    
return 0;
}


float f (float x)
{
    float r = 0; 
    r = pow(x,10) - 1;
    return r;
};