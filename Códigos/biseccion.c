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
int errPorcAprox;
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
printf("Introduzca la tolerancia: \n");
scanf("%f",&tolerancia);
//printf("Elija su tipo de error:\n 1- Error absoluto exacto.\n 2- Error absoluto aproximado.\n 3- Error porcentual exacto.\n 4- Error porcentual aproximado.");
/*switch(opcion)
{
    case 1:errAbsExact = 1;
    break; 
    case 2:errAbsAprox = 1;
    break;
    case 3:errPorcExact = 1;
    break;
    case 4:errPorcAprox = 1;
    break;
};*/



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
    r = -2 + 7*x -5*x*x+6*x*x*x;
    return r;
};