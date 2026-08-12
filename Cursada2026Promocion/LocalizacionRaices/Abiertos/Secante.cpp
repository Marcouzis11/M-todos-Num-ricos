#include <stdio.h>
#include <math.h>

#define MAX_ITER 10000

double f(double x){
    return x*x -4;////
}

void Secante();


int main (){
    Secante();
    return 0;
}

void Secante(){
    double xViejoViejo , xViejo , xNuevo , error = 1e30 , tolerancia;
    int contador = 0;
    printf("Ingrese la tolerancia\n");
    scanf("%lf", &tolerancia);
    printf("Ingrese el primer punto inicial (xViejoViejo)\n");
    scanf("%lf", &xViejoViejo);
    printf("Ingrese el segundo punto inicial (xViejo)\n");
    scanf("%lf", &xViejo);


    do{
        contador ++ ;

        xNuevo = xViejo - f(xViejo)*(xViejoViejo - xViejo)/(f(xViejoViejo) - f(xViejo));
        error = fabs(xNuevo - xViejo);
        xViejoViejo = xViejo;
        xViejo = xNuevo;

    }while(error > tolerancia && contador < MAX_ITER );

    if(error > tolerancia)
        printf("\n\nATENCION: se corto sin alcanzar la tolerancia pedida.");

    printf("\n\n\nLa raiz de f es: %.10lf", xNuevo);
    printf("\nEl valor del error en la raiz es de: %.10lf", error);
    printf("\nLa resolucion del problema tomó %d iteraciones", contador);
}
