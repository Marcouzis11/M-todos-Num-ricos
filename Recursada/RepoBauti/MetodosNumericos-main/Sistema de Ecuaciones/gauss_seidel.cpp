#include <iostream>
#include <math.h>

/////////////////////
///DEFINIR TOLERANCIA
/////////////////////
#define ERROR pow(10, -6)

#define MAXCOLUMS 100
#define MAXROWS 100
#define MAXITERATIONS pow(10,5)

using namespace std;

void readFile(double[MAXROWS][MAXCOLUMS], double[MAXROWS], int *, int *);

void printMatrix(double[MAXROWS][MAXCOLUMS], int, int);

void diagonallyDominant(double[MAXROWS][MAXCOLUMS], int, int);

void gaussSeidel(double [MAXROWS][MAXCOLUMS], double [MAXROWS], int, int);

int main() {
	
	double matrix[MAXROWS][MAXCOLUMS];
	double b[MAXROWS];
	int rows = 0;
	int columns = 0;
	
	cout << "-------------------" << endl;
	cout << "METODO GAUSS-SEIDEL" << endl;
	cout << "-------------------" << endl;
	
	readFile(matrix, b, &rows, &columns);
	printMatrix(matrix, rows, columns);
	gaussSeidel(matrix, b, rows, columns);
	
	return 0;
}

void readFile(double matrix[MAXROWS][MAXCOLUMS], double b[MAXROWS], int *rows, int *columns) {
	FILE *file;
	char c;
	
	file = fopen("data.txt", "r");
	if (file == NULL) {
		puts("No se puede abrir el archivo");
	}
	
	while ((c = fgetc(file)) != EOF) {
		if (c == '\n') {
			*rows += 1;
		}
	}
	
	cout << "Filas:" << *rows << endl;
	
	//reseteamos el puntero del archivo
	rewind(file);
	
	//Cargo los datos leidos en el array
	int indexB;
	for (int indexA = 0; indexA < *rows; indexA++) {
		indexB = 0;
		do {
			fscanf(file, "%lf", &(matrix[indexA][indexB]));
			indexB++;
		} while ((c = fgetc(file)) != '\n');
	}
	
	*columns = indexB;
	cout << "Columnas: " << *columns << endl;
	
	//guardo los terminos independientes
	for (int indexA = 0; indexA < *rows; ++indexA) {
		b[indexA] = matrix[indexA][*columns - 1];
	}
	
	//es para chequear el vector de terminos independientes
	/*cout<<endl<<"---------------------------------------------"<<endl;
	cout << "LOS TERMINOS INDEPENDIENTES DE LA MATRIZ SON:" << endl;
	for (int index = 0; index < *rows; index++) {
	cout<<b[index]<<"\t";
	}
	cout<<endl<<"---------------------------------------------"<<endl;
	*/
}

void printMatrix(double matrix[MAXROWS][MAXCOLUMS], int rows, int columns) {
	
	for (int indexA = 0; indexA < rows; indexA++) {
		
		for (int indexB = 0; indexB < columns; indexB++) {
			cout << matrix[indexA][indexB] << "\t";
		}
		cout << endl;
	}
}

void diagonallyDominant(double matrix[MAXROWS][MAXCOLUMS], int rows, int columns) {
	
	for (int indexA = 0; indexA < rows; indexA++) {
		double sum = 0;
		for (int indexB = 0; indexB < rows; indexB++) {
			if (indexA != indexB) {
				sum = sum + fabs(matrix[indexA][indexB]);
			}
		}
		if (fabs(matrix[indexA][indexA]) <= sum) {
			printf("Warning: la matriz no es diagonalmente dominante\n");
			break;
		}
	}
}

void gaussSeidel(double matrix[MAXROWS][MAXCOLUMS], double b[MAXROWS], int rows, int columns) {
	//gaussSeidel
	double newX[MAXCOLUMS] = {0};
	double oldX[MAXCOLUMS] = {0};
	double e = 0;
	int iterations = 0;
	double sum = 0;
	
	
	double tolerance = ERROR;
	
	diagonallyDominant(matrix, rows, columns);
	
	do {
		e = 0;
		iterations++;
		for (int indexA = 0; indexA < rows; indexA++) {
			
			if (indexA == 0) {
				sum = 0;
				for (int indexB = 1; indexB < rows; ++indexB) {
					sum = sum + matrix[indexA][indexB] * oldX[indexB];
				}
				newX[indexA] = (b[indexA] - sum) / matrix[indexA][indexA];
				
			} else {
				
				sum = 0;
				for (int indexB = 0; indexB < indexA; ++indexB) {
					sum = sum + matrix[indexA][indexB] * newX[indexB];
				}
				
				for (int indexB = indexA + 1; indexB < rows; ++indexB) {
					sum = sum + matrix[indexA][indexB] * oldX[indexB];
				}
				
				newX[indexA] = (b[indexA] - sum) / matrix[indexA][indexA];
				
			}
			
		}
		for (int index = 0; index < rows; ++index) {
			e = e + pow(newX[index] - oldX[index], 2);
		}
		e = sqrt(e);
		
		for (int index = 0; index < rows; ++index) {
			oldX[index] = newX[index];
		}
		
	} while (tolerance < e && iterations <  7);
	
	
	if (iterations ==  MAXITERATIONS)
		cout << "Numero de iteraciones maximas alcanzadas" << endl;
	
	cout << "Conjunto solucion:" << endl;
	for (int indexA = 0; indexA < rows; ++indexA) {
		cout << "x" << indexA << "=" << newX[indexA] << endl;
	}
	cout << "Error:" << e << "\n" << "Iteraciones:" << iterations << endl;
	
}
