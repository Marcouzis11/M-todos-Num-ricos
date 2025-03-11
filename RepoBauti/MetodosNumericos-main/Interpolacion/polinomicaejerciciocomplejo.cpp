#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#define FILAS 20
#define COLUMNAS 20


void buildMatrix(double m[7][2], double a[FILAS][FILAS], double b[FILAS], int filas);

// Función para triangular matriz
void triangulacion(double a[FILAS][FILAS], double b[FILAS], double x[FILAS], int filas);

//Funcion para retrostutituir matriz
void retrostutitucion(double a[FILAS][FILAS], double b[FILAS], double x[FILAS], int filas);

//
void pivot(double a[FILAS][FILAS], double b[FILAS], int filas, int i);

//verifica que en la diagonal no hayan ceros.
double determinante(double a[FILAS][FILAS], double b[FILAS], double x[FILAS], int filas);

using namespace std;

int main(int argc, char *argv[]) {
	double m[10][2] = {{1.0,-0.236},{1.2,0.209},{1.5,0.853},{2.0,1.746},{2.6,2.231},{2.9,2.163},{3.0,2.110} };
	int filas = 7;
	printf("FILAS: %d \nPUNTOS (x,y): \n" , filas);
	for(int i = 0 ; i < filas ; i++){
		printf("\t %lf \t %lf", m[i][0] , m[i][1]);
		printf("\n");
	}
	
	double a[FILAS][FILAS] = {{0}};
	double b[FILAS] = {0};
	buildMatrix(m,a,b,filas);
	
	return 0;
}



	
void buildMatrix(double m[7][2], double a[FILAS][FILAS], double b[FILAS], int filas){
	//m[i][0] es xi
	//m[i][1] es yi
	for(int i = 0 ; i < filas ; i++){
		
		a[0][0] += sin(m[i][0])*sin(m[i][0]);
		a[0][1] += sin(m[i][0])*cos(m[i][0]);
		b[0] += m[i][1] * sin(m[i][0]);
		a[1][0] += sin(m[i][0])*cos(m[i][0]);
		a[1][1] += cos(m[i][0])*cos(m[i][0]);
		b[1] += cos(m[i][0]) * m[i][1];
	}
	
	printf("\n\n\nMATRIZ;\n");
	for(int i = 0 ; i < 2 ; i++){
		printf("\t %lf \t %lf \t %lf \t %lf", a[i][0] , a[i][1], b[i]);
		printf("\n");
	}
	
	
	double* x = (double*)malloc(filas * sizeof(double));
	triangulacion(a, b, x, 2);
	
}
void triangulacion(double a[FILAS][FILAS], double b[FILAS], double x[FILAS], int filas){
	for (int i = 0 ; i < (filas - 1) ; i++){
		pivot(a, b, filas , i);
		for (int j = i + 1; j < filas; j++) {
			double factor = -a[j][i] / a[i][i];
			for (int k = 0; k < filas; ++k) {
				a[j][k] = a[i][k] * factor + a[j][k];
			}
			b[j] = b[i] * factor + b[j];
		}
	}
	
	double norma = determinante(a,b,x,filas);
	if(norma == 0.0){
		printf("\n\nmatriz singular");
	}else{
		retrostutitucion(a, b, x, filas);
	}
}
void retrostutitucion(double a[FILAS][FILAS], double b[FILAS], double x[FILAS], int filas){
	double value = 0;
	value = b[filas - 1] / a[filas - 1][filas - 1];
	x[filas - 1] = value;
	for (int i = filas - 2; i >= 0; --i) {
		double sum = 0;
		for (int j = i + 1; j < filas; ++j) {
			sum = sum + a[i][j] * x[j];
		}
		value = (b[i] - sum) / a[i][i];
		x[i] = value;
	}
	printf("\n\nConjunto solucion: \n");
	for (int i = 0; i < filas; ++i) {
		printf("x%d = %lf\n", i + 1, x[i]);
	}
}
void pivot(double a[FILAS][FILAS], double b[FILAS], int filas, int i){
	if (fabs(a[i][i]) < 0.0001) {
		for (int j = i + 1; j < filas; j++) {
			if (fabs(a[j][i]) > fabs(a[i][i])) {
				for (int k = i; k < filas; ++k) {
					printf("Se realizo pivoteo\n");
					double swap = a[i][k];
					a[i][k] = a[j][k];
					a[j][k] = swap;
				}
				double swap = b[i];
				b[i] = b[j];
				b[j] = swap;
			}
		}
	}
}
double determinante(double a[FILAS][FILAS], double b[FILAS], double x[FILAS], int filas){
	double norma = 1;
	for(int i = 0; i < filas ; i++){
		norma = norma * a[i][i];
	}
	return norma;
}
