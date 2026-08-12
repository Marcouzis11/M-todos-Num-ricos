#include <stdio.h>
#include <math.h>
#include <iostream>

using namespace std;

double funcion(double x)
{
  return -x + 2;
}

double derivada(double x)
{
  double h = 0.000001;
  return ((funcion(x + h) - funcion(x)) / h);
}

int main()
{

  int pasos = 0;

  double epsilon, xp, x, error;

  cout << "Ingrese el X inicial" << endl;

  cin >> x;

  cout << "Ingrese el valor de epsilon" << endl;

  cin >> epsilon;

  do
  {

    xp = (x - (funcion(x) / derivada(x)));

    if (derivada(xp) > 1)
    { //Si el resultado de la derivada esta a la izquierda pongo <, de lo contrario pongo menor;

      pasos++;

      error = (fabs(xp - x) / fabs(xp)); //Error absoluto
      // error porcentual = error exacto * 100 /raiz
      //error = pow((xp-x),2) / pow(x,2);    //Error relativo

      if (pasos == 1000)
      {
        cout << "No se ha encontrado raices, inicie con otro intervalo o un menor error" << endl;
        exit(EXIT_FAILURE);
      }

      x = xp;
    }
    else
    {

      cout << "Error de convergencia, inicie con otro epsilon mayor" << endl;
      exit(EXIT_FAILURE);
    }

  } while (error > epsilon);

  cout << " La raiz es " << x << "\n numero de pasos : " << pasos << "\n con un error : " << error << endl;

  return 0;
}