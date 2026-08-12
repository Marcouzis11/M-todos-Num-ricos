#include <stdio.h>
#include <math.h>

#define MAXROW 10
#define MAXCOL 2

void readTxt(double [MAXROW][MAXCOL], int *);


int main(int argc, char *argv[]) {
    double nodes[MAXROW][MAXCOL];
    int rows;
    readTxt(nodes, &rows);
    double a, b, x;
    int n = rows - 1;
    double derivative = 0;

    FILE *file;

    file = fopen("derivadas.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return -1;
    }

    double h = (double) (nodes[rows - 1][0] - nodes[0][0]) / (rows - 1);

    for (int i = 0; i < 2; ++i) {
        //x = a + i * h;
        //derivative = (-f(x + 2 * h) + 4 * f(x + h) - 3 * f(x)) / (2 * h);
        derivative = (-nodes[i + 2][1] + 4 * nodes[i + 1][1] - 3 * nodes[i][1]) / (2 * h);
        fprintf(file, "%lf\t%lf\n", nodes[i][0], derivative);
    }

    for (int i = 2; i < n - 1; ++i) {
        //x = a + i * h;
        //derivative = (-f(x + 2 * h) + 8 * f(x + h) - 8 * f(x - h) + f(x - 2 * h)) / (12 * h);
        derivative = (-nodes[i + 2][1] + 8 * nodes[i + 1][1] - 8 * nodes[i - 1][1] + nodes[i - 2][1]) / (12 * h);
        fprintf(file, "%lf\t%lf\n", nodes[i][0], derivative);
    }

    for (int i = n - 1; i < n + 1; ++i) {
        //x = a + i * h;
        //derivative = (3 * f(x) - 4 * f(x - h) + f(x - 2 * h)) / (2 * h);
        derivative = (3 * nodes[i][1] - 4 * nodes[i - 1][1] + nodes[i - 2][1]) / (2 * h);
        fprintf(file, "%lf\t%lf\n", nodes[i][0], derivative);
    }
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