#include <stdio.h>
#include <math.h>

int menu();

void trapeze();

void simpson();

double f(double x) {
    return 3*pow(x,2)+1;
}

int main(int argc, char *argv[]) {
    int option;

    do {
        option = menu();
        switch (option) {
            case 0:
                break;
            case 1:
                trapeze();
                break;
            case 2:
                simpson();
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
    printf("Seleccione el metodo a utilizar: \n1- Trapecio Compuesta \n2- Simpson Compuesta \n0- Finalizar programa\n");
    printf("---------------------------------------------------------\n---------------------------------------------------------\n");
    scanf("%d", &op);
    return op;
}

void trapeze() {
    double a, b;
    int intervals;
    printf("Ingrese el limite de integracion inferior\n");
    scanf("%lf", &a);
    printf("Ingrese el limite de integracion superior\n");
    scanf("%lf", &b);
    printf("Ingrese la cantidad de intervalos\n");
    scanf("%d", &intervals);

    double h = (b - a) / intervals;

    /*double sum1, sum2, sum;
    //Alternativa de calculo parcial II
    for (int i = 0; i < intervals; ++i) {
        sum1 = sum1 + h * f(a + i * h);
    }
    for (int i = 0; i < intervals; ++i) {
        sum2 = sum2 + h * (f(a + (i + 1) * h) - f(a + i * h));
    }
    sum2 = sum2 / 2;
    sum = sum1 + sum2;*/

    //Metodo de trapecio compuesto original

    double sum = f(a) + f(b);
    for (int i = 1; i < intervals; ++i) {
        sum = sum + 2 * f(a + i * h);
    }
    sum = sum * (h / 2);

    printf("El valor de la integral es de: %lf", sum);

}

void simpson() {

    double a, b;
    int intervals;
    printf("Ingrese el limite de integracion inferior\n");
    scanf("%lf", &a);
    printf("Ingrese el limite de integracion superior\n");
    scanf("%lf", &b);
    printf("Ingrese la cantidad de intervalos\n");
    scanf("%d", &intervals);

    if (intervals % 2 != 0) {
        printf("El numero de intervalos debe ser par\n");
        return;
    }

    double h = (b - a) / intervals;
    double sum = f(a) + f(b);

    for (int i = 1; i < (intervals / 2); ++i) {
        double x = a + 2 * i * h;
        sum = sum + 2 * f(x) + 4 * f(x - h);
    }

    sum = (h / 3) * (sum + 4 * f(b - h));

    printf("El valor de la integral es de: %.30lf", sum);
}