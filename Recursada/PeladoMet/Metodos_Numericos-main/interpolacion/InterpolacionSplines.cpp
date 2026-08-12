#include <stdio.h>
#include <math.h>

int main() {

    FILE *fp;
    char ch;
    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        puts("No se puede abrir el archivo");
    }

    //contador de filas
    int filas = 0;
    char c;
    int maxValues = 0;
    int columnas;

    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            filas++;
        }
    }

    printf("numero de filas=%i\n", filas);

    rewind(fp);
    double x[filas];
    double y[filas]; //Termino independiente


    //Cargo los datos leidos en el array
    int xi = 0;
    int yi = 0;
    int j = 0;
    int i;
    for (i = 0; i < filas; i++) {
        j = 0;

        do {
            if (j == 0) {
                fscanf(fp, "%lf", &(x[xi]));
                xi++;
                j++;
            } else {
                fscanf(fp, "%lf", &(y[yi]));
                yi++;
            }
        } while ((c = fgetc(fp)) != '\n');

    }

    columnas = j;

    printf("numero de columnas=%i\n\n", columnas);

    //imprimo la matriz para verificar que lo leyo correctamente

    printf("LOS ELEMENTOS DE LA TABLA SON:\n");

    for (i = 0; i < filas; i++) {
        printf("%lf \t %lf", x[i], y[i]);
        printf("\n");
    }

    //Puntos deben estar ordenados!

    printf("SPLINE CUBICA NATURAL\n");

    int n = filas;

    //Error real: valor real - valor del metodo
    //Error relativo: (valor real - valor del metodo) / valor real
    //Error porcentual : idem del de arriba, pero multiplicarlo por 100.

    //tengo 4(n-1) incognitas
    int N = 4 * (n - 1) + 1; // N total
    double m[N][N];
    double b[N];
    double aux;
    double valor; //valor buscado
    double resultado; // solucion!!

    printf("Cantidad de puntos: \n", n);

    //inicializo mis matrices en cero
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            m[i][j] = 0;
        b[i] = 0;
    }


    //Polinomios
    for (int i = 1; i < n; i++) {
        m[i * 2 - 1][i * 4 - 3] = 1;
        m[i * 2 - 1][i * 4 - 2] = x[i - 1];
        m[i * 2 - 1][i * 4 - 1] = pow(x[i - 1], 2);
        m[i * 2 - 1][i * 4] = pow(x[i - 1], 3);
        b[i * 2 - 1] = y[i - 1];

        m[i * 2][i * 4 - 3] = 1;
        m[i * 2][i * 4 - 2] = x[i + 1 - 1];
        m[i * 2][i * 4 - 1] = pow(x[i + 1 - 1], 2);
        m[i * 2][i * 4] = pow(x[i + 1 - 1], 3);
        b[i * 2] = y[i + 1 - 1];
    }


    //Derivadas Primeras
    j = 2 * (n - 1);
    for (int i = 2; i <= n - 1; i++) {
        m[j + i - 1][(i - 1) * 4 - 3] = 0;
        m[j + i - 1][(i - 1) * 4 - 2] = 1;
        m[j + i - 1][(i - 1) * 4 - 1] = 2 * x[i - 1];
        m[j + i - 1][(i - 1) * 4] = 3 * pow(x[i - 1], 2);

        m[j + i - 1][(i - 1) * 4 + 1] = 0;
        m[j + i - 1][(i - 1) * 4 + 2] = -1;
        m[j + i - 1][(i - 1) * 4 + 3] = -2 * x[i - 1];
        m[j + i - 1][(i - 1) * 4 + 4] = -3 * pow(x[i - 1], 2);
    }



    // Derivadas Segundas
    j = 2 * (n - 1) + (n - 2);
    for (int i = 2; i <= n - 1; i++) {
        m[j + i - 1][(i - 1) * 4 - 3] = 0;
        m[j + i - 1][(i - 1) * 4 - 2] = 0;
        m[j + i - 1][(i - 1) * 4 - 1] = 2;
        m[j + i - 1][(i - 1) * 4] = 6 * x[i - 1];

        m[j + i - 1][(i - 1) * 4 + 1] = 0;
        m[j + i - 1][(i - 1) * 4 + 2] = 0;
        m[j + i - 1][(i - 1) * 4 + 3] = -2;
        m[j + i - 1][(i - 1) * 4 + 4] = -6 * x[i - 1];
    }

    //Condicion de Splain natural (se puede cambiar!! OJO) (recordar que aca lo estoy igualando a la derivada
    // segunda =0!!
    j = 2 * (n - 1) + (n - 2) + (n - 2);
    m[j + 1][1] = 0;
    m[j + 1][2] = 0;
    m[j + 1][3] = 2;
    m[j + 1][4] = 6 * x[0];
    b[j + 1] = 0;

    m[j + 2][N - 4] = 0;
    m[j + 2][N - 3] = 0;
    m[j + 2][N - 2] = 2;
    m[j + 2][N - 1] = 6 * x[n - 1];
    b[j + 2] = 0;

    // muevo la matriz un indice!!
    for (int i = 1; i < N; i++) {
        for (j = 1; j < N; j++) {
            m[i - 1][j - 1] = m[i][j];
        }
        b[i - 1] = b[i];
    }


    //Imprimo mi matriz
    for (int i = 0; i < N - 1; i++) {
        for (j = 0; j < N - 1; j++) {
            printf("%lf", m[i][j]);
        }

        printf("-----> %lf", b[i]);

        printf("\n");
    }

    //redefino mi N
    int Nnuevo = N - 1;
    double errorMinimo = 1e-6;
    double f;

    printf("%lf N", Nnuevo);

    //triangulacion superior
    for (int i = 0; i < Nnuevo - 1; i++) {
        //pivoteo
        int cambio = 0;
        if (fabs(m[i][i]) < errorMinimo) {
            for (int j = i + 1; j <= Nnuevo - 1; j++) {
                if (fabs(m[j][i]) > errorMinimo) {
                    for (int k = i; k <= Nnuevo - 1; k++) {
                        aux = m[i][k];
                        m[i][k] = m[j][k];
                        m[j][k] = aux;
                    }
                    aux = b[i];
                    b[i] = b[j];
                    b[j] = aux;
                    cambio = 1;
                    break;
                }
            }
            if (cambio == 0) {
                printf("El sistema es singular! no se puede resolver: %d --------m: %lf", i, m[i][i]);
                return 0;
            }
        }
        //***********************FIN PIVOTEO********************************
        for (int j = i + 1; j <= Nnuevo - 1; j++) {
            f = (-m[j][i]) / (m[i][i]);
            for (int k = i; k <= Nnuevo - 1; k++)
                m[j][k] = m[j][k] + f * m[i][k];
            b[j] = b[j] + f * b[i];
        }
    }

    //imprime la matriz como quedo!!

    printf("\nLa Matriz triangular superior quedo: \n");

    for (int i = 0; i < Nnuevo; i++) {
        for (int j = 0; j < Nnuevo; j++) {
            printf("%.2lf ", m[i][j]);
        }

        printf(" -----> %lf", b[i]);
        printf("\n");

    }


    //sustitucion regresiva
    double suma;
    double a[Nnuevo]; //vector de soluciones

    //valor de la ultima variable
    a[Nnuevo - 1] = b[Nnuevo - 1] / m[Nnuevo - 1][Nnuevo - 1];

    printf(" ------ Soluciones ----- \n");

    printf("a[%.0lf]= %lf", (double)(Nnuevo-1), a[Nnuevo-1]);

    for (int i = Nnuevo - 2; i >= 0; i--) {
        suma = b[i];
        for (int j = i + 1; j <= Nnuevo - 1; j++) {
            suma -= m[i][j] * a[j];
        }
        a[i] = (suma) / m[i][i];

        printf("\n a[%i]= %lf", i, a[i]);
    }

    printf("\n");

    //Modificacion!!
    double valorFinal;
    double delta;


    printf("Ingrese el valor a interpolar \n");
    scanf("%lf", &valor);
    //cout << "Ingrese el valor final del intervalo ";
    //cin >> valorFinal;
    //cout << "Ingrese el incremento: ";
    //cin >> delta;
    //cout<< endl;

    printf("X :         Y: \n");
    //while(valor<=valorFinal){

    for (int i = 0; i < n - 1; i++) {
        if (x[i] <= valor && valor <= x[i + 1]) {
            j = i * 4;
            resultado = a[j] + a[j + 1] * valor + a[j + 2] * pow(valor, 2) + a[j + 3] * pow(valor, 3);

            break;
        }
    }

    printf("%lf %lf", valor, resultado);
    printf("\n");
    valor += delta;
    //}
}

