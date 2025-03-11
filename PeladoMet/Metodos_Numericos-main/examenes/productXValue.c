#include <stdio.h>
#include <math.h>

#define MAXROW 10
#define MAXCOL 2

void readTxt(double[MAXROW][MAXCOL], int *);

int main(){

    double nodes[MAXROW][MAXCOL];
    int rows;
    readTxt(nodes, &rows);

    FILE *file;

    file = fopen("tabla.txt", "w");

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return 1;
    }

    for (int i = 0; i < rows; ++i) {

        fprintf(file, "%lf\t%lf\n", nodes[i][0], nodes[i][0] * nodes[i][1]);
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
