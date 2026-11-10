#include <stdio.h>
#include <math.h>

#define MAX_ITER 10000

double funcion(double x);

void regulaFalsi();

int main() {

    regulaFalsi();

    return 0;
}

double funcion(double x) {

    return log(x) + exp(sin(x)) - x;

}

void regulaFalsi() {

    double a, b, tolerancia, c, cViejo, error;
    double fa, fb, fc;
    int contador = 0;

    printf("Ingrese el límite inferior del intervalo: ");
    scanf("%lf", &a);
    printf("Ingrese el límite superior del intervalo: ");
    scanf("%lf", &b);
    printf("Ingrese la tolerancia: ");
    scanf("%lf", &tolerancia);

    /* Si el intervalo viene invertido lo acomoda */
    if (a > b) {
        double aux = a;
        a = b;
        b = aux;
        printf("\nAviso: se intercambiaron los extremos del intervalo\n");
    }

    fa = funcion(a);
    fb = funcion(b);

    /* Sin esto, un f(a) infinito hace que la fórmula dé inf/inf = NaN */
    if (!isfinite(fa) || !isfinite(fb)) {
        printf("\nLa función no está definida en algún extremo del intervalo\n");
        return;
    }

    /* Raíz exacta en un extremo: el producto daría 0 y no entraría al bucle */
    if (fa == 0.0) {
        printf("\nLa raíz exacta es el extremo inferior: %.10lf\n", a);
        return;
    }
    if (fb == 0.0) {
        printf("\nLa raíz exacta es el extremo superior: %.10lf\n", b);
        return;
    }

        if (fa * fb < 0.0) {

            c = a;   /* semilla, para que la primera resta tenga sentido */

            do {
                cViejo = c;

                c = (a*fb - b*fa) / (fb - fa);
                fc = funcion(c);

                
                error = fabs(c - cViejo);
                contador++;

                if (fa * fc > 0) {
                    a = c;
                    fa = fc;
                } else if (fa * fc < 0) {
                    b = c;
                    fb = fc;
                } else {
                    error = 0.0;   /* f(c) == 0, la raíz es exacta */
                    break;
                }

            } while (error > tolerancia && contador < MAX_ITER);

            if (contador >= MAX_ITER) {
                printf("\nNo se alcanzó la tolerancia en %d iteraciones\n", MAX_ITER);
            }

            printf("\nResultado de la raíz: %.10lf", c);
            printf("\nError estimado: %.10lf", error);
            printf("\nResiduo |f(c)|: %.3e", fabs(fc));
            printf("\nCantidad de iteraciones: %d\n", contador);

        } else {
            printf("\nNo hay raíz o hay un número par de ellas\n");
        }

}
