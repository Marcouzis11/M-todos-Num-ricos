#include <stdio.h>
#include <math.h>

#define MAXROW 10
#define MAXCOL 2
#define MAXCOL2 3


int menu();

void readTxtI(double[MAXROW][MAXCOL], int *);

void printMatrixI(double[MAXROW][MAXCOL], int);

void lagrange(double[MAXROW][MAXCOL], int);

void polynomial(double[MAXROW][MAXCOL], int);


void printMatrix(double [MAXROW][MAXCOL2], double [MAXROW], int, int);

void triangulation(double [MAXROW][MAXCOL2], double [MAXROW], int, int);

void retrosustitucion(double [MAXROW][MAXCOL2], double [MAXROW], double [MAXROW], int, int);

void pivot(double [MAXROW][MAXCOL2], double [MAXROW], int, int, int);

double f(double x) {
    return 1 - 0.01 * x + 0.001 * pow(x, 2);
}

int main(int argc, char *argv[]) {
    int option;
    double array[MAXROW][MAXCOL];
    int rows;
    readTxtI(array, &rows);
    printMatrixI(array, rows);
    do {
        option = menu();
        switch (option) {
            case 0:
                break;
            case 1:
                lagrange(array, rows);
                break;
            case 2:
                polynomial(array, rows);
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
    printf("Seleccione el metodo a utilizar: \n1- Lagrange \n2- Construccion del polinomio \n0- Finalizar programa\n");
    printf("---------------------------------------------------------\n---------------------------------------------------------\n");
    scanf("%d", &op);
    return op;
}

void readTxtI(double m[MAXROW][MAXCOL], int *rows) {
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

void printMatrixI(double m[MAXROW][MAXCOL], int rows) {
    printf("------ NODOS ------\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            printf(" %lf ", m[i][j]);
        }
        printf("\n");
    }
    printf("-------------------\n");
}

void printMatrix(double m[MAXROW][MAXCOL2], double b[MAXROW], int rows, int columns) {
    double diagonal = 1;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%E ", m[i][j]);
        }
        printf("%E", b[i]);
        diagonal = diagonal * m[i][i];
        printf("\n");
    }
    printf("Diagonal: %lf\n", diagonal);
}

void lagrange(double m[MAXROW][MAXCOL], int rows) {
    double x;
    double e;
    printf("Recuerde haber definido la funcion previamente\n");
    printf("Ingrese el valor a interpolar\n");
    scanf("%lf", &x);

    double sum = 0;
    for (int i = 0; i < rows; i++) {
        double product = 1;
        for (int j = 0; j < rows; j++) {
            if (j != i) {
                product = product * ((x - m[i][0]) / (m[j][0] - m[i][0]));
            }
        }
        sum = sum + m[i][1] * product;
    }
    e = fabs(f(x) - sum);
    printf("El valor interpolado para %lf es: %E, con un error de %E", x, sum, e);
}

void polynomial(double m[MAXROW][MAXCOL], int rows) {
    double A[rows][rows];
    double B[rows];
    double x[rows];
    printf("Recuerde haber cambiado el valor de MAXCOL2, correspondiente al tamanio de la matriz de coeficientes\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            A[i][j] = pow(m[i][0], j);
        }
        B[i] = m[i][1];
    }
    printf("Matriz antes de triangular\n");
    printMatrix(A, B, rows, rows);
    printf("Matriz despues de triangular\n");
    triangulation(A, B, rows, rows);
    retrosustitucion(A, B, x, rows, rows);

}

void triangulation(double m[MAXROW][MAXCOL2], double b[MAXROW], int rows, int columns) {
    for (int i = 0; i < rows - 1; ++i) {
        pivot(m, b, rows, columns, i);
        for (int j = i + 1; j < rows; ++j) {
            double factor = -m[j][i] / m[i][i];
            for (int k = 0; k < columns; ++k) {
                m[j][k] = m[i][k] * factor + m[j][k];
            }
            b[j] = b[i] * factor + b[j];
        }
    }
    printMatrix(m, b, rows, columns);
}

void pivot(double m[MAXROW][MAXCOL2], double b[MAXROW], int rows, int columns, int i) {
    double tolerance = pow(10, -3);
    if (fabs(m[i][i]) < tolerance) {
        for (int j = i + 1; j < rows; ++j) {
            if (fabs(m[j][i]) > fabs(m[i][i])) {
                for (int k = i; k < columns; ++k) {
                    double swap = m[i][k];
                    m[i][k] = m[j][k];
                    m[j][k] = swap;
                }
                double swap = b[i];
                b[i] = b[j];
                b[j] = swap;
            }
        }
    }
}

void retrosustitucion(double m[MAXROW][MAXCOL2], double b[MAXROW], double x[MAXROW], int rows, int columns) {
    double value;
    value = b[rows - 1] / m[rows - 1][columns - 1];
    x[rows - 1] = value;
    for (int i = rows - 2; i >= 0; --i) {
        double sum = 0;
        for (int j = i + 1; j < columns; ++j) {
            sum = sum + m[i][j] * x[j];
        }
        value = (b[i] - sum) / m[i][i];
        x[i] = value;
    }
    printf("Polinomio Interpolador: \n");
    int pow = 0;
    for (int i = 0; i <= rows - 1; i++) {
        if (pow == 0)
            printf("%lf", x[i]);
        else {
            if (x[i] >= 0)
                printf(" + %E.x^%d ", x[i], pow);
            else
                printf(" %E.x^%d", x[i], pow);
        }
        pow++;
    }
}
