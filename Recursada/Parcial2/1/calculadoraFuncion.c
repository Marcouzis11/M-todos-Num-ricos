#include <math.h>

float funcion(float);

int main()
{
    float x = 0;
    float y = 0;
    printf("introduzca el X\n");
    scanf("%f", &x);
    y = funcion(x);
    printf("%f\n", y);
}

float funcion (float x)
{
    return ((2*x)/(x*x+1))-cos(x);
}