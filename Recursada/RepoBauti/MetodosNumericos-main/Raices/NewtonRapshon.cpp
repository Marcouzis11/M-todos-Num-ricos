#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

/////////////////////
///DEFINIR TOLERANCIA
/////////////////////
#define ERROR (1e-8)
//////////////////////////////////
///VALOR INICIAL(por defecto UNO)
/////////////////////////////////
#define INICIAL 0.5
///
#define ha (0.01)
///
double function(double);

double derivada(double);

///////////////////////////////
///DEFINIR FUNCION
///////////////////////////////
double function(double x) {
	return 3*x + sin(x) - exp(x);
}

////////////////////////////
///derivada function(x)
////////////////////////////
double derivada(double x, double h) {
	return (function(x + h) - function(x - h)) / (2 * h);
}


int main() {
	
	double xViejo = INICIAL;
	double xNuevo, eAproximado, ePorcentual = 0;
	double tolerancia = ERROR;
	int iteraciones = 0;
	
	do {
		////////////////
		///usando limite
		/////////////////
		/*double lim = (function(xViejo + (0.01)) - function(xViejo)) / (0.01); //limite original
		//double lim =(function(xViejo+0.01)- function(xViejo - 0.01))/(2*0.01);
		//double lim =(3 * function(xViejo) - 4 * function(xViejo - 0.01) + function(xViejo - (2 * 0.01))) / (2 * 0.01);
		///si la derivada de la funcion es muy ceracana a cero no se puede usar el meotodo, ya que nos quedaria una division por cero
		if (fabs(lim) < 0.000001) {
			cout << "Warning: la derivada de la funcion es demasiano cercana a cero, probar con otro metodo" << endl;
			return 0;
		}
		xNuevo = xViejo - (function(xViejo) / lim);
		eAproximado = fabs(xNuevo - xViejo);
		ePorcentual = ((fabs(xNuevo - xViejo) / xNuevo) * 100);
		xViejo = xNuevo;
		iteraciones++;
		cout << iteraciones << endl;*/
		
		
		//////////////////////////
		///usando funcion derivada
		//////////////////////////
		if (fabs(derivada(xViejo,ha)) < 0.001) {
		cout << "Warning: la derivada de la funcion es demasiano cercana a cero, probar con otro metodo" << endl;
		cout<<"Numero de iteraciones: "<<iteraciones;
		return 0;
		}
		
		xNuevo = xViejo - (function(xViejo) / derivada(xViejo,ha)); 
		eAproximado = fabs(xNuevo - xViejo);
		ePorcentual = ((fabs(xNuevo- xViejo)/xNuevo)*100);
		xViejo = xNuevo;
		iteraciones++;
		cout << iteraciones << endl;
		
	} while (eAproximado > tolerancia && iteraciones < 10000);
	
	cout << "Raiz: "  << fixed << setprecision(20)<< xNuevo << "\nError: " << eAproximado << "\nIteraciones: " << iteraciones << endl;
	cout << " function(xNuevo)=" << (int) function(xNuevo) << endl;
	cout << "Error porcentual: " << ePorcentual << endl;
	return 0;
}
