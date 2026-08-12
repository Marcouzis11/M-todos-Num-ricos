#include <stdio.h>
#include <math.h>

#define MAXCOL 15
#define MAXROW 15

void readTxt(double [MAXROW][MAXCOL], double [MAXROW], int *, int *);

void printMatrix(double [MAXROW][MAXCOL], double [MAXROW], int, int);

void triangulation(double [MAXROW][MAXCOL], double [MAXROW], int, int);

void retrosustitucion(double [MAXROW][MAXCOL], double [MAXROW], double [MAXROW], int, int);

void pivot(double [MAXROW][MAXCOL], double [MAXROW], int, int, int);

double norma(double [MAXROW][MAXCOL], double [MAXROW], int, int);

void banda(double [MAXROW][MAXCOL], double [MAXROW], int *, int *);

void save(double [MAXROW][MAXCOL], double [MAXROW]);

int main(int argc, char *argv[]) {
    double matrix[MAXROW][MAXCOL];
    double b[MAXROW];
    double x[MAXROW];
    int rows = 0;
    int columns = 0;
    //Matrices en banda
    //banda(matrix, b, &rows, &columns);
    //save(matrix, b);
    readTxt(matrix, b, &rows, &columns);
    printMatrix(matrix, b, rows, columns);
    triangulation(matrix, b, rows, columns);
    retrosustitucion(matrix, b, x, rows, columns);

    return 0;
}

void readTxt(double m[MAXROW][MAXCOL], double b[MAXROW], int *rows, int *columns) {
    FILE *readPtr;
    int j, i, n;
    float k;
    readPtr = fopen("data.txt", "r");
    fscanf(readPtr, "%d", &n);
    *rows = n;
    *columns = n;
    for (i = 0; i < *rows; i++) {
        for (j = 0; j < *columns; j++) {
            fscanf(readPtr, "%f", &k);
            m[i][j] = k;
        }
        fscanf(readPtr, "%f", &k);
        b[i] = k;
    }
    fclose(readPtr);
}

void printMatrix(double m[MAXROW][MAXCOL], double b[MAXROW], int rows, int columns) {
    double diagonal = 1;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%.2lf ", m[i][j]);

        }
        printf("%.2lf", b[i]);
        diagonal = diagonal * m[i][i];
        printf("\n");
    }
    printf("Diagonal: %lf\n", diagonal);
    printf("Norma Euclidiana: %lf\n", norma(m, b, rows, columns));
}

void triangulation(double m[MAXROW][MAXCOL], double b[MAXROW], int rows, int columns) {
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

void pivot(double m[MAXROW][MAXCOL], double b[MAXROW], int rows, int columns, int i) {
    double tolerance = pow(10, -3);
    if (fabs(m[i][i]) < tolerance) {
        for (int j = i + 1; j < rows; ++j) {
            if (fabs(m[j][i]) > fabs(m[i][i])) {
                for (int k = i; k < columns; ++k) {
                    printf("Se realizo pivoteo\n");
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

void retrosustitucion(double m[MAXROW][MAXCOL], double b[MAXROW], double x[MAXROW], int rows, int columns) {
    double value = 0;
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
    printf("Conjunto solucion: \n");
    for (int i = 0; i < rows; ++i) {
        printf("x%d = %lf\n", i + 1, x[i]);
    }
}

double norma(double m[MAXROW][MAXCOL], double b[MAXROW], int rows, int columns) {
    double norma = 1;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            norma = norma + pow(m[i][j], 2);
        }
    }
    norma = sqrt(norma);
    return norma;
}

/*void banda(double m[MAXROW][MAXCOL], double b[MAXROW], int *rows, int *columns) {
    printf("Recuerde haber definido el tamanio maximo de la matriz en MAXCOL y MAXROW");
    *rows = MAXROW;
    *columns = MAXCOL;
    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            if (i == 0 && j == 0) {
                m[i][j] = 1;
            } else if (i == MAXROW-1 && j == MAXROW-1) {
                m[i][j] = 1;
            } else if (j == i - 1 && i != MAXROW -1) {
                m[i][j] = 1;
            } else if (j == i) {
                m[i][j] = -2;
            } else if (j == i + 1 && i != 0) {
                m[i][j] = 1;
            } else {
                m[i][j] = 0;
            }
            b[i] = 1;
        }
    }
}

void save(double m[MAXCOL][MAXROW], double b[MAXROW]) {
    FILE *readPtr;
    int j, i;
    readPtr = fopen("data.txt", "w");
    fprintf(readPtr, "%d\n", MAXROW);
    for (i = 0; i < MAXROW; i++) {
        for (j = 0; j < MAXCOL; j++) {
            fprintf(readPtr, "%.2lf\t", m[i][j]);
        }
        fprintf(readPtr,"%.2lf\n", b[i]);
    }
    fclose(readPtr);
}*/
