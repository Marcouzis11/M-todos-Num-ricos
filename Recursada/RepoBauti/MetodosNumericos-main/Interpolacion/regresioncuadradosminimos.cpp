#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Función para realizar la regresión por cuadrados mínimos
void regresionCuadradosMinimos(const vector<double>& x, const vector<double>& y, double& a, double& b) {
	int n = x.size();
	double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
	
	for (int i = 0; i < n; i++) {
		sumX += x[i];
		sumY += y[i];
		sumXY += x[i] * y[i];
		sumX2 += x[i] * x[i];
	}
	
	a = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
	b = (sumY - a * sumX) / n;
}

double coeficienteCorrelacion(const vector<double>& x, const vector<double>& y, double a, double b) {
	int n = x.size();
	double yPromedio = 0;
	for (int i = 0; i < n; i++) {
		yPromedio += y[i];
	}
	yPromedio /= n;
	
	double e = 0, st = 0;
	for (int i = 0; i < n; i++) {
		double yPredicho = a * x[i] + b;
		e += pow(y[i] - yPredicho, 2);
		st += pow(y[i] - yPromedio, 2);
	}
	
	return 1 - e / st;
}


int main() {
	std::vector<double> x = {0.5, 0.8, 1.3, 2.0};
	std::vector<double> y = {-0.716, -0.103, 3.419, 52.598};
	double a, b;
	
	regresionCuadradosMinimos(x, y, a, b);
	
	cout << "Resultados de la regresión por cuadrados mínimos:" << endl;
	cout << "a = " << a << endl;
	cout << "b = " << b << endl;
	
	double r = coeficienteCorrelacion(x, y, a, b);
	
	cout << "Coeficiente de correlación (r) = " << r << endl;
	return 0;
}
