#include <stdio.h>
#include <math.h>


int menu();

void firstOrder();

void secondOrder();

void thirdOrder();

void fourthOrder();

double f(double x) {
    return ;
}

int main(int argc, char *argv[]) {

    int option;

    do {
        option = menu();
        switch (option) {
            case 0:
                break;
            case 1:
                firstOrder();
                break;
            case 2:
                secondOrder();
                break;
            case 3:
                thirdOrder();
                break;
            case 4:
                fourthOrder();
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
    printf("Seleccione el metodo a utilizar: \n1- Orden 1 \n2- Orden 2 \n3- Orden 3 \n4- Orden 4 \n0- Finalizar programa\n");
    printf("---------------------------------------------------------\n---------------------------------------------------------\n");
    scanf("%d", &op);
    return op;
}

void firstOrder() {
    double a, b, x;
    int n;
    double derivative = 0;

    FILE *file;

    file = fopen("derivadas.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    printf("Ingrese el valor inicial\n");
    scanf("%lf", &a);
    printf("Ingrese el valor final\n");
    scanf("%lf", &b);
    printf("Ingrese la cantidad de subintervalos\n");
    scanf("%d", &n);
    double h = (b - a) / n;

    for (int i = 0; i < 2; ++i) {
        x = a + i * h;
        derivative = (-f(x + 2 * h) + 4 * f(x + h) - 3 * f(x)) / (2 * h);
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }

    for (int i = 2; i < n - 1; ++i) {
        x = a + i * h;
        derivative = (-f(x + 2 * h) + 8 * f(x + h) - 8 * f(x - h) + f(x - 2 * h)) / (12 * h);
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }

    for (int i = n - 1; i < n + 1; ++i) {
        x = a + i * h;
        derivative = (3 * f(x) - 4 * f(x - h) + f(x - 2 * h)) / (2 * h);
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }

}

void secondOrder() {
    double a, b, x;
    int n;
    double derivative = 0;

    FILE *file;

    file = fopen("derivadas.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    printf("Ingrese el valor inicial\n");
    scanf("%lf", &a);
    printf("Ingrese el valor final\n");
    scanf("%lf", &b);
    printf("Ingrese la cantidad de subintervalos\n");
    scanf("%d", &n);
    double h = (b - a) / n;

    for (int i = 0; i < 3; ++i) {
        x = a + i * h;
        derivative = (-f(x + 3 * h) + 4 * f(x + 2 * h) - 5 * f(x + h) + 2 * f(x)) / (pow(h, 2));
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }

    for (int i = 2; i < n - 1; ++i) {
        x = a + i * h;
        derivative = (-f(x + 2 * h) + 8 * f(x + h) - 8 * f(x - h) + f(x - 2 * h)) / (12 * h);
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }

    for (int i = n - 1; i < n + 1; ++i) {
        x = a + i * h;
        derivative = (2 * f(x) - 5 * f(x - h) + 4 * f(x - 2 * h) - f(x - 3 * h) / (pow(h, 2)));
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }
}

void thirdOrder() {
    double a, b, x;
    int n;
    double derivative = 0;

    FILE *file;

    file = fopen("derivadas.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    printf("Ingrese el valor inicial\n");
    scanf("%lf", &a);
    printf("Ingrese el valor final\n");
    scanf("%lf", &b);
    printf("Ingrese la cantidad de subintervalos\n");
    scanf("%d", &n);
    double h = (b - a) / n;

    for (int i = 0; i < 3; ++i) {
        x = a + i * h;
        derivative = (-3 * f(x + 4 * h) + 14 * f(x + 3 * h) - 24 * f(x + 2 * h) + 18 * f(x + h) - 5 * f(x)) /
                     (2 * pow(h, 3));
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }

    for (int i = 3; i < n - 2; ++i) {
        x = a + i * h;
        derivative =
                (-f(x + 3 * h) + 8 * f(x + 2 * h) - 13 * f(x + h) + 13 * f(x - h) - 8 * f(x - 2 * h) + f(x - 3 * h)) /
                (8 * pow(x, 3));
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }

    for (int i = n - 2; i < n + 1; ++i) {
        x = a + i * h;
        derivative =
                (5 * f(x) - 18 * f(x - h) + 24 * f(x - 2 * h) - 14 * f(x - 3 * h) + 3 * f(x - 4 * h)) / (2 * pow(h, 3));
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }
}

void fourthOrder() {
    double a, b, x;
    int n;
    double derivative = 0;

    FILE *file;

    file = fopen("derivadas.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    printf("Ingrese el valor inicial\n");
    scanf("%lf", &a);
    printf("Ingrese el valor final\n");
    scanf("%lf", &b);
    printf("Ingrese la cantidad de subintervalos\n");
    scanf("%d", &n);
    double h = (b - a) / n;

    for (int i = 0; i < 3; ++i) {
        x = a + i * h;
        derivative = (-2 * f(x + 5 * h) + 11 * f(x + 4 * h) - 24 * f(x + 3 * h) + 26 * f(x + 2 * h) - 14 * f(x + h) +
                      3 * f(x)) / (pow(h, 4));;
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }

    for (int i = 3; i < n - 2; ++i) {
        x = a + i * h;
        derivative =
                (-f(x + 3 * h) + 12 * f(x + 2 * h) - 39 * f(x + h) + 56 * f(x) - 39 * f(x - h) + 12 * f(x - 2 * h) -
                 f(x - 3 * h)) / (6 * pow(h, 4));;
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }

    for (int i = n - 2; i < n + 1; ++i) {
        x = a + i * h;
        derivative = (3 * f(x) - 14 * f(x - h) + 26 * f(x - 2 * h) - 24 * f(x - 3 * h) + 11 * f(x - 4 * h) -
                      2 * f(x - 5 * h)) / (pow(h, 4));;
        fprintf(file, "%lf\t%lf\n", x, derivative);
    }
}
