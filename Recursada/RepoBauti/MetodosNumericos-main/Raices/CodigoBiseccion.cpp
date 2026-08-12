#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

/////////////////////
///DEFINIR TOLERANCIA
/////////////////////
#define ERROR pow(10, -8)

/////////////////////
///DEFINIR INTERVALO
/////////////////////
#define A 0.7
#define B 1.3

double function(double);

///////////////////
///DEFINIR FUNCION
//////////////////
double function(double x) {
	//return (31.06*pow(x,3)-74.5*pow(x,2)+58.8*x-15.4) ;
	return 0.91*exp(pow(x,2))-2.3;
}

int main() {
	
	// intervalo que abarca la raiz
	double a = A;
	double b = B;
	
	double cn, cv;
	double eAproximado, ePorcentual = 0;
	double tolerancia = ERROR;
	int iteraciones = 0;
	
	cout << "BISECCION" << endl;
	
	if (function(a) * function(b) < 0) { //Si hay una raiz entre a y b
		do { 
			if (iteraciones == 0) {
				cn = (a + b) / 2;
				cv = a;
				eAproximado = fabs(cn - cv);
				ePorcentual = fabs(eAproximado / cn) * 100;
			} else {
				if (function(a) * function(cn) < 0) { //Si hay una raiz entre a y cn, se mueve cn a b
					b = cn;
					cv = cn; //Se actualiza el c
					cn = (a + b) / 2;
					eAproximado = fabs(cn - cv);
					ePorcentual = (eAproximado / cn) * 100;
				} else if (function(b) * function(cn) < 0) { //Si hay una raiz entre by cn, se mueve cn a a
					a = cn;
					cv = cn; //Se actualiaz el c
					cn = (a + b) / 2;
					eAproximado = fabs(cn - cv);
					ePorcentual = (eAproximado / cn) * 100;
				} else {
					break;
				}
			}
			iteraciones++;
		} while (eAproximado > tolerancia);
		cout  << fixed << setprecision(20)<< "Raiz: "  << fixed << setprecision(20)<< cn << "\nError aproximado: " << eAproximado << "\nError porcentual: " << ePorcentual
			<< "\nIteraciones: " << iteraciones << endl;
		cout << "la funcion en dicho punto es: " << (int) function(cn) << endl;
	} else {
		cout << "La funcion no tiene raices o  la misma no se encuentra en el intervalo elegido" << endl;
	}
	
	return 0;
}
