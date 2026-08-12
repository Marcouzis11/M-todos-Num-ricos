#include <stdio.h>
#include <math.h>

#define MAXROW 10
#define MAXCOL 2

int menu();

void trapeze(double[MAXROW][MAXCOL], int);

void simpson(double[MAXROW][MAXCOL], int);

void readTxt(double[MAXROW][MAXCOL], int *);


int main(int argc, char *argv[]) {
    int option;
    double nodes[MAXROW][MAXCOL];
    int rows;
    readTxt(nodes, &rows);

    do {
        option = menu();
        switch (option) {
            case 0:
                break;
            case 1:
                trapeze(nodes, rows);
                break;
            case 2:
                simpson(nodes, rows);
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

void trapeze(double nodes[MAXROW][MAXCOL], int rows) {
    double a, b;
    int intervals = rows - 1;
    printf("Ingrese el limite de integracion inferior\n");
    scanf("%lf", &a);
    printf("Ingrese el limite de integracion superior\n");
    scanf("%lf", &b);

    double h = (b - a) / intervals;

    //Metodo de trapecio compuesto original

    //double sum = f(a) + f(b);
    double sum = nodes[0][1] + nodes[rows - 1][1];
    for (int i = 1; i < intervals; ++i) {
        //sum = sum + 2 * f(a + i * h);
        sum = sum + 2 * nodes[i][1];
    }
    sum = sum * (h / 2);

    printf("El valor de la integral es de: %lf", sum);

}

void simpson(double nodes[MAXROW][MAXCOL], int rows) {

    double a, b;
    printf("Ingrese el limite de integracion inferior\n");
    scanf("%lf", &a);
    printf("Ingrese el limite de integracion superior\n");
    scanf("%lf", &b);

    int intervals = rows - 1;

    double h = (b - a) / intervals;

    if (intervals % 2 != 0) {
        printf("El numero de intervalos debe ser par\n");
        return;
    }

    //double sum = f(a) + f(b);
    double sum = nodes[0][1] + nodes[rows - 1][1];

    for (int i = 1; i < (intervals / 2); ++i) {
        //x = a + 2*i*h;
        //sum = sum + 2 * f(x) + 4 * f(x - h);
        sum = sum + 2 * nodes[2 * i][1] + 4 * nodes[2 * i - 1][1];
    }

    //sum = (h / 3) * (sum + 4 * f(b - h));
    sum = (h / 3) * (sum + 4 * nodes[rows - 2][1]);

    printf("El valor de la integral es de: %.30lf", sum);
}

void readTxt(double m[MAXROW][MAXCOL], int *rows) {
    FILE *readPtr;
    int j, i, n;
    float k;
    readPtr = fopen("data.txt", "r");
    fscanf(readPtr, "%d", &n);
    *rows = n;

    for (i = 0; i < *rows; i++) {
        for (j = 0; j < MAXCOL; j++) {
            fscanf(readPtr, "%f", &k);
            m[i][j] = k;
        }
    }
    fclose(readPtr);
}
