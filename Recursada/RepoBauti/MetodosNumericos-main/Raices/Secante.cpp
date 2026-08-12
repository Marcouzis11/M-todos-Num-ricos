#include <iostream>
#include <math.h>
#include <iomanip>

/////////////////////
///DEFINIR INTERVALO
/////////////////////
#define a 9
#define b 7

using namespace std;

double funcion(double);

///////////////////
///DEFINIR FUNCION
//////////////////
double funcion(double x) {
	return x * cosh(10 / x) - x - 6;
}

int main() {
	double xNuevo, eAproximado;
	double xViejoViejo = a;
	double xViejo = b;
	double tolerancia = pow(10, -5);
	int iteraciones = 0;
	
	do {
		xNuevo = xViejo - (funcion(xViejo) * (xViejoViejo - xViejo)) / (funcion(xViejoViejo) - funcion(xViejo));
		eAproximado = fabs(xNuevo - xViejo);
		xViejoViejo = xViejo;
		xViejo = xNuevo;
		iteraciones++;
	} while (eAproximado > tolerancia || iteraciones > 10000);
	
	cout << "Raiz: "<< fixed << setprecision(20) << xNuevo << "\nError: " << eAproximado << "\nIteraciones: " << iteraciones << endl;
	return 0;
}
