/*
!IMPORTANTE
!ANTES DE COMENZAR DESPEJAR f(x) de manera que quede g(x) = x;
!SI NO HAGO ESTO NADA VA A FUNCIONAR
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double g (double);

int main()
{
    double xViejo = 0;
    double xNuevo = 0;
    double tol = 0;
    double error = 0;
    int iteracion = 0;
    double a;

    printf("Ingrese el Xviejo: \n");
    scanf("%lf", &xViejo);
    printf("Ingrese la tolerancia: \n");
    scanf("%lf", &tol);

    if(fabs((g(xViejo+0.01)-g(xViejo))/0.01)>1)
        {
            a = fabs((g(xViejo+0.01)-g(xViejo))/0.01);
            printf("%lf \n",a);
            printf("No se cumple el criterio de convergencia. \n");
            exit(1);
        }
    do
    {
        iteracion++;
        xNuevo = g(xViejo);
        error = fabs(xNuevo-xViejo);
        xViejo=xNuevo;
    }
    while(error > tol);
    printf("Raíz: %.12lf +- %.12lf \n", xNuevo, error);
    printf("Iteraciones: %d", iteracion);
}

double g (double x)
{    
    //Func original: 3x +sin(x) - e^x = 0
    // El de abajo es g(x) = x
    return -(sin(x)-exp(x))/3;
}