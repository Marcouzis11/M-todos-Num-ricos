#include <iostream>
#include <math.h>

using namespace std;

#define MAXCOLUMNS 2
#define MAXROWS 16
#define ERROR pow(10, -3)

// Declaración de funciones
void readFile(double array[MAXROWS][MAXCOLUMNS], int *rows);
void printNodes(double matrix[MAXROWS][MAXCOLUMNS], int rows);
void assembleMatrix(double nodes[MAXROWS][MAXCOLUMNS], double matrix[MAXROWS][MAXROWS], double b[MAXROWS], int intervals);
void printMatrix(double matrix[MAXROWS][MAXROWS], double b[MAXROWS], int rows, int columns);
int gaussianElimination(double matrix[MAXROWS][MAXROWS], double b[MAXROWS], double x[MAXROWS], int rows);
void interpolation(double nodes[MAXROWS][MAXCOLUMNS], double z[MAXROWS], int rows);

int main() {
	double nodes[MAXROWS][MAXCOLUMNS] = {0};
	int rows = 0;
	double matrix[MAXROWS][MAXROWS] = {0};
	double b[MAXROWS] = {0};
	double z[MAXROWS] = {0};
	
	readFile(nodes, &rows);
	
	int intervalos = rows - 1;
	
	printNodes(nodes, rows);
	assembleMatrix(nodes, matrix, b, intervalos);
	printMatrix(matrix, b, 4 * intervalos, 4 * intervalos);
	gaussianElimination(matrix, b, z, 4 * intervalos);
	interpolation(nodes, z, rows);
	
	return 0;
}

void readFile(double array[MAXROWS][MAXCOLUMNS], int *rows) {
	FILE *file;
	char c;
	
	file = fopen("nodes.txt", "r");
	if (file == NULL) {
		cerr << "Error: No se puede abrir el archivo" << endl;
		exit(1); // Salir del programa con código de error
	}
	
	while ((c = fgetc(file)) != EOF) {
		if (c == '\n') {
			*rows += 1;
		}
	}
	cout << "Filas:" << *rows << endl;
	
	// Reseteamos el puntero del archivo
	rewind(file);
	
	// Cargamos los datos leídos en el array
	int indexB;
	for (int indexA = 0; indexA < *rows; indexA++) {
		indexB = 0;
		do {
			fscanf(file, "%lf", &(array[indexA][indexB]));
			indexB++;
		} while ((c = fgetc(file)) != '\n');
	}
	
	fclose(file); // Cerrar el archivo después de usarlo
}

void printNodes(double matrix[MAXROWS][MAXCOLUMNS], int rows) {
	cout << "LOS NODOS SON:" << endl;
	for (int indexA = 0; indexA < rows; indexA++) {
		for (int indexB = 0; indexB < MAXCOLUMNS; indexB++) {
			cout << matrix[indexA][indexB] << "\t";
		}
		printf("\n");
	}
}

void printMatrix(double matrix[MAXROWS][MAXROWS], double b[MAXROWS], int rows, int columns) {
	for (int indexA = 0; indexA < rows; indexA++) {
		for (int indexB = 0; indexB < columns; indexB++) {
			cout << matrix[indexA][indexB] << "\t";
		}
		cout << b[indexA] << endl;
	}
}

void assembleMatrix(double nodes[MAXROWS][MAXCOLUMNS], double matrix[MAXROWS][MAXROWS], double b[MAXROWS], int intervals) {
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

int gaussianElimination(double matrix[MAXROWS][MAXROWS], double b[MAXROWS], double x[MAXROWS], int rows) {
	double aux;
	double factor;
	float e = ERROR;
	
	//triangulacion superior
	for (int indexA = 0; indexA < rows - 1; indexA++) {
		//pivoteo
		int swap = 0;
		if (fabs(matrix[indexA][indexA]) < e) {
			for (int indexB = indexA + 1; indexB <= rows; indexB++) {
				if (fabs(matrix[indexB][indexA]) > fabs(matrix[indexA][indexA])) {
					for (int indexC = indexA; indexC < rows; indexC++) {
						aux = matrix[indexA][indexC];
						matrix[indexA][indexC] = matrix[indexB][indexC];
						matrix[indexB][indexC] = aux;
					}
					aux = b[indexA];
					b[indexA] = b[indexB];
					b[indexB] = aux;
					swap = 1;
					break;
				}
			}
			if (swap == 0) {
				cout << "Warning: sistema singular, no se puede resolver" << endl;
				return 0;
			} else
				cout << "Pivoteo concretado " << endl;
		}
		
		for (int indexB = indexA + 1; indexB <= rows - 1; indexB++) {
			factor = (-matrix[indexB][indexA]) / (matrix[indexA][indexA]);
			
			for (int indexC = indexA; indexC <= rows - 1; indexC++)
				matrix[indexB][indexC] = matrix[indexB][indexC] + factor * matrix[indexA][indexC];
				b[indexB] = b[indexB] + factor * b[indexA];
		}
	}
	
	//imprime matrix triangular
	cout << endl << "La Matriz triangular superior quedo: " << endl;
	for (int indexA = 0; indexA < rows; indexA++) {
		
		for (int indexB = 0; indexB < rows - 1; indexB++) {
			cout << matrix[indexA][indexB] << " ";
		}
		cout << b[indexA] << endl;
	}
	
	//sustitucion regresiva
	double suma;
	x[rows - 1] = b[rows - 1] / matrix[rows - 1][rows - 1];
	
	for (int indexA = rows - 2; indexA >= 0; indexA--) {
		suma = b[indexA];
		
		for (int indexB = indexA + 1; indexB <= rows - 1; indexB++) {
			suma -= matrix[indexA][indexB] * x[indexB];
		}
		x[indexA] = (suma) / matrix[indexA][indexA];
	}
	
	cout << endl << "----------" << endl;
	cout << "SOLUCIONES" << endl;
	cout << "----------";
	for (int index = 0; index <= rows - 1; index++) {
		cout << endl << "x" << index + 1 << "=" << x[index];
	}
	cout << endl;

}

void interpolation(double nodes[MAXROWS][MAXCOLUMNS], double z[MAXROWS], int rows) {
	double value;
	double result = 0;
	cout << "\nIngrese el valor a interpolar: ";
	cin >> value;
	
	if (value >= nodes[0][0] && value <= nodes[rows - 1][0]) {
		for (int i = 0; i < rows; ++i) {
			if (value <= nodes[i + 1][0]) {
				result = z[4 * i] * pow(value, 3) + z[4 * i + 1] * pow(value, 2) + z[4 * i + 2] * value + z[4 * i + 3];
				break;
			}
		}
		cout << "El valor interpolado para " << value << " es: " << result << endl;
	} else {
		cout << "\nEl valor a interpolar no se encuentra en el rango de datos" << endl;
	}
}
