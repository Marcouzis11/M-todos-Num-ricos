#include <stdio.h>
#include <math.h>

double f(double x) {
    return exp(sqrt(1 + x)) * log(1 + 2 * pow(x, 2));
}

int main(int argc, char *argv[]) {
    double x0, xf, h, result, x;
    int n;
    printf("Ingrese la cantidad de subintervalos (n) \n");
    scanf("%d", &n);
    printf("Ingrese el valor inicial (x0) \n");
    scanf("%lf", &x0);
    printf("Ingrese el valor final (xf) \n");
    scanf("%lf", &xf);

    h = (xf - x0) / n;

    FILE *file;

    file = fopen("tabla.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return 1;
    }

    for (int i = 0; i < n + 1; ++i) {
        x = x0 + i * h;
        result = f(x);

        fprintf(file, "%lf\t%lf\n", x, result);
    }

    return 0;
}
