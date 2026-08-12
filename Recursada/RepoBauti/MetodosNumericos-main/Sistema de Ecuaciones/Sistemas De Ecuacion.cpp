#include <iostream>
#include <fstream>
#include <cmath>

#define MAXCOLUMNS 20
#define MAXROWS 20
#define ERROR pow(10, -3)
#define MAXITERATIONS pow(10, 5)

using namespace std;

void readFile(double matrix[MAXROWS][MAXCOLUMNS], double b[MAXROWS], int* rows, int* columns) {
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


void printMatrix(double matrix[MAXROWS][MAXCOLUMNS], int rows, int columns) {
	for (int indexA = 0; indexA < rows; indexA++) {
		for (int indexB = 0; indexB < columns; indexB++) {
			cout << matrix[indexA][indexB] << "\t";
		}
		cout << endl;
	}
}

bool isDiagonallyDominant(double matrix[MAXROWS][MAXCOLUMNS], int rows, int columns) {
	for (int indexA = 0; indexA < rows; indexA++) {
		double diagonalValue = fabs(matrix[indexA][indexA]);
		double sum = 0;
		
		for (int indexB = 0; indexB < rows; indexB++) {
			if (indexA != indexB) {
				sum += fabs(matrix[indexA][indexB]);
			}
		}
		
		if (diagonalValue <= sum) {
			return false; // La matriz no es diagonalmente dominante
		}
	}
	
	return true; // La matriz es diagonalmente dominante
}

int countNonZeroElements(double matrix[MAXROWS][MAXCOLUMNS], int rows, int columns) {
	// Implementa el conteo de elementos no nulos en la matriz
	int count = 0;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (matrix[i][j] != 0.0) {
				count++;
			}
		}
	}
	return count;
}

void gaussianElimination(double matrix[MAXROWS][MAXROWS], double b[MAXROWS], double x[MAXROWS], int rows, int columns) {
	double aux; //Intercambio temporal de valores para el pivoteo
	double factor; //Almacenar factores utilizados en la funcion
	float e = ERROR;
	
	//triangulacion superior
	for (int indexA = 0; indexA < rows - 1; indexA++) { //Recorre las filas
		//pivoteo
		int swap = 0; //Indica si se realizo un cambio de filas
		if (fabs(matrix[indexA][indexA]) < e) {
			for (int indexB = indexA + 1; indexB <= rows; indexB++) {
				if (fabs(matrix[indexB][indexA]) > fabs(matrix[indexA][indexA])) { //Verifica si el elemento pivote de la fila es menor q la tolerancia
					for (int indexC = indexA; indexC < columns; indexC++) { //busca el valor absoluto mas grande
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
				return;
			} else
				cout << "Pivoteo concretado " << endl;
		}
		///Se transforma la matriz a una triangular superior
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
		
		for (int indexB = 0; indexB < columns - 1; indexB++) {
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

void gaussSeidel(double matrix[MAXROWS][MAXCOLUMNS], double b[MAXROWS], int rows, int columns) {
	// Declaración de vectores para las soluciones actuales (newX) y anteriores (oldX)
	double newX[MAXCOLUMNS] = {0};
	double oldX[MAXCOLUMNS] = {0};
	double e = 0; // Variable para el error
	int iterations = 0; // Contador de iteraciones
	double sum = 0;
	
	double tolerance = ERROR; // Tolerancia para la convergencia
	
	// Ciclo principal del método Gauss-Seidel
	do {
		e = 0; // Inicializar el error en cada iteración
		iterations++; // Incrementar el contador de iteraciones
		for (int indexA = 0; indexA < rows; indexA++) {
			
			if (indexA == 0) {
				sum = 0;
				// Este bucle for calcula la suma de los productos de los elementos de la matriz
				// en la fila indexA (excepto el elemento de la diagonal) por los valores de newX
				// que ya se han calculado en la iteración actual.
				for (int indexB = 1; indexB < rows; ++indexB) {
					sum = sum + matrix[indexA][indexB] * oldX[indexB];
				}
				// Calcular el nuevo valor de la variable x[indexA] utilizando la fórmula de Gauss-Seidel.
				newX[indexA] = (b[indexA] - sum) / matrix[indexA][indexA];
				
			} else {
				sum = 0;
				// En este bucle for, se calcula la suma de los productos de los elementos de la matriz
				// en la fila indexA que están a la izquierda de la diagonal (elementos con índices menores)
				// por los valores actualizados de newX correspondientes.
				for (int indexB = 0; indexB < indexA; ++indexB) {
					sum = sum + matrix[indexA][indexB] * newX[indexB];
				}
				
				// En este bucle for, se calcula la suma de los productos de los elementos de la matriz
				// en la fila indexA que están a la derecha de la diagonal (elementos con índices mayores)
				// por los valores antiguos de oldX correspondientes.
				for (int indexB = indexA + 1; indexB < rows; ++indexB) {
					sum = sum + matrix[indexA][indexB] * oldX[indexB];
				}
				
				// Calcular el nuevo valor de la variable x[indexA] utilizando la fórmula de Gauss-Seidel.
				newX[indexA] = (b[indexA] - sum) / matrix[indexA][indexA];
			}
		}
		
		// Calcular el error en esta iteración
		for (int index = 0; index < rows; ++index) {
			e = e + pow(newX[index] - oldX[index], 2);
		}
		e = sqrt(e);
		
		// Actualizar el vector de soluciones anteriores (oldX) con los nuevos valores (newX).
		for (int index = 0; index < rows; ++index) {
			oldX[index] = newX[index];
		}
		
	} while (tolerance < e && iterations <  MAXITERATIONS);
	
	// Comprobar si se alcanzó el número máximo de iteraciones
	if (iterations ==  MAXITERATIONS)
		cout << "Numero de iteraciones maximas alcanzadas" << endl;
	
	// Imprimir la solución encontrada y el error
	cout << "Conjunto solucion:" << endl;
	for (int indexA = 0; indexA < rows; ++indexA) {
		cout << "x" << indexA << "=" << newX[indexA] << endl;
	}
	cout << "Error:" << e << "\n" << "Iteraciones:" << iterations << endl;
}

void jacobi(double matrix[MAXROWS][MAXCOLUMNS], double b[MAXROWS], int rows, int columns) {
	double oldX[MAXROWS] = {0}; //Aproximacion anigua
	double newX[MAXROWS] = {0}; //Aproximacion nueva
	double tolerance = ERROR;
	double e;
	int iterations = 0;
	
	do {
		e = 0;
		iterations++;
		for (int indexA = 0; indexA < rows; ++indexA) { //Recorre todas las filas
			
			double sum = 0;
			for (int indexB = 0; indexB < columns; ++indexB) { //Recorre las columnas
				if (indexA != indexB) {
					sum = sum + matrix[indexA][indexB] * oldX[indexB];
				}
			}
			newX[indexA] = (b[indexA] - sum) / matrix[indexA][indexA]; //Nuva Aproximacion
			e = e + pow((newX[indexA] - oldX[indexA]), 2);
			oldX[indexA] = newX[indexA]; //Se guarda la nueva aproximacion para la siguiente iteracion
		}
		
		e = sqrt(e);
		
		
	} while (tolerance < e && iterations < MAXITERATIONS);
	
	if (iterations == MAXITERATIONS) {
		cout << "FINALIZED: maxima iteraciones" << endl;
	}
	
	cout << "Conjunto solucion:" << endl;
	for (int indexA = 0; indexA < rows; ++indexA) {
		cout << "x" << indexA << "=" << newX[indexA] << endl;
	}
	cout << "Error:" << e << "\n" << "Iteraciones:" << iterations << endl;
	
}

int main() {
	double matrix[MAXROWS][MAXCOLUMNS];
	double b[MAXROWS];
	int columns = 0;
	int rows = 0;
	
	// Leer la matriz y los datos de entrada
	readFile(matrix, b, &rows, &columns);
	
	// Verificar la diagonal dominante y la cantidad de elementos no nulos
	bool isDiagonalDominant = isDiagonallyDominant(matrix, rows, columns);
	int nonZeroCount = countNonZeroElements(matrix, rows, columns);
	
	if (isDiagonalDominant) {
		cout << "Usando el método de Jacobi" << endl;
		jacobi(matrix, b, rows, columns);
	} else if (nonZeroCount < (rows * columns) / 2) {
		cout << "Usando el método de Gauss-Seidel" << endl;
		gaussSeidel(matrix, b, rows, columns);
	} else {
		cout << "Usando el método de Eliminación Gaussiana" << endl;
		double x[MAXROWS];
		gaussianElimination(matrix, b, x, rows, columns);
		
	}
	
	return 0;
}
