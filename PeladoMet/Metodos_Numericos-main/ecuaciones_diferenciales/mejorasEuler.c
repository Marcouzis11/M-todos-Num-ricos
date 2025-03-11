#include <stdio.h>
#include <math.h>


int menu();

void heun();

void middlePoint();

void rk4();

double f(double x, double y) {
    return (4*y- pow(x,2))*y;
}

double F(double x){
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
                heun();
                break;
            case 2:
                middlePoint();
                break;
            case 3:
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
    printf("Seleccione el metodo a utilizar: \n1- Heun \n2- Punto Medio \n3- RK4 \n0- Finalizar programa\n");
    printf("---------------------------------------------------------\n---------------------------------------------------------\n");
    scanf("%d", &op);
    return op;
}

void heun() {
    double xf, xn, xv, y, yt, h;
    int n;


    printf("Ingrese el valor inicial (x0) \n");
    scanf("%lf", &xv);
    printf("Ingrese el valor final (xf) \n");
    scanf("%lf", &xf);
    printf("Ingrese el dato inicial (y0) \n");
    scanf("%lf", &y);
    printf("Ingrese la cantidad de subintervalos (n) \n");
    scanf("%d", &n);

    h = (xf - xv) / n;

    FILE *file;

    file = fopen("resultadosHeun.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }


    for (int i = 0; i < n; ++i) {

        yt = y + h * f(xv, y);
        xn = xv + h;
        y = y + h * (f(xv, y) + f(xn, yt)) / 2;
        xv = xn;

        fprintf(file, "\t\t%lf\t%lf\n", xn, y);
    }

}

void middlePoint() {
    double xf, xm, xv, y, ym, h;
    int n;

    printf("Ingrese el valor inicial (x0) \n");
    scanf("%lf", &xv);
    printf("Ingrese el valor final (xf) \n");
    scanf("%lf", &xf);
    printf("Ingrese el dato inicial (y0) \n");
    scanf("%lf", &y);
    printf("Ingrese la cantidad de subintervalos (n) \n");
    scanf("%d", &n);

    h = (xf - xv) / n;

    FILE *file;

    file = fopen("resultadosPM.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }


    for (int i = 0; i < n; ++i) {

        ym = y + f(xv, y) * h / 2;
        xm = xv + h / 2;
        y = y + h * f(xm, ym);
        xv = xv + h;

        fprintf(file, "%lf\t%lf\n", xv, y);
    }

}

void rk4() {
    double xf, x, y, h, k1, k2, k3, k4;
    int n;

    printf("Ingrese el valor inicial (x0) \n");
    scanf("%lf", &x);
    printf("Ingrese el valor final (xf) \n");
    scanf("%lf", &xf);
    printf("Ingrese el dato inicial (y0) \n");
    scanf("%lf", &y);
    printf("Ingrese la cantidad de subintervalos (n) \n");
    scanf("%d", &n);

    h = (xf - x) / n;

    FILE *file;

    file = fopen("resultadosRK4.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }


    for (int i = 0; i < n; ++i) {
        k1 = f(x, y);
        k2 = f(x + h, y + k1 * h * 1 / 2);
        k3 = f(x + h / 2, y + k2 * h * 1 / 2);
        k4 = f(x + h, y + k3 * h);

        y = y + h * (k1 + 2 * k2 + 2 * k3 + k4) * 1 / 6;
        x = x + h;

        fprintf(file, "%lf\t%lf\n", x, y);

    }
}
