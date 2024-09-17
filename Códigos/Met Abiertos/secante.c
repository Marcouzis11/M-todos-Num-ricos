/*
!IMPORTANTE
!ANTES DE COMENZAR DESPEJAR f(x) de manera que quede g(x) = x;
!SI NO HAGO ESTO NADA VA A FUNCIONAR
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float g (float);

int main()
{
    float xViejoViejo;
    float xViejo = 0;
    float xNuevo = 0;
    float tol = 0;
    float error = 0;
    int iteracion = 0;
    float a;

    printf("Ingrese el xViejoViejo: \n");
    scanf("%f", &xViejoViejo);
    printf("Ingrese el Xviejo: \n");
    scanf("%f", &xViejo);
    printf("Ingrese la tolerancia: \n");
    scanf("%f", &tol);

    if(fabs((g(xViejo+0.01)-g(xViejo))/0.01)>1)
        {
            a = fabs((g(xViejo+0.01)-g(xViejo))/0.01);
            printf("%f \n",a);
            printf("No se cumple el criterio de convergencia. \n");
            exit(1);
        }
    do
    {
        iteracion++;
        float denom = g(xViejoViejo) - g(xViejo);

        // Verifica que el denominador no sea cercano a cero
        if (fabs(denom) < 1e-12) {
            printf("El denominador es muy pequeño o cero en la iteración %d.\n", iteracion);
            exit(1);
        }
        xNuevo = xViejo - (g(xViejo)*(xViejoViejo - xViejo))/(g(xViejoViejo) - g(xViejo));
        error = fabs(xNuevo-xViejo);
        xViejoViejo = xViejo;
        xViejo=xNuevo;
    }
    while(error > tol && iteracion <= 1000);
    printf("Raíz: %.12f +- %.12f \n", xNuevo, error);
    printf("Iteraciones: %d", iteracion);
}

float g (float x)
{    
    //Func original: pow(x,2) + x - 4
    // El de abajo es g(x) = x
    return 4/(x+2);
}