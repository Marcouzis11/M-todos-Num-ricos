#include <iostream>
#include <vector>
#include <cmath>

// Función para calcular la media de un vector de datos
double calcularMedia(const std::vector<double>& datos) {
	double suma = 0.0;
	for (const double& dato : datos) {
		suma += dato;
	}
	return suma / datos.size();
}

// Función para calcular la pendiente (a) y la intersección (b) de la regresión lineal
void regresionLineal(const std::vector<double>& x, const std::vector<double>& y, double& a, double& b) {
	int n = x.size();
	
	// Calcular las sumas necesarias para la fórmula
	double sumX = 0.0;
	double sumY = 0.0;
	double sumXY = 0.0;
	double sumX2 = 0.0;
	
	for (int i = 0; i < n; i++) {
		sumX += x[i];
		sumY += y[i];
		sumXY += x[i] * y[i];
		sumX2 += x[i] * x[i];
	}
	
	// Calcular la pendiente (a) y la intersección (b) de la recta de regresión
	a = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
	b = (sumY - a * sumX) / n;
}

int main() {
	//std::vector<double> x;
	//std::vector<double> y;
	//int n;
	
	/*std::cout << "Ingrese la cantidad de datos: ";
	std::cin >> n;
	
	for (int i = 0; i < n; i++) {
		double datoX, datoY;
		std::cout << "Ingrese el dato " << i + 1 << " de x: ";
		std::cin >> datoX;
		std::cout << "Ingrese el dato " << i + 1 << " de y: ";
		std::cin >> datoY;
		
		x.push_back(datoX);
		y.push_back(datoY);
	}*/
	
	std::vector<double> x = {0.5, 0.8, 1.3, 2.0};
	std::vector<double> y = {-0.716, -0.103, 3.419, 52.598};
	
	double a, b;
	regresionLineal(x, y, a, b);
	
	std::cout << "La ecuación de regresión lineal es: y = " << a << "x + " << b << std::endl;
	
	return 0;
}
