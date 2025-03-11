#include <stdio.h>
#include <math.h>

double f(double x) {
    return (exp(sqrt(1 + x)) * log(1 + 2 * pow(x, 2))) / (2 * sqrt(1 + x)) +
           (4 * exp(sqrt(1 + x)) * x) / (1 + 2 * pow(x, 2));
}

double function(double x, double v) {
    return 10 * exp(0.05 * v * pow(x, 0.02));
}

int main(int argc, char *argv[]) {
    //Case 7: escritura en archivo
    FILE *readPtr;

    int points;
    double integral, a, b;
    double two[] = {1, 1, -0.577350269, 0.577350269};
    double three[] = {0.5555556, 0.8888889, 0.5555556, -0.774596669, 0.0, 0.774596669};
    double four[] = {0.3478548, 0.6521452, 0.6521452, 0.3478548, -0.861136312, -0.339981044, 0.339981044, 0.861136312};
    double five[] = {0.2369269, 0.4786287, 0.5688889, 0.4786287, 0.2369269, -0.906179846, -0.538469310, 0.0,
                     0.538469310, 0.906179846};
    double six[] = {0.1713245, 0.3607616, 0.4679139, 0.4679139, 0.3607616, 0.1713245, -0.932469514, -0.661209386,
                    -0.238619186, 0.238619186, 0.661209386, 0.932469514};

    printf("Ingrese el valor del limite de integracion inferior\n");
    scanf("%lf", &a);
    printf("Ingrese el valor del limite de integracion superior\n");
    scanf("%lf", &b);

    do {
        printf("Ingrese la cantidad de puntos (recuerde que dicho valor esta entre 2 y 6). Ingrese 0 para salir\n");
        scanf("%d", &points);
        switch (points) {
            case 0:
                break;
            case 2:
                integral = ((b - a) / 2) *
                           (two[0] * f(((b - a) * two[2] + b + a) / 2) + two[1] * f(((b - a) * two[3] + b + a) / 2));
                printf("El valor de la integral es: %.100lf\n", integral);
                break;
            case 3:
                integral = ((b - a) / 2) *
                           (three[0] * f(((b - a) * three[3] + b + a) / 2) +
                            three[1] * f(((b - a) * three[4] + b + a) / 2) +
                            three[2] * f(((b - a) * three[5] + b + a) / 2));
                printf("El valor de la integral es: %lf\n", integral);
                break;
            case 4:
                integral = ((b - a) / 2) *
                           (four[0] * f(((b - a) * four[4] + b + a) / 2) +
                            four[1] * f(((b - a) * four[5] + b + a) / 2) +
                            four[2] * f(((b - a) * four[6] + b + a) / 2) +
                            four[3] * f(((b - a) * four[7] + b + a) / 2));
                printf("El valor de la integral es: %lf\n", integral);
                break;
            case 5:
                integral = ((b - a) / 2) *
                           (five[0] * f(((b - a) * five[5] + b + a) / 2) +
                            five[1] * f(((b - a) * five[6] + b + a) / 2) +
                            five[2] * f(((b - a) * five[7] + b + a) / 2) +
                            five[3] * f(((b - a) * five[8] + b + a) / 2) +
                            five[4] * f(((b - a) * five[9] + b + a) / 2));
                printf("El valor de la integral es: %lf\n", integral);
                break;
            case 6:
                integral = ((b - a) / 2) *
                           (six[0] * f(((b - a) * six[6] + b + a) / 2) +
                            six[1] * f(((b - a) * six[7] + b + a) / 2) +
                            six[2] * f(((b - a) * six[8] + b + a) / 2) +
                            six[3] * f(((b - a) * six[9] + b + a) / 2) +
                            six[4] * f(((b - a) * six[10] + b + a) / 2) +
                            six[5] * f(((b - a) * six[11] + b + a) / 2));
                printf("El valor de la integral es: %lf\n", integral);
                break;
            case 7:
                readPtr = fopen("data.txt", "w");
                for (int i = 0; i < 21; ++i) {
                    integral = ((b - a) / 2) *
                               (three[0] * function(((b - a) * three[3] + b + a) / 2, 2.5*i) +
                                three[1] * function(((b - a) * three[4] + b + a) / 2, 2.5*i) +
                                three[2] * function(((b - a) * three[5] + b + a) / 2, 2.5*i));
                    fprintf(readPtr, "%lf\t%lf\n", 2.5*i, integral);
                }
                break;
            default:
                printf("La opcion ingresada no es valida. Porfavor ingrese una opcion valida\n");
                break;
        }
    } while (points != 0);

    return 0;
}