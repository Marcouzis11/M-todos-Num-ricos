#include <stdio.h>
#include <math.h>

#define MAXROW 12
#define MAXCOL 2
#define MAXCOL2 12


void readTxtI(double[MAXROW][MAXCOL], int *);

void printMatrixI(double[MAXROW][MAXCOL], int);

void buildMatrix(double [MAXROW][MAXCOL], double [MAXROW][MAXCOL2], double [MAXROW], int);

void interpolation(double [MAXROW][MAXCOL], double [MAXROW], int);


void printMatrix(double [MAXROW][MAXCOL2], double [MAXROW], int, int);

void triangulation(double [MAXROW][MAXCOL2], double [MAXROW], int, int);

void retrosustitucion(double [MAXROW][MAXCOL2], double [MAXROW], double [MAXROW], int, int);

void pivot(double [MAXROW][MAXCOL2], double [MAXROW], int, int, int);

int main(int argc, char *argv[]) {

    double nodes[MAXROW][MAXCOL];
    double matrix[MAXROW][MAXCOL2];
    double b[MAXROW];
    double z[MAXROW];
    int rows;
    readTxtI(nodes, &rows);
    printMatrixI(nodes, rows);
    // intervals = nodes - 1
    buildMatrix(nodes, matrix, b, rows - 1);
    printf("\nMatriz antes de reducir\n");
    printMatrix(matrix, b, 4 * (rows - 1), 4 * (rows - 1));
    printf("\nMatriz despues de reducir\n");
    triangulation(matrix, b, 4 * (rows - 1), 4 * (rows - 1));
    retrosustitucion(matrix, b, z, 4 * (rows - 1), 4 * (rows - 1));
    interpolation(nodes, z, rows);

    return 0;
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
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%.2lf\t", m[i][j]);
        }
        printf("%.2lf", b[i]);
        printf("\n");
    }
}

void buildMatrix(double nodes[MAXROW][MAXCOL], double matrix[MAXROW][MAXCOL2], double b[MAXROW], int intervals) {

    ///construccion de las primeras 2n ecuaciones(imagen de la funcion)
    for (int indexA = 0; indexA < intervals; ++indexA) {
        for (int indexB = 0; indexB <= 3; ++indexB) {
            matrix[2 * indexA][4 * indexA + indexB] = pow(nodes[indexA][0], 3 - indexB);
            matrix[2 * indexA + 1][4 * indexA + indexB] = pow(nodes[indexA + 1][0], 3 - indexB);
        }
        b[2 * indexA] = nodes[indexA][1];
        b[2 * indexA + 1] = nodes[indexA + 1][1];
    }

    ///construccion de las primeras n-1 ecuaciones(derivadas primeras)
    for (int indexA = 2 * intervals; indexA <= (3 * intervals - 2); ++indexA) {
        for (int indexB = 0; indexB <= intervals - 2; ++indexB) {
            for (int indexC = 0; indexC <= 2; ++indexC) {
                matrix[indexA][4 * indexB + indexC] = (3 - indexC) * pow(nodes[indexB + 1][0], 2 - indexC);
                matrix[indexA][4 * (indexB + 1) + indexC] = -(3 - indexC) * pow(nodes[indexB + 1][0], 2 - indexC);
            }
        }
        b[indexA] = 0;
    }

    ///derivadas segundas
    for (int indexA = 3 * intervals - 1; indexA <= (4 * intervals - 3); ++indexA) {
        for (int indexB = 0; indexB <= intervals - 2; ++indexB) {
            matrix[indexA][4 * indexB] = 6 * nodes[indexB + 1][0];
            matrix[indexA][4 * indexB + 1] = 2;
            matrix[indexA][4 * indexB + 4] = (-6 * nodes[indexB + 1][0]);
            matrix[indexA][4 * indexB + 5] = (-2);
        }
        b[indexA] = 0;
    }

    matrix[4 * intervals - 2][0] = 6 * nodes[0][0];
    matrix[4 * intervals - 2][1] = 2;
    b[4 * intervals - 2] = 0;

    matrix[4 * intervals - 1][4 * intervals - 4] = 6 * nodes[intervals][0];
    matrix[4 * intervals - 1][4 * intervals - 3] = 2;
    b[4 * intervals - 1] = 0;

}

void interpolation (double nodes[MAXROW][MAXCOL], double z[MAXROW], int rows) {
    double value;
    double result = 0;
    printf("\nIngrese el valor a interpolar\n");
    scanf("%lf", &value);

    if (value >= nodes[0][0] && value <= nodes[rows - 1][0]) {
        for (int i = 0; i < rows; ++i) {
            if(value <= nodes[i+1][0]){
                result = z[4*i]*pow(value,3) + z[4*i+1]*pow(value,2) + z[4*i+2]*value + z[4*i+3];
                break;
            }
        }
        printf("El valor interpolado para %lf es: %lf\n", value, result);
    }
    else{
        printf("\nEl valor a interpolar no se encuentra en el rango de datos\n");
    }
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
        printf("x%d = %lf\n", i, x[i]);
    }
}
