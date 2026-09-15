#include <stdio.h>
#include <math.h>

double f(double);
double fPrima(double);
void NewtonRaphson();


int main (){
    NewtonRaphson();
    return 0;
}

//NEWTON RAPHSON UTILIZA LA DERIVADA EXACTA.
double f(double x){
    return log(x*x + 1) -sin (x);////
}

//DERIVADA EXACTA
double fPrima(double x){
    return (2*x)/(x*x + 1) - cos(x);////
}

void NewtonRaphson(){
    double xViejo , error , tolerancia , xNuevo;
    int contador = 0;
    printf("Ingrese la tolerancia\n"); 
    scanf("%lf", &tolerancia);
    printf("Ingrese el punto inicial\n"); 
    scanf("%lf", &xViejo);

    do{
        contador ++ ;
        
        if(fabs(fPrima(xViejo)) < 1e-5)
        {
            printf("\n********\nDERIVADA PEQUEÑA********\n");
            break;
        } 
        else
        {
            xNuevo = xViejo - (f(xViejo)/fPrima(xViejo));
            error = fabs(xNuevo - xViejo);
            xViejo = xNuevo;
        }

        printf("ITERACIÓN %d:\n", contador);
        printf("c=%.10lf\n", xNuevo);
        printf("error=%.10lf\n\n", error);

    }while(error > tolerancia && contador < 10000 );

    printf("\n\n\nLa raiz de f es: %.10lf", xNuevo);
    printf("\nEl valor del error en la raiz es de: %.10lf", error);
    printf("\nLa resolucion del problema tomó %d iteraciones", contador);
}