#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double f(double);
double fp(double, double);

int main(){
double xViejo = 0;
double xNuevo = 0;
int iteracion = 0;
double tolerancia = 0;
double error = 0;
double h = 0;
int cantIt = 0;

printf("Introduzca xViejo:\n");
scanf("%lf", &xViejo);
printf("Introduzca h:\n");
scanf("%lf", &h);
printf("Introduzca la cantidad de iteraciones: \n");
scanf("%d", &cantIt);
printf("Introduzca la tolerancia: \n");
scanf("%lf", &tolerancia);


do
{
    iteracion++;
    xNuevo = xViejo - (f(xViejo)/fp(xViejo,h));
    if(fabs(fp(xNuevo,h))<pow(10,-10))
    {
        printf("La derivada es muy grande. \n");
        exit(1);
    }
    error = fabs(xNuevo-xViejo);
    xViejo = xNuevo;
}
while (error > tolerancia && iteracion<=cantIt );
//while(error > 0,00000001);

if(f(xNuevo < tolerancia))
{
    printf("Raíz : %.10lf +- %.10lf\n Iteracion: %d", xNuevo, error, iteracion);
}
}


double f (double x)
{
    double r = 0;
    r = 3*x + sin(x) - exp(x);
    return r;
}

double fp (double x, double h)
{
    double r = 0;
    r = (f(x+h)-f(x-h))/(2*h);
    return r;
}