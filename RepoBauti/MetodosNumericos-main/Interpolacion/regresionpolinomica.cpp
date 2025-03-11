#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

// Función para resolver un sistema de ecuaciones lineales utilizando el método de Gauss con pivoteo
void gaussPivot(vector<vector<double>>& A, vector<double>& B, vector<double>& x, int n) {
	for (int i = 0; i < n; i++) {
		int maxRow = i;
		for (int k = i + 1; k < n; k++) {
			if (abs(A[k][i]) > abs(A[maxRow][i])) {
				maxRow = k;
			}
		}
		
		// Intercambiar filas
		swap(A[i], A[maxRow]);
		swap(B[i], B[maxRow]);
		
		for (int k = i + 1; k < n; k++) {
			double factor = A[k][i] / A[i][i];
			for (int j = i; j < n; j++) {
				A[k][j] -= factor * A[i][j];
			}
			B[k] -= factor * B[i];
		}
	}
	
	// Sustitución hacia atrás
	x.resize(n);
	for (int i = n - 1; i >= 0; i--) {
		x[i] = B[i];
		for (int j = i + 1; j < n; j++) {
			x[i] -= A[i][j] * x[j];
		}
		x[i] /= A[i][i];
	}
}

// Función para calcular el coeficiente de correlación (R-squared)
double calculateRSquared(const vector<double>& y, const vector<double>& predictedY) {
	double yMean = 0.0;
	for (double value : y) {
		yMean += value;
	}
	yMean /= y.size();
	
	double ssTotal = 0.0;
	double ssResidual = 0.0;
	
	for (int i = 0; i < y.size(); i++) {
		ssTotal += pow(y[i] - yMean, 2);
		ssResidual += pow(y[i] - predictedY[i], 2);
	}
	
	double rSquared = 1.0 - (ssResidual / ssTotal);
	return rSquared;
}

int main() {
	int n;
	cout << "Ingrese el número de puntos (n+1): ";
	cin >> n;
	int p;
	cout << "Ingrese el grado del polinomio (p): ";
	cin >> p;
	
	/*vector<double> x(n + 1);
	vector<double> y(n + 1);
	
	for (int i = 0; i <= n; i++) {
		cout << "Ingrese x[" << i << "]: ";
		cin >> x[i];
		cout << "Ingrese y[" << i << "]: ";
		cin >> y[i];
	}*/
	
	vector<double> x = {0.2, 0.6, 1.3, 1.4,1.8,2.0};
	vector<double> y = {-0.94,-0.26,2.35,2.94,5.45,7.20};
	
	vector<vector<double>> A(p + 1, vector<double>(p + 1));
	vector<double> B(p + 1);
	
	for (int l = 0; l <= p; l++) {
		for (int m = 0; m <= p; m++) {
			A[l][m] = 0.0;
			for (int i = 0; i <= n; i++) {
				A[l][m] += pow(x[i], l + m);
			}
		}
		
		B[l] = 0.0;
		for (int i = 0; i <= n; i++) {
			B[l] += y[i] * pow(x[i], l);
		}
	}
	
	vector<double> coefficients;
	gaussPivot(A, B, coefficients, p + 1);
	
	cout << "Los coeficientes del polinomio son:\n";
	for (int i = 0; i <= p; i++) {
		cout << "a" << i << " = " << coefficients[i] << endl;
	}
	
	// Calcular el valor predicho
	vector<double> predictedY(n + 1, 0.0);
	for (int i = 0; i <= n; i++) {
		for (int j = 0; j <= p; j++) {
			predictedY[i] += coefficients[j] * pow(x[i], j);
		}
	}
	
	// Calcular el coeficiente de correlación
	double rSquared = calculateRSquared(y, predictedY);
	cout << "El coeficiente de correlación (R-squared) es: " << rSquared << endl;
	
	return 0;
}
