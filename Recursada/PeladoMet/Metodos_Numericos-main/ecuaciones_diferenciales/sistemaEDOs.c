#include <stdio.h>
#include <math.h>

#define MAXTAM 102

int menu();

void euler();

void rk4();

double f1(double x, double y1, double y2, double y3) {
    return -y1 * x + 4 * sin(x);
}

double f2(double x, double y1, double y2, double y3) {
    return -y1 + 4 * cos(x);
}

double f3(double x, double y1, double y2, double y3) {
    return 0;
}

int main(int argc, char *argv[]) {
    int option;

    do {
        option = menu();
        switch (option) {
            case 0:
                break;
            case 1:
                euler();
                break;
            case 2:
                rk4();
                break;
            default:
                printf("La opcion ingresada no es valida. Porfavor ingrese una opcion valida\n");
                break;
        }
    } while (option != 0);

    return 0;

}

int menu() {
    int op = 0;
    printf("\n---------------------------------------------------------\n---------------------------------------------------------\n");
    printf("Seleccione el metodo a utilizar: \n1- Euler \n2- RK4 \n0- Finalizar programa\n");
    printf("---------------------------------------------------------\n---------------------------------------------------------\n");
    scanf("%d", &op);
    return op;
}

void euler() {
    double x[MAXTAM], y1[MAXTAM], y2[MAXTAM], y3[MAXTAM];
    double h;
    int n;

    printf("Ingrese el valor inicial (x0) \n");
    scanf("%lf", &x[0]);
    printf("Ingrese el valor final (xf) \n");
    scanf("%lf", &x[MAXTAM - 1]);
    printf("Ingrese el dato inicial (y10) \n");
    scanf("%lf", &y1[0]);
    printf("Ingrese el dato inicial (y20) \n");
    scanf("%lf", &y2[0]);
    printf("Ingrese el dato inicial (y30) \n");
    scanf("%lf", &y3[0]);
    printf("Ingrese la cantidad de subintervalos (n) \n");
    scanf("%d", &n);

    h = (x[MAXTAM - 1] - x[0]) / n;

    FILE *file;

    file = fopen("resultadosEulerSistEDOs.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    fprintf(file, "\tx\t\t\ty1\t\t\ty2\t\t\ty3\n");

    for (int i = 0; i < n + 1; ++i) {

        x[i + 1] = x[i] + h;
        y1[i + 1] = y1[i] + h * f1(x[i], y1[i], y2[i], y3[i]);
        y2[i + 1] = y2[i] + h * f2(x[i], y1[i], y2[i], y3[i]);
        y3[i + 1] = y3[i] + h * f3(x[i], y1[i], y2[i], y3[i]);
        fprintf(file, "%lf\t%lf\t%lf\t%lf\n", x[i], y1[i], y2[i], y3[i]);
        //fprintf(file, "%lf\t%lf\n", x[i], y2[i]);

    }

}

void rk4() {
    double x[MAXTAM], y1[MAXTAM], y2[MAXTAM], y3[MAXTAM], k1[3], k2[3], k3[3], k4[3];
    double h;
    int n;

    printf("Ingrese el valor inicial (x0) \n");
    scanf("%lf", &x[0]);
    printf("Ingrese el valor final (xf) \n");
    scanf("%lf", &x[MAXTAM - 1]);
    printf("Ingrese el dato inicial (y10) \n");
    scanf("%lf", &y1[0]);
    printf("Ingrese el dato inicial (y20) \n");
    scanf("%lf", &y2[0]);
    printf("Ingrese el dato inicial (y30) \n");
    scanf("%lf", &y3[0]);
    printf("Ingrese la cantidad de subintervalos (n) \n");
    scanf("%d", &n);

    h = (x[MAXTAM - 1] - x[0]) / n;

    FILE *file;

    file = fopen("resultadosRK4SistEDOs.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    fprintf(file, "\tx\t\t\ty1\t\t\ty2\t\t\ty3\n");

    fprintf(file, "%lf\t%lf\t%lf\t%lf\n", x[0], y1[0], y2[0], y3[0]);

    for (int i = 1; i < n + 1; ++i) {
        x[i] = x[0] + i * h;

        k1[0] = f1(x[i - 1], y1[i - 1], y2[i - 1], y3[i - 1]);
        k1[1] = f2(x[i - 1], y1[i - 1], y2[i - 1], y3[i - 1]);
        k1[2] = f3(x[i - 1], y1[i - 1], y2[i - 1], y3[i - 1]);

        k2[0] = f1(x[i - 1] + h / 2., y1[i - 1] + k1[0] * h / 2., y2[i - 1] + k1[1] * h / 2.,
                   y3[i - 1] + k1[2] * h / 2.);
        k2[1] = f2(x[i - 1] + h / 2., y1[i - 1] + k1[0] * h / 2., y2[i - 1] + k1[1] * h / 2.,
                   y3[i - 1] + k1[2] * h / 2.);
        k2[2] = f3(x[i - 1] + h / 2., y1[i - 1] + k1[0] * h / 2., y2[i - 1] + k1[1] * h / 2.,
                   y3[i - 1] + k1[2] * h / 2.);

        k3[0] = f1(x[i - 1] + h / 2., y1[i - 1] + k2[0] * h / 2., y2[i - 1] + k2[1] * h / 2.,
                   y3[i - 1] + k2[2] * h / 2.);
        k3[1] = f2(x[i - 1] + h / 2., y1[i - 1] + k2[0] * h / 2., y2[i - 1] + k2[1] * h / 2.,
                   y3[i - 1] + k2[2] * h / 2.);
        k3[2] = f3(x[i - 1] + h / 2., y1[i - 1] + k2[0] * h / 2., y2[i - 1] + k2[1] * h / 2.,
                   y3[i - 1] + k2[2] * h / 2.);

        k4[0] = f1(x[i - 1] + h / 2., y1[i - 1] + k3[0] * h, y2[i - 1] + k3[1] * h, y3[i - 1] + k3[2] * h);
        k4[1] = f2(x[i - 1] + h / 2., y1[i - 1] + k3[0] * h, y2[i - 1] + k3[1] * h, y3[i - 1] + k3[2] * h);
        k4[2] = f3(x[i - 1] + h / 2., y1[i - 1] + k3[0] * h, y2[i - 1] + k3[1] * h, y3[i - 1] + k3[2] * h);

        y1[i] = y1[i - 1] + h * (k1[0] + 2. * k2[0] + 2. * k3[0] + k4[0]) / 6.;
        y2[i] = y2[i - 1] + h * (k1[1] + 2. * k2[1] + 2. * k3[1] + k4[1]) / 6.;
        y3[i] = y3[i - 1] + h * (k1[2] + 2. * k2[2] + 2. * k3[2] + k4[2]) / 6.;

        fprintf(file, "%lf\t%lf\t%lf\t%lf\n", x[i], y1[i], y2[i], y3[i]);
        //fprintf(file, "%lf\t%lf\n", x[i], y1[i]);
    }


}
