#include <iostream>
#include <math.h>
using namespace std;

int main(int argc, char *argv[]) {
	double 	valor_exacto=0.549620740148,
			valor_medido=0.537755,
			resultado=0;
	
	resultado = fabs (valor_medido-valor_exacto)/valor_exacto * 100;
	printf("..................................................\n");
	printf("  (%.6lf-%.6lf)\t*100\n",valor_medido,valor_exacto);
	printf("----------------------\n\t%.6lf\n\n\n",valor_exacto);
	printf("Valor exacto: %lf\n", valor_exacto); 
	printf("Valor medido: %lf\n", valor_medido); 
	printf("Error porcentual: %lf%%", resultado); 
	printf("\n..................................................\n");
	return 0;
}

