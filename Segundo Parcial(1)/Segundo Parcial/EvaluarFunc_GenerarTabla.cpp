#include <iostream>
#include <math.h>

// Definición de la función a evaluar. Puedes cambiar esta función según lo necesites.
double funcion(double x) {
	return exp(sqrt(1+x))*log(1+2*pow(x,2));  // Ejemplo: sin(x) + cos(x)
}

int main() {
	double inicio, fin;
	int num_puntos;
	
	// Solicitar al usuario el intervalo y la cantidad de puntos
	std::cout << "Ingrese el inicio del intervalo: ";
	std::cin >> inicio;
	std::cout << "Ingrese el fin del intervalo: ";
	std::cin >> fin;
	std::cout << "Ingrese la cantidad de sub-intervalos: ";
	std::cin >> num_puntos;
	num_puntos++;
	// Calcular el paso para que los puntos sean equidistantes
	double paso = (fin - inicio) / (num_puntos - 1);
	
	// Evaluar la función en cada punto y mostrar el resultado
	std::cout << "Evaluando la funcion en " << num_puntos << " puntos equidistantes:\n------------------------------------------------\n";
	for (int i = 0; i < num_puntos; ++i) {
		double x = inicio + i * paso;
		double y = funcion(x);
		std::cout << "x = " << x << ", f(x) = " << y << '\n';
	}
	std::cout << "\n---------------------------------------------\n\n";
	for (int i = 0; i < num_puntos; ++i) {
		double x = inicio + i * paso;
		double y = funcion(x);
		std::cout << x << "\t\t" << y << '\n';
	}
	return 0;
}


