/* edo.cpp -- ECUACIONES DIFERENCIALES ORDINARIAS   (Guia 9)
 *
 *  Problema de valor inicial:   dy/dx = f(x,y)   con   y(x0) = y0
 *
 *  Metodos: Euler, Heun (Euler mejorado, con corrector iterado), Punto Medio,
 *  Ralston (RK2) y Runge-Kutta 4.  Muestra la PRIMERA ITERACION paso a paso,
 *  la tabla completa, el error exacto si se da la solucion analitica, la
 *  comparacion entre metodos y el efecto de cambiar h.
 *
 *  Todo queda en salida_edo.txt
 */
#include "mn_comun.h"
#include "mn_euler_sistemas.h"

#define MAXPASOS 20000

/* Control de que filas se muestran en la tabla: los enunciados suelen pedir
   "en intervalos de 0.2" o "los ultimos 5 puntos" aunque h sea 0.01. */
static int VER_MODO = 0;          /* 0 = todas , 1 = cada k pasos , 2 = ultimos N */
static int VER_K = 1, VER_N = 5;

static int fila_visible(int i, int n) {
    if (VER_MODO == 1) return (i % VER_K == 0) || i == n;
    if (VER_MODO == 2) return i > n - VER_N;
    return 1;
}

static char F[256] = "";          /* f(x,y) = dy/dx */
static char YEX[256] = "";        /* solucion exacta y(x), opcional */
static double X0, Y0, HP, XF;
static int NPASOS;

static double fxy(double x, double y) { return evalf2(F, x, y); }
static double yex(double x) { return evalf(YEX, x); }

/* ------------------------------------------------------------------ */
static void glosario(void) {
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   dy/dx = f(x,y)  ->  la ecuacion diferencial: da la PENDIENTE en cada punto\n");
    P("   x0 , y0 = condicion inicial (el punto por donde pasa la solucion)\n");
    P("   h = paso: cuanto se avanza en x en cada iteracion\n");
    P("   x_i = x0 + i*h        y_i = valor APROXIMADO de y en x_i\n");
    P("   phi = pendiente que usa el metodo para avanzar (cada metodo la calcula distinto)\n");
    P("   La formula general de todos estos metodos es:   y_(i+1) = y_i + phi * h\n");
    P("   error exacto = |y_i - y(x_i) exacta|   (solo si se conoce la solucion analitica)\n");
}

static void cargar(void) {
    P("\n=== CARGAR EL PROBLEMA DE VALOR INICIAL ===\n");
    P("\nEscribi la derivada usando x e y. Ejemplos:\n");
    P("   dy/dx = 4*exp(0.8*x) - 0.5*y      ->  escribis:  4*exp(0.8*x) - 0.5*y\n");
    P("   dy/dx = x + y                      ->  escribis:  x + y\n");
    P("   dy/dx = -2*x^3 + 12*x^2 - 20*x + 8.5  (no depende de y: igual se escribe asi)\n");
    leer_funcion("\nf(x,y) = dy/dx =  ", F);
    X0 = leer_num("x0  (valor inicial de x) = ");
    Y0 = leer_num("y0  (valor inicial de y, o sea y(x0)) = ");
    HP  = leer_num("h   (paso, ej: 0.5) = ");
    if (HP == 0) { P("h no puede ser 0.\n"); HP = 0.5; }
    XF = leer_num("x final  (hasta donde integrar, ej: 4) = ");
    NPASOS = (int)floor(fabs((XF - X0) / HP) + 0.5);
    if (NPASOS < 1) NPASOS = 1;
    if (NPASOS > MAXPASOS) { NPASOS = MAXPASOS; P("Se limita a %d pasos.\n", MAXPASOS); }
    P("Se haran %d pasos, desde x = %.10g hasta x = %.10g.\n", NPASOS, X0, X0 + NPASOS * HP);
    VER_MODO = 0; VER_K = 1; VER_N = 5;
    if (NPASOS > 25) {
        P("\nSon %d pasos: la tabla seria larguisima. Como la mostramos?\n", NPASOS);
        P("  1) Todas las filas\n");
        P("  2) Cada cierta cantidad de pasos   ('en intervalos de x de 0.2')\n");
        P("  3) Solo los ultimos puntos         ('muestre los ultimos 5 puntos')\n");
        int v = leer_int("Opcion (1-3): ");
        if (v == 2) {
            double dx = leer_num("Cada cuanto en x  (ej: 0.2) = ");
            VER_K = (int)floor(fabs(dx / HP) + 0.5);
            if (VER_K < 1) VER_K = 1;
            VER_MODO = 1;
            P("Se mostrara una fila cada %d pasos (cada %.10g en x).\n", VER_K, VER_K * HP);
        } else if (v == 3) {
            VER_N = leer_int("Cuantos puntos finales  (ej: 5) = ");
            if (VER_N < 1) VER_N = 1;
            VER_MODO = 2;
        }
    }
    YEX[0] = 0;
    leer_linea("Solucion EXACTA y(x), si el enunciado la da (ej: 4/1.3*(exp(0.8*x) - exp(-0.5*x)) + 2*exp(-0.5*x); ENTER = no): ", YEX, 256);
    if (YEX[0] && !expr_valida(YEX)) { printf("   No entendi esa funcion: se ignora.\n"); YEX[0] = 0; }
    if (YEX[0]) P("Se usara y(x) = %s para calcular los errores exactos.\n", YEX);
}

/* ------------------------------------------------------------------ */
/* Un paso de cada metodo. Devuelve y_(i+1) y deja la pendiente en *phi */
/* met: 0 Euler, 1 Heun, 2 Punto medio, 3 Ralston, 4 RK4              */
/* ------------------------------------------------------------------ */
static int CORR_ITER = 1;         /* iteraciones del corrector de Heun */

static double paso(int met, double x, double y, double h, double *phi) {
    double k1 = fxy(x, y), k2, k3, k4;
    switch (met) {
        case 0:
            *phi = k1;
            return y + k1 * h;
        case 1: {                                  /* Heun */
            double yp = y + k1 * h;                /* predictor (Euler) */
            for (int it = 0; it < CORR_ITER; it++) {
                k2 = fxy(x + h, yp);
                yp = y + (k1 + k2) / 2 * h;        /* corrector */
            }
            *phi = (yp - y) / h;
            return yp;
        }
        case 2:                                    /* Punto medio */
            k2 = fxy(x + h / 2, y + k1 * h / 2);
            *phi = k2;
            return y + k2 * h;
        case 3:                                    /* Ralston (RK2) */
            k2 = fxy(x + 0.75 * h, y + 0.75 * k1 * h);
            *phi = k1 / 3 + 2 * k2 / 3;
            return y + (*phi) * h;
        default:                                   /* RK4 clasico */
            k2 = fxy(x + h / 2, y + k1 * h / 2);
            k3 = fxy(x + h / 2, y + k2 * h / 2);
            k4 = fxy(x + h, y + k3 * h);
            *phi = (k1 + 2 * k2 + 2 * k3 + k4) / 6;
            return y + (*phi) * h;
    }
}

static const char *NOMBRE[5] = {"EULER", "HEUN (Euler mejorado)", "PUNTO MEDIO", "RALSTON (RK2)", "RUNGE-KUTTA 4"};
static const char *ORDEN[5]  = {"1", "2", "2", "2", "4"};

/* ------------------------------------------------------------------ */
/* Explicacion de la formula de cada metodo                            */
/* ------------------------------------------------------------------ */
static void formula(int met) {
    switch (met) {
        case 0:
            H("Formula de EULER (tambien 'Euler hacia adelante' o 'Euler explicito'):\n");
            H("   y_(i+1) = y_i + f(x_i , y_i) * h\n");
            H("Usa la pendiente AL PRINCIPIO del intervalo y avanza en linea recta.\n");
            H("Es de orden 1: el error global es O(h) (si se parte h al medio, el error se parte al medio).\n");
            break;
        case 1:
            H("Formula de HEUN (Euler mejorado / predictor-corrector):\n");
            H("   PREDICTOR:  y*_(i+1) = y_i + f(x_i , y_i) * h            (un paso de Euler)\n");
            H("   CORRECTOR:  y_(i+1)  = y_i + [ f(x_i , y_i) + f(x_(i+1) , y*_(i+1)) ] / 2 * h\n");
            H("Promedia la pendiente del principio y la del final: por eso es mas preciso.\n");
            H("Es de orden 2: el error global es O(h^2).\n");
            break;
        case 2:
            H("Formula del PUNTO MEDIO (midpoint):\n");
            H("   1) y_(i+1/2) = y_i + f(x_i , y_i) * h/2        (medio paso de Euler)\n");
            H("   2) y_(i+1)   = y_i + f(x_(i+1/2) , y_(i+1/2)) * h\n");
            H("Usa la pendiente evaluada en el MEDIO del intervalo. Orden 2: error O(h^2).\n");
            break;
        case 3:
            H("Formula de RALSTON (Runge-Kutta de 2do orden, version de Ralston):\n");
            H("   k1 = f(x_i , y_i)\n");
            H("   k2 = f(x_i + 3h/4 , y_i + 3*k1*h/4)\n");
            H("   y_(i+1) = y_i + ( k1/3 + 2*k2/3 ) * h\n");
            H("Orden 2, con el menor error de truncamiento entre los RK2.\n");
            break;
        default:
            H("Formula de RUNGE-KUTTA 4 (RK4 clasico):\n");
            H("   k1 = f(x_i , y_i)\n");
            H("   k2 = f(x_i + h/2 , y_i + k1*h/2)\n");
            H("   k3 = f(x_i + h/2 , y_i + k2*h/2)\n");
            H("   k4 = f(x_i + h   , y_i + k3*h)\n");
            H("   y_(i+1) = y_i + ( k1 + 2*k2 + 2*k3 + k4 ) / 6 * h\n");
            H("Promedia 4 pendientes (las del medio pesan doble). Orden 4: error O(h^4),\n");
            H("por eso con el mismo h es muchisimo mas preciso que Euler o Heun.\n");
            break;
    }
}

/* Detalle numerico de la primera iteracion */
static void detalle_primer_paso(int met, double h) {
    double x = X0, y = Y0, k1 = fxy(x, y), k2, k3, k4, yn;
    H("\nPRIMERA ITERACION PASO A PASO  (i = 0 ,  x0 = %.10g , y0 = %.10g , h = %.10g):\n", X0, Y0, h);
    switch (met) {
        case 0:
            H("   f(x0 , y0) = f(%.10g , %.10g) = %.10f\n", x, y, k1);
            H("   y1 = y0 + f(x0,y0)*h = %.10g + (%.10f)*(%.10g) = %.10f\n", y, k1, h, y + k1 * h);
            H("   o sea  y(%.10g) ~ %.10f\n", x + h, y + k1 * h);
            break;
        case 1: {
            double yp = y + k1 * h;
            H("   k1 = f(x0 , y0) = f(%.10g , %.10g) = %.10f\n", x, y, k1);
            H("   PREDICTOR:  y*1 = y0 + k1*h = %.10g + (%.10f)*(%.10g) = %.10f\n", y, k1, h, yp);
            for (int it = 0; it < CORR_ITER; it++) {
                k2 = fxy(x + h, yp);
                double ynew = y + (k1 + k2) / 2 * h;
                if (CORR_ITER > 1) H("   --- corrector, pasada %d:\n", it + 1);
                H("   f(x1 , y*1) = f(%.10g , %.10f) = %.10f\n", x + h, yp, k2);
                H("   pendiente promedio = (k1 + k2)/2 = (%.10f + %.10f)/2 = %.10f\n", k1, k2, (k1 + k2) / 2);
                H("   CORRECTOR:  y1 = y0 + (k1+k2)/2 * h = %.10g + (%.10f)*(%.10g) = %.10f\n",
                  y, (k1 + k2) / 2, h, ynew);
                yp = ynew;
            }
            H("   o sea  y(%.10g) ~ %.10f\n", x + h, yp);
            break;
        }
        case 2:
            k2 = fxy(x + h / 2, y + k1 * h / 2);
            H("   k1 = f(x0 , y0) = f(%.10g , %.10g) = %.10f\n", x, y, k1);
            H("   punto medio:  x0 + h/2 = %.10g      y0 + k1*h/2 = %.10g + (%.10f)*(%.10g) = %.10f\n",
              x + h / 2, y, k1, h / 2, y + k1 * h / 2);
            H("   k2 = f(%.10g , %.10f) = %.10f        (esta es la pendiente que se usa)\n", x + h / 2, y + k1 * h / 2, k2);
            H("   y1 = y0 + k2*h = %.10g + (%.10f)*(%.10g) = %.10f\n", y, k2, h, y + k2 * h);
            H("   o sea  y(%.10g) ~ %.10f\n", x + h, y + k2 * h);
            break;
        case 3:
            k2 = fxy(x + 0.75 * h, y + 0.75 * k1 * h);
            H("   k1 = f(%.10g , %.10g) = %.10f\n", x, y, k1);
            H("   k2 = f(x0 + 3h/4 , y0 + 3*k1*h/4) = f(%.10g , %.10f) = %.10f\n",
              x + 0.75 * h, y + 0.75 * k1 * h, k2);
            H("   phi = k1/3 + 2*k2/3 = %.10f/3 + 2*%.10f/3 = %.10f\n", k1, k2, k1 / 3 + 2 * k2 / 3);
            H("   y1 = y0 + phi*h = %.10g + (%.10f)*(%.10g) = %.10f\n", y, k1 / 3 + 2 * k2 / 3, h, y + (k1 / 3 + 2 * k2 / 3) * h);
            break;
        default:
            k2 = fxy(x + h / 2, y + k1 * h / 2);
            k3 = fxy(x + h / 2, y + k2 * h / 2);
            k4 = fxy(x + h, y + k3 * h);
            H("   k1 = f(x0 , y0)             = f(%.10g , %.10g) = %.10f\n", x, y, k1);
            H("   k2 = f(x0 + h/2 , y0 + k1*h/2) = f(%.10g , %.10f) = %.10f\n", x + h / 2, y + k1 * h / 2, k2);
            H("   k3 = f(x0 + h/2 , y0 + k2*h/2) = f(%.10g , %.10f) = %.10f\n", x + h / 2, y + k2 * h / 2, k3);
            H("   k4 = f(x0 + h   , y0 + k3*h)   = f(%.10g , %.10f) = %.10f\n", x + h, y + k3 * h, k4);
            H("   phi = (k1 + 2*k2 + 2*k3 + k4)/6\n");
            H("       = (%.10f + 2*(%.10f) + 2*(%.10f) + %.10f)/6 = %.10f\n",
              k1, k2, k3, k4, (k1 + 2 * k2 + 2 * k3 + k4) / 6);
            yn = y + (k1 + 2 * k2 + 2 * k3 + k4) / 6 * h;
            H("   y1 = y0 + phi*h = %.10g + (%.10f)*(%.10g) = %.10f\n", y, (k1 + 2 * k2 + 2 * k3 + k4) / 6, h, yn);
            H("   o sea  y(%.10g) ~ %.10f\n", x + h, yn);
            break;
    }
    if (YEX[0]) {
        double ye = yex(X0 + h);
        double y1 = paso(met, X0, Y0, h, &k1);
        H("\n   Valor exacto:  y(%.10g) = %.10f\n", X0 + h, ye);
        H("   Error absoluto exacto = |%.10f - %.10f| = %.3e\n", y1, ye, fabs(y1 - ye));
        if (ye != 0) H("   Error relativo porcentual = %.4f %%\n", 100 * fabs(y1 - ye) / fabs(ye));
    }
}

/* ------------------------------------------------------------------ */
static void correr(int met) {
    double h = HP, y = Y0, x = X0, phi;
    int n = NPASOS;
    P("\n=== METODO DE %s ===   dy/dx = %s\n", NOMBRE[met], F);
    glosario();
    if (met == 1) {
        P("\nEl corrector de Heun se puede ITERAR (volver a corregir con el valor ya corregido).\n");
        CORR_ITER = leer_int("Cuantas pasadas del corrector  (1 = la version normal del libro): ");
        if (CORR_ITER < 1) CORR_ITER = 1;
    }
    hoja_reset();
    H("METODO DE %s   (orden %s)\n", NOMBRE[met], ORDEN[met]);
    H("Ecuacion:  dy/dx = f(x,y) = %s\n", F);
    H("Condicion inicial:  y(%.10g) = %.10g        Paso:  h = %.10g\n", X0, Y0, h);
    if (YEX[0]) H("Solucion exacta (dato):  y(x) = %s\n", YEX);
    H("\n");
    formula(met);
    if (met == 1 && CORR_ITER > 1) H("(Aca el corrector se aplica %d veces en cada paso.)\n", CORR_ITER);
    detalle_primer_paso(met, h);

    H("\nTABLA DE RESULTADOS:\n");
    if (YEX[0]) {
        H("    i |      x_i       |    y_i (aprox)   |    y exacta     |  error abs.  |  error %%\n");
        H("  ----+----------------+------------------+-----------------+--------------+----------\n");
    } else {
        H("    i |      x_i       |    y_i (aprox)   |  pendiente phi\n");
        H("  ----+----------------+------------------+----------------\n");
    }
    double emax = 0;
    for (int i = 0; i <= n; i++) {
        if (i > 0) { y = paso(met, x, y, h, &phi); x += h; }
        else phi = fxy(x, y);
        if (YEX[0]) { double ee = fabs(y - yex(x)); if (ee > emax) emax = ee; }
        if (!fila_visible(i, n)) continue;
        if (YEX[0]) {
            double ye = yex(x), ea = fabs(y - ye);
            if (ea > emax) emax = ea;
            H("  %3d | %14.8f | %16.10f | %15.10f | %12.3e | %8.4f\n",
              i, x, y, ye, ea, ye != 0 ? 100 * ea / fabs(ye) : 0.0);
        } else {
            H("  %3d | %14.8f | %16.10f | %14.8f\n", i, x, y, phi);
        }
    }
    H("\nRESULTADO:  y(%.10g) ~ %.10f\n", x, y);
    if (YEX[0]) {
        double ye = yex(x);
        H("   Valor exacto:            y(%.10g) = %.10f\n", x, ye);
        H("   Error absoluto exacto:   |%.10f - %.10f| = %.3e\n", y, ye, fabs(y - ye));
        if (ye != 0) H("   Error relativo:          %.6f %%\n", 100 * fabs(y - ye) / fabs(ye));
        H("   Error maximo en toda la tabla: %.3e\n", emax);
        H("\n   NOTA: el error de estos metodos se ACUMULA paso a paso. El error de la ultima\n");
        H("   fila incluye todo lo que se fue arrastrando desde el principio.\n");
    } else {
        H("\n   El enunciado no da la solucion exacta, asi que el error exacto no se puede\n");
        H("   calcular. Para estimarlo se puede repetir con h/2 y comparar (opcion 8).\n");
    }
    hoja_que_escribir("La formula del metodo escrita con sus pasos",
                      "El valor de h y la condicion inicial",
                      "La PRIMERA iteracion con todos los numeros reemplazados",
                      "La tabla de x_i e y_i (o la captura si es larga)",
                      "El resultado pedido con todos los decimales",
                      YEX[0] ? "El error absoluto y relativo contra la solucion exacta" : "Que el error no se puede calcular sin la solucion exacta",
                      "El orden del metodo y que significa (como baja el error al bajar h)",
                      (const char *)0);
    P("\nRESULTADO:  y(%.10g) ~ %.10f\n", x, y);
    if (YEX[0]) P("Exacta = %.10f   error = %.3e   (%.4f %%)\n", yex(x), fabs(y - yex(x)),
                  yex(x) ? 100 * fabs(y - yex(x)) / fabs(yex(x)) : 0.0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void op_comparar(void) {
    double ys[5], x, y, phi;
    P("\n=== COMPARAR TODOS LOS METODOS ===\n");
    hoja_reset();
    H("COMPARACION DE METODOS PARA  dy/dx = %s\n", F);
    H("y(%.10g) = %.10g ,  h = %.10g ,  %d pasos hasta x = %.10g\n", X0, Y0, HP, NPASOS, X0 + NPASOS * HP);
    if (YEX[0]) H("Solucion exacta: y(x) = %s\n", YEX);
    CORR_ITER = 1;
    for (int m = 0; m < 5; m++) {
        y = Y0; x = X0;
        for (int i = 0; i < NPASOS; i++) { y = paso(m, x, y, HP, &phi); x += HP; }
        ys[m] = y;
    }
    double xf = X0 + NPASOS * HP;
    H("\nValor de y en x = %.10g:\n\n", xf);
    H("  METODO                  | orden |        y        ");
    if (YEX[0]) H("|  error abs.  |  error %%");
    H("\n  ------------------------+-------+-----------------");
    if (YEX[0]) H("+--------------+----------");
    H("\n");
    for (int m = 0; m < 5; m++) {
        H("  %-23s |   %-3s | %15.10f ", NOMBRE[m], ORDEN[m], ys[m]);
        if (YEX[0]) {
            double ye = yex(xf), ea = fabs(ys[m] - ye);
            H("| %12.3e | %8.4f ", ea, ye != 0 ? 100 * ea / fabs(ye) : 0.0);
        }
        H("\n");
    }
    if (YEX[0]) H("\n  Valor EXACTO: y(%.10g) = %.10f\n", xf, yex(xf));
    H("\n  Evaluaciones de f por paso:  Euler 1 ;  Heun 2 ;  Punto medio 2 ;  Ralston 2 ;  RK4 4\n");
    H("\nCONCLUSION TIPICA: Euler (orden 1) es el menos preciso. Heun, punto medio y\n");
    H("Ralston (orden 2) mejoran mucho con solo una evaluacion extra de f. RK4 (orden 4)\n");
    H("es el mas preciso: con 4 evaluaciones por paso logra un error muchisimo menor,\n");
    H("por eso es el que mas se usa en la practica.\n");
    hoja_que_escribir("La tabla comparativa con los valores de cada metodo",
                      "El orden de cada metodo y su error",
                      "Cuantas veces evalua f cada uno (el costo)",
                      "La conclusion: precision contra costo",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void op_efecto_h(void) {
    double hs[6], x, y, phi;
    P("\n=== EFECTO DEL PASO h  (convergencia) ===\n");
    P("\nSe resuelve el mismo problema con h, h/2, h/4... y se mira como baja el error.\n");
    int met = leer_int("Metodo (0 Euler, 1 Heun, 2 Punto medio, 3 Ralston, 4 RK4): ");
    if (met < 0 || met > 4) return;
    double xf = leer_num("x donde comparar los valores = ");
    int cuantos = leer_int("Cuantos refinamientos (ej: 4) = ");
    if (cuantos < 1) cuantos = 1;
    if (cuantos > 6) cuantos = 6;
    CORR_ITER = 1;
    hoja_reset();
    H("EFECTO DEL PASO h   -   metodo de %s (orden %s)\n", NOMBRE[met], ORDEN[met]);
    H("dy/dx = %s ,  y(%.10g) = %.10g ,  se compara y(%.10g)\n", F, X0, Y0, xf);
    H("\n       h       |   pasos  |        y(%.6g)      ", xf);
    if (YEX[0]) H("|  error abs.  | error(h)/error(h/2)");
    H("\n  -------------+----------+----------------------");
    if (YEX[0]) H("+--------------+--------------------");
    H("\n");
    double eant = 0;
    for (int k = 0; k < cuantos; k++) {
        double h = HP / pow(2.0, k);
        int n = (int)floor(fabs((xf - X0) / h) + 0.5);
        if (n > 200000) break;
        y = Y0; x = X0;
        for (int i = 0; i < n; i++) { y = paso(met, x, y, h, &phi); x += h; }
        hs[k] = y;
        H("  %12.8g | %8d | %20.12f ", h, n, y);
        if (YEX[0]) {
            double ea = fabs(y - yex(xf));
            H("| %12.3e |", ea);
            if (k > 0 && ea > 0) H(" %18.4f", eant / ea);
            eant = ea;
        }
        H("\n");
    }
    if (YEX[0]) {
        H("\n  Valor EXACTO: y(%.10g) = %.10f\n", xf, yex(xf));
        int f = (met == 0) ? 2 : (met == 4 ? 16 : 4);
        H("\nINTERPRETACION: el metodo es de orden %s, asi que al partir h al medio el error\n", ORDEN[met]);
        H("deberia dividirse por 2^%s = %d. Mira la ultima columna: tiene que dar cerca de %d.\n",
          ORDEN[met], f, f);
        H("Eso CONFIRMA numericamente el orden del metodo.\n");
    }
    hoja_que_escribir("La tabla de h, cantidad de pasos y el valor obtenido",
                      "Como baja el error al partir h al medio",
                      "La relacion con el orden del metodo (2 para orden 1, 4 para orden 2, 16 para RK4)",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* METODO A MEDIDA (multipaso): el enunciado propone una formula nueva  */
/*    y_(i+1) = y_i + h * ( expresion con fi, fim1, ... )               */
/* ------------------------------------------------------------------ */
static void op_multipaso(void) {
    char expr[256];
    P("\n=== METODO DE EDO A MEDIDA (definido por vos) ===\n");
    P("\nSirve cuando el enunciado INVENTA un metodo y pide implementarlo.\n");
    P("Se escribe lo que va dentro del parentesis en:\n");
    P("   y_(i+1) = y_i + h * ( ... )\n");
    P("\nVARIABLES QUE PODES USAR:\n");
    P("   fi    = f(x_i , y_i)            (la pendiente en el paso actual)\n");
    P("   fim1  = f(x_(i-1) , y_(i-1))    (la del paso ANTERIOR)\n");
    P("   fim2  = f(x_(i-2) , y_(i-2))    (dos pasos atras)\n");
    P("   xi , yi = valores actuales ;  h = paso ;  i = numero de paso\n");
    P("\nEJEMPLOS:\n");
    P("   Euler:                    fi\n");
    P("   El del parcial 2022:      2*fi - fim1\n");
    P("   Adams-Bashforth 3:        (23*fi - 16*fim1 + 5*fim2)/12\n");
    vars_reset();
    var_set("fi", 1); var_set("fim1", 1); var_set("fim2", 1);
    var_set("xi", 1); var_set("yi", 1); var_set("h", 1); var_set("i", 1);
    leer_funcion("\nExpresion = ", expr);
    P("\nLos metodos multipaso necesitan ARRANCAR con otro metodo (no hay paso anterior).\n");
    P("  1) Euler        (es lo que suele pedir el enunciado)\n");
    P("  2) Heun\n");
    P("  3) RK4          (mas preciso para el arranque)\n");
    int arr = leer_int("Metodo de arranque (1-3): ");
    int marr = (arr == 3) ? 4 : (arr == 2 ? 1 : 0);
    CORR_ITER = 1;
    double h = HP;
    int n = NPASOS;

    hoja_reset();
    H("METODO MULTIPASO PROPUESTO EN EL ENUNCIADO\n");
    H("Ecuacion:  dy/dx = f(x,y) = %s\n", F);
    H("Condicion inicial:  y(%.10g) = %.10g        h = %.10g        %d pasos\n", X0, Y0, h, n);
    if (YEX[0]) H("Solucion exacta (dato):  y(x) = %s\n", YEX);
    H("\nFormula del metodo:\n");
    H("   y_(i+1) = y_i + h * ( %s )\n", expr);
    H("Arranque (para i = 0, porque todavia no hay paso anterior): %s\n", NOMBRE[marr]);
    H("\nPSEUDOCODIGO (para copiar en la hoja si lo piden):\n");
    H("   ALGORITMO MetodoMultipasoPropuesto\n");
    H("   ENTRADA: f, x0, y0, h, n\n");
    H("   SALIDA:  tabla de (x_i , y_i)\n");
    H("   1   x[0] <- x0 ;  y[0] <- y0\n");
    H("   2   f[0] <- f(x[0], y[0])\n");
    H("   3   // primer paso con un metodo de un solo paso (%s)\n", NOMBRE[marr]);
    if (marr == 0) H("   4   y[1] <- y[0] + h*f[0]\n");
    else if (marr == 1) H("   4   y[1] <- y[0] + h*( f[0] + f(x[0]+h, y[0]+h*f[0]) )/2\n");
    else H("   4   y[1] <- y[0] + h*(k1 + 2k2 + 2k3 + k4)/6     // RK4\n");
    H("   5   x[1] <- x[0] + h ;  f[1] <- f(x[1], y[1])\n");
    H("   6   PARA i = 1 HASTA n-1 HACER\n");
    H("   7       y[i+1] <- y[i] + h * ( %s )\n", expr);
    H("   8       x[i+1] <- x[i] + h\n");
    H("   9       f[i+1] <- f(x[i+1], y[i+1])\n");
    H("   10  FIN PARA\n");
    H("   11  DEVOLVER la tabla\n");

    /* calculo */
    double x = X0, y = Y0, phi;
    double fi = fxy(x, y), fim1 = 0, fim2 = 0;
    double xv[2], yv[2];
    xv[0] = x; yv[0] = y;
    H("\nPRIMERA ITERACION (arranque con %s):\n", NOMBRE[marr]);
    H("   f(x0 , y0) = f(%.10g , %.10g) = %.10f\n", x, y, fi);
    double y1 = paso(marr, x, y, h, &phi);
    H("   y1 = %.10f        (x1 = %.10g)\n", y1, x + h);
    H("\nSEGUNDA ITERACION (ya con la formula propuesta):\n");
    double fi1 = fxy(x + h, y1);
    H("   f(x1 , y1) = f(%.10g , %.10f) = %.10f        -> este es 'fi'\n", x + h, y1, fi1);
    H("   f(x0 , y0) = %.10f                                 -> este es 'fim1'\n", fi);
    vars_reset();
    var_set("fi", fi1); var_set("fim1", fi); var_set("fim2", 0);
    var_set("xi", x + h); var_set("yi", y1); var_set("h", h); var_set("i", 1);
    double inc = evalf(expr, x + h);
    H("   ( %s ) = %.10f\n", expr, inc);
    H("   y2 = y1 + h*( ... ) = %.10f + (%.10g)*(%.10f) = %.10f\n", y1, h, inc, y1 + h * inc);

    H("\nTABLA DE RESULTADOS:\n");
    if (YEX[0]) {
        H("    i |      x_i       |    y_i (aprox)   |    y exacta     |  error abs.  |  error %%\n");
        H("  ----+----------------+------------------+-----------------+--------------+----------\n");
    } else {
        H("    i |      x_i       |    y_i (aprox)\n  ----+----------------+------------------\n");
    }
    /* reinicio y corrida completa */
    x = X0; y = Y0; fi = fxy(x, y);
    double emax = 0;
    for (int i = 0; i <= n; i++) {
        if (i == 1) { y = paso(marr, x, y, h, &phi); x += h; fim1 = fi; fi = fxy(x, y); }
        else if (i > 1) {
            vars_reset();
            var_set("fi", fi); var_set("fim1", fim1); var_set("fim2", fim2);
            var_set("xi", x); var_set("yi", y); var_set("h", h); var_set("i", (double)i - 1);
            y = y + h * evalf(expr, x);
            x += h;
            fim2 = fim1; fim1 = fi; fi = fxy(x, y);
        }
        if (!fila_visible(i, n)) continue;
        if (YEX[0]) {
            double ye = yex(x), ea = fabs(y - ye);
            if (ea > emax) emax = ea;
            H("  %3d | %14.8f | %16.10f | %15.10f | %12.3e | %8.4f\n",
              i, x, y, ye, ea, ye != 0 ? 100 * ea / fabs(ye) : 0.0);
        } else H("  %3d | %14.8f | %16.10f\n", i, x, y);
    }
    H("\nRESULTADO:  y(%.10g) ~ %.10f\n", x, y);
    if (YEX[0]) {
        double ye = yex(x);
        H("   Valor exacto:          y(%.10g) = %.10f\n", x, ye);
        H("   Error absoluto exacto: %.3e\n", fabs(y - ye));
        if (ye != 0) H("   Error relativo:        %.6f %%\n", 100 * fabs(y - ye) / fabs(ye));
    }
    /* comparacion con RK4 y Euler */
    double yr, ye2, xx;
    yr = Y0; xx = X0;
    for (int i = 0; i < n; i++) { yr = paso(4, xx, yr, h, &phi); xx += h; }
    ye2 = Y0; xx = X0;
    for (int i = 0; i < n; i++) { ye2 = paso(0, xx, ye2, h, &phi); xx += h; }
    H("\nCOMPARACION en x = %.10g (mismo h):\n", xx);
    H("   Metodo propuesto : %.10f\n", y);
    H("   Euler            : %.10f\n", ye2);
    H("   RK4              : %.10f\n", yr);
    if (YEX[0]) H("   EXACTO           : %.10f\n", yex(xx));
    H("\nNOTA: este metodo usa DOS valores anteriores, asi que es un metodo MULTIPASO\n");
    H("(a diferencia de Euler o RK4, que son de un solo paso). Su ventaja es que\n");
    H("evalua f una sola vez por paso; su desventaja es que necesita arrancar con otro\n");
    H("metodo y no se le puede cambiar el paso tan facil.\n");
    hoja_que_escribir("El pseudocodigo del metodo (esta arriba, listo para copiar)",
                      "Como se arranca el metodo y por que hace falta",
                      "Las primeras dos iteraciones con los numeros",
                      "La tabla pedida (o los ultimos puntos si piden solo esos)",
                      YEX[0] ? "El error exacto contra la solucion analitica" : "La comparacion con Euler y RK4",
                      (const char *)0);
    P("\nRESULTADO:  y(%.10g) ~ %.10f\n", x, y);
    if (YEX[0]) P("Exacta = %.10f   error = %.3e\n", yex(x), fabs(y - yex(x)));
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* RUNGE-KUTTA A MEDIDA: el enunciado da OTRO esquema de k1..k4         */
/*    k1 = f(xi , yi)                                                   */
/*    k2 = f(xi + a2*h , yi + b21*h*k1)                                 */
/*    k3 = f(xi + a3*h , yi + b31*h*k1 + b32*h*k2)                      */
/*    k4 = f(xi + a4*h , yi + b41*h*k1 + b42*h*k2 + b43*h*k3)           */
/*    y_(i+1) = y_i + h*(w1*k1 + w2*k2 + w3*k3 + w4*k4)                 */
/* ------------------------------------------------------------------ */
static int ET;                         /* cantidad de etapas */
static double CA[4], CB[4][4], CW[4];  /* coeficientes del esquema */

static double paso_rk(double x, double y, double h, double *ks) {
    double inc = 0;
    for (int s = 0; s < ET; s++) {
        double dy = 0;
        for (int j = 0; j < s; j++) dy += CB[s][j] * h * ks[j];
        ks[s] = fxy(x + CA[s] * h, y + dy);
    }
    for (int s = 0; s < ET; s++) inc += CW[s] * ks[s];
    return y + h * inc;
}

static void op_rk_medida(void) {
    double ks[4];
    P("\n=== RUNGE-KUTTA A MEDIDA (esquema dado por el enunciado) ===\n");
    P("\nSirve cuando el enunciado da OTRO Runge-Kutta, con sus propios k1..k4.\n");
    P("El esquema general es:\n");
    P("   k1 = f(xi , yi)\n");
    P("   k2 = f(xi + a2*h , yi + b21*h*k1)\n");
    P("   k3 = f(xi + a3*h , yi + b31*h*k1 + b32*h*k2)\n");
    P("   k4 = f(xi + a4*h , yi + b41*h*k1 + b42*h*k2 + b43*h*k3)\n");
    P("   y(i+1) = y(i) + h*( w1*k1 + w2*k2 + w3*k3 + w4*k4 )\n");
    P("\nESQUEMAS YA CARGADOS:\n");
    P("  1) RK4 clasico        w = 1/6, 2/6, 2/6, 1/6\n");
    P("  2) RK4 3/8            el del parcial 2025:  y + h/8*(k1 + 3k2 + 3k3 + k4)\n");
    P("  3) Heun (RK2)         4) Punto medio (RK2)      5) Ralston (RK2)\n");
    P("  6) ESCRIBIR LOS COEFICIENTES A MANO  (para cualquier otro esquema)\n");
    int op = leer_int("Opcion (1-6): ");
    const char *nm = "RK a medida";
    for (int i = 0; i < 4; i++) { CA[i] = CW[i] = 0; for (int j = 0; j < 4; j++) CB[i][j] = 0; }
    if (op == 1) {
        ET = 4; nm = "RK4 CLASICO";
        CA[1] = 0.5; CB[1][0] = 0.5;
        CA[2] = 0.5; CB[2][1] = 0.5;
        CA[3] = 1;   CB[3][2] = 1;
        CW[0] = 1.0/6; CW[1] = 2.0/6; CW[2] = 2.0/6; CW[3] = 1.0/6;
    } else if (op == 2) {
        ET = 4; nm = "RK4 3/8";
        CA[1] = 1.0/3; CB[1][0] = 1.0/3;
        CA[2] = 2.0/3; CB[2][0] = -1.0/3; CB[2][1] = 1;
        CA[3] = 1;     CB[3][0] = 1; CB[3][1] = -1; CB[3][2] = 1;
        CW[0] = 1.0/8; CW[1] = 3.0/8; CW[2] = 3.0/8; CW[3] = 1.0/8;
    } else if (op == 3) {
        ET = 2; nm = "HEUN (RK2)";
        CA[1] = 1; CB[1][0] = 1; CW[0] = 0.5; CW[1] = 0.5;
    } else if (op == 4) {
        ET = 2; nm = "PUNTO MEDIO (RK2)";
        CA[1] = 0.5; CB[1][0] = 0.5; CW[0] = 0; CW[1] = 1;
    } else if (op == 5) {
        ET = 2; nm = "RALSTON (RK2)";
        CA[1] = 0.75; CB[1][0] = 0.75; CW[0] = 1.0/3; CW[1] = 2.0/3;
    } else {
        P("\nPodes escribir fracciones: 1/3, 2/3, -1/3, 3/8 ...\n");
        do { ET = leer_int("Cuantas etapas k tiene el metodo (2 a 4): "); } while (ET < 2 || ET > 4);
        char msg[80];
        for (int s = 1; s < ET; s++) {
            sprintf(msg, "a%d  (en k%d = f(xi + a%d*h , ...)) = ", s + 1, s + 1, s + 1);
            CA[s] = leer_num(msg);
            for (int j = 0; j <= s - 1; j++) {
                sprintf(msg, "b%d%d (coeficiente de h*k%d dentro de k%d) = ", s + 1, j + 1, j + 1, s + 1);
                CB[s][j] = leer_num(msg);
            }
        }
        for (int s = 0; s < ET; s++) {
            sprintf(msg, "w%d  (peso de k%d en la combinacion final) = ", s + 1, s + 1);
            CW[s] = leer_num(msg);
        }
        double sw = 0;
        for (int s = 0; s < ET; s++) sw += CW[s];
        P("\nControl: la suma de los pesos da %.10g (tiene que dar 1 para que el metodo sea consistente).\n", sw);
    }
    double h = HP;
    int n = NPASOS;
    hoja_reset();
    H("METODO DE RUNGE-KUTTA: %s\n", nm);
    H("Ecuacion:  dy/dx = f(x,y) = %s\n", F);
    H("Condicion inicial:  y(%.10g) = %.10g        h = %.10g        %d pasos\n", X0, Y0, h, n);
    if (YEX[0]) H("Solucion exacta (dato):  y(x) = %s\n", YEX);
    H("\nESQUEMA USADO:\n");
    H("   k1 = f(xi , yi)\n");
    for (int s = 1; s < ET; s++) {
        H("   k%d = f(xi + %.6g*h , yi", s + 1, CA[s]);
        for (int j = 0; j < s; j++) if (CB[s][j] != 0) H(" %s %.6g*h*k%d", CB[s][j] < 0 ? "-" : "+", fabs(CB[s][j]), j + 1);
        H(")\n");
    }
    H("   y(i+1) = y(i) + h*(");
    for (int s = 0; s < ET; s++) H("%s %.6g*k%d", s ? " +" : "", CW[s], s + 1);
    H(")\n");
    H("\nPRIMERA ITERACION PASO A PASO  (x0 = %.10g , y0 = %.10g , h = %.10g):\n", X0, Y0, h);
    {
        double x = X0, y = Y0, inc = 0;
        for (int s = 0; s < ET; s++) {
            double dy = 0;
            for (int j = 0; j < s; j++) dy += CB[s][j] * h * ks[j];
            ks[s] = fxy(x + CA[s] * h, y + dy);
            H("   k%d = f(%.10g , %.10f) = %.10f\n", s + 1, x + CA[s] * h, y + dy, ks[s]);
        }
        for (int s = 0; s < ET; s++) inc += CW[s] * ks[s];
        H("   combinacion = ");
        for (int s = 0; s < ET; s++) H("%s%.6g*(%.10f)", s ? " + " : "", CW[s], ks[s]);
        H(" = %.10f\n", inc);
        H("   y1 = y0 + h*combinacion = %.10g + (%.10g)*(%.10f) = %.10f\n", y, h, inc, y + h * inc);
        if (YEX[0]) H("   Exacta y(%.10g) = %.10f     error = %.3e\n",
                      x + h, yex(x + h), fabs(y + h * inc - yex(x + h)));
    }
    H("\nTABLA DE RESULTADOS:\n");
    if (YEX[0]) {
        H("    i |      x_i       |    y_i (aprox)   |    y exacta     |  error abs.  |  error %%\n");
        H("  ----+----------------+------------------+-----------------+--------------+----------\n");
    } else H("    i |      x_i       |    y_i (aprox)\n  ----+----------------+------------------\n");
    double x = X0, y = Y0, emax = 0;
    for (int i = 0; i <= n; i++) {
        if (i > 0) { y = paso_rk(x, y, h, ks); x += h; }
        if (YEX[0]) { double ee = fabs(y - yex(x)); if (ee > emax) emax = ee; }
        if (!fila_visible(i, n)) continue;
        if (YEX[0]) {
            double ye = yex(x), ea = fabs(y - ye);
            H("  %3d | %14.8f | %16.10f | %15.10f | %12.3e | %8.4f\n",
              i, x, y, ye, ea, ye != 0 ? 100 * ea / fabs(ye) : 0.0);
        } else H("  %3d | %14.8f | %16.10f\n", i, x, y);
    }
    H("\nRESULTADO:  y(%.10g) ~ %.10f\n", x, y);
    if (YEX[0]) {
        H("   Valor exacto:          y(%.10g) = %.10f\n", x, yex(x));
        H("   Error absoluto exacto: %.3e\n", fabs(y - yex(x)));
        H("   Error maximo de toda la tabla: %.3e\n", emax);
    }
    hoja_que_escribir("El esquema de k1..k4 tal como lo da el enunciado",
                      "La primera iteracion con TODOS los k calculados",
                      "La combinacion final y el valor de y1",
                      "La tabla y el valor que piden, con 10 decimales",
                      YEX[0] ? "El error absoluto exacto" : "La comparacion con RK4 clasico",
                      (const char *)0);
    P("\nRESULTADO:  y(%.10g) ~ %.10f\n", x, y);
    if (YEX[0]) P("Exacta = %.10f   error = %.3e\n", yex(x), fabs(y - yex(x)));
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* EDO DE ORDEN SUPERIOR  /  SISTEMA DE DOS ECUACIONES                  */
/*   y'' = g(x, y, yp)   se baja a   y' = yp ;  yp' = g(x, y, yp)       */
/* ------------------------------------------------------------------ */
static char G1[256], G2[256], YPEX[256];
static double YP0;

static double ff1(double x, double y, double yp) { var_set("yp", yp); var_set("z", yp); return evalf2(G1, x, y); }
static double ff2(double x, double y, double yp) { var_set("yp", yp); var_set("z", yp); return evalf2(G2, x, y); }

static void op_sistema(void) {
    P("\n=== EDO DE ORDEN SUPERIOR  /  SISTEMA DE DOS ECUACIONES ===\n");
    P("\nUna EDO de SEGUNDO orden no se puede resolver directo con Euler o RK4:\n");
    P("primero hay que BAJARLA a un sistema de dos ecuaciones de PRIMER orden.\n");
    P("\n  1) Tengo una EDO de 2do orden:  y'' = g(x , y , y')\n");
    P("  2) Tengo un sistema de dos ecuaciones de 1er orden\n");
    int tipo = leer_int("Opcion (1-2): ");
    vars_reset();
    var_set("yp", 1); var_set("z", 1);
    if (tipo == 2) {
        P("\nEscribi las dos derivadas. Usa x, y, y z (o yp) para la segunda incognita.\n");
        leer_funcion("dy/dx = ", G1);
        leer_funcion("dz/dx = ", G2);
    } else {
        P("\nEscribi y'' despejada. Usa  yp  para y'.\n");
        P("Ejemplo:  y'' + 2y' + 5y = 0   se despeja como:   -2*yp - 5*y\n");
        leer_funcion("y'' = ", G2);
        strcpy(G1, "yp");
    }
    X0 = leer_num("x0 = ");
    Y0 = leer_num(tipo == 2 ? "y(x0) = " : "y(x0) = ");
    YP0 = leer_num(tipo == 2 ? "z(x0) = " : "y'(x0) = ");
    HP = leer_num("h  (paso) = ");
    if (HP == 0) return;
    double xf = leer_num("x final = ");
    int n = (int)floor(fabs((xf - X0) / HP) + 0.5);
    if (n < 1) n = 1;
    if (n > MAXPASOS) n = MAXPASOS;
    NPASOS = n;
    VER_MODO = 0;
    if (n > 25) {
        P("\nSon %d pasos. Como mostramos la tabla?\n  1) Todas\n  2) Cada cierto Dx\n  3) Ultimos N\n", n);
        int v = leer_int("Opcion (1-3): ");
        if (v == 2) { double dx = leer_num("Cada cuanto en x = "); VER_K = (int)floor(fabs(dx / HP) + 0.5); if (VER_K < 1) VER_K = 1; VER_MODO = 1; }
        else if (v == 3) { VER_N = leer_int("Cuantos puntos finales = "); if (VER_N < 1) VER_N = 1; VER_MODO = 2; }
    }
    YEX[0] = 0;
    leer_linea("Solucion EXACTA y(x), si la da el enunciado (ENTER = no): ", YEX, 256);
    if (YEX[0] && !expr_valida(YEX)) { printf("   No entendi: se ignora.\n"); YEX[0] = 0; }
    P("\n  1) Euler    2) RK4\n");
    int met = leer_int("Metodo (1-2): ");

    double h = HP;
    hoja_reset();
    if (tipo == 1) {
        H("EDO DE SEGUNDO ORDEN REDUCIDA A UN SISTEMA DE PRIMER ORDEN\n");
        H("\nEcuacion original:   y'' = %s\n", G2);
        H("Condiciones iniciales:  y(%.10g) = %.10g ,  y'(%.10g) = %.10g\n", X0, Y0, X0, YP0);
        H("\nCAMBIO DE VARIABLES (esto es lo que hay que justificar en la hoja):\n");
        H("   Se define una nueva incognita:   z = y'\n");
        H("   Entonces la EDO de 2do orden se convierte en un SISTEMA de dos de 1er orden:\n");
        H("      dy/dx = z\n");
        H("      dz/dx = %s        (con y' reemplazado por z)\n", G2);
        H("   Condiciones iniciales del sistema:  y(%.10g) = %.10g ,  z(%.10g) = %.10g\n", X0, Y0, X0, YP0);
        H("\nAhora SI se puede aplicar Euler o RK4, pero avanzando las DOS incognitas\n");
        H("a la vez en cada paso (y la pendiente de una depende de la otra).\n");
    } else {
        H("SISTEMA DE DOS EDO DE PRIMER ORDEN\n");
        H("   dy/dx = %s\n   dz/dx = %s\n", G1, G2);
        H("   y(%.10g) = %.10g ,  z(%.10g) = %.10g\n", X0, Y0, X0, YP0);
    }
    H("\nMETODO: %s     h = %.10g     %d pasos\n", met == 2 ? "RUNGE-KUTTA 4" : "EULER", h, n);
    if (met == 2) {
        H("\nRK4 para sistemas (los k vienen de a PARES, uno por incognita):\n");
        H("   k1y = f1(x , y , z)                    k1z = f2(x , y , z)\n");
        H("   k2y = f1(x+h/2 , y+k1y*h/2 , z+k1z*h/2)  k2z = f2(x+h/2 , y+k1y*h/2 , z+k1z*h/2)\n");
        H("   k3y = f1(x+h/2 , y+k2y*h/2 , z+k2z*h/2)  k3z = f2(...)\n");
        H("   k4y = f1(x+h , y+k3y*h , z+k3z*h)        k4z = f2(...)\n");
        H("   y(i+1) = y_i + h*(k1y + 2k2y + 2k3y + k4y)/6\n");
        H("   z(i+1) = z_i + h*(k1z + 2k2z + 2k3z + k4z)/6\n");
        H("IMPORTANTE: hay que calcular k1y y k1z ANTES de pasar a k2 (las dos incognitas\n");
        H("avanzan juntas; es el error mas comun hacerlo de a una).\n");
    } else {
        H("\nEuler para sistemas:\n");
        H("   y(i+1) = y_i + h*f1(x_i , y_i , z_i)\n");
        H("   z(i+1) = z_i + h*f2(x_i , y_i , z_i)\n");
    }
    {   /* primer paso detallado */
        double x = X0, y = Y0, z = YP0;
        H("\nPRIMERA ITERACION PASO A PASO:\n");
        double a1 = ff1(x, y, z), b1 = ff2(x, y, z);
        H("   f1(x0,y0,z0) = %.10f        f2(x0,y0,z0) = %.10f\n", a1, b1);
        if (met == 1) {
            H("   y1 = %.10g + (%.10g)*(%.10f) = %.10f\n", y, h, a1, y + h * a1);
            H("   z1 = %.10g + (%.10g)*(%.10f) = %.10f\n", z, h, b1, z + h * b1);
        } else {
            double a2 = ff1(x + h/2, y + a1*h/2, z + b1*h/2), b2 = ff2(x + h/2, y + a1*h/2, z + b1*h/2);
            double a3 = ff1(x + h/2, y + a2*h/2, z + b2*h/2), b3 = ff2(x + h/2, y + a2*h/2, z + b2*h/2);
            double a4 = ff1(x + h, y + a3*h, z + b3*h),       b4 = ff2(x + h, y + a3*h, z + b3*h);
            H("   k1y = %.10f   k1z = %.10f\n", a1, b1);
            H("   k2y = %.10f   k2z = %.10f\n", a2, b2);
            H("   k3y = %.10f   k3z = %.10f\n", a3, b3);
            H("   k4y = %.10f   k4z = %.10f\n", a4, b4);
            H("   y1 = %.10g + %.10g*(%.10f + 2*%.10f + 2*%.10f + %.10f)/6 = %.10f\n",
              y, h, a1, a2, a3, a4, y + h*(a1 + 2*a2 + 2*a3 + a4)/6);
            H("   z1 = %.10g + %.10g*(%.10f + 2*%.10f + 2*%.10f + %.10f)/6 = %.10f\n",
              z, h, b1, b2, b3, b4, z + h*(b1 + 2*b2 + 2*b3 + b4)/6);
        }
    }
    H("\nTABLA DE RESULTADOS:\n");
    H("    i |      x_i       |       y_i        |    z_i = y'_i   ");
    if (YEX[0]) H("|   y exacta     |  error abs.");
    H("\n  ----+----------------+------------------+-----------------");
    if (YEX[0]) H("+----------------+------------");
    H("\n");
    double x = X0, y = Y0, z = YP0, emax = 0;
    for (int i = 0; i <= n; i++) {
        if (i > 0) {
            double a1 = ff1(x, y, z), b1 = ff2(x, y, z);
            if (met == 1) { double yn = y + h*a1, zn = z + h*b1; y = yn; z = zn; }
            else {
                double a2 = ff1(x + h/2, y + a1*h/2, z + b1*h/2), b2 = ff2(x + h/2, y + a1*h/2, z + b1*h/2);
                double a3 = ff1(x + h/2, y + a2*h/2, z + b2*h/2), b3 = ff2(x + h/2, y + a2*h/2, z + b2*h/2);
                double a4 = ff1(x + h, y + a3*h, z + b3*h),       b4 = ff2(x + h, y + a3*h, z + b3*h);
                double yn = y + h*(a1 + 2*a2 + 2*a3 + a4)/6;
                double zn = z + h*(b1 + 2*b2 + 2*b3 + b4)/6;
                y = yn; z = zn;
            }
            x += h;
        }
        if (YEX[0]) { double ee = fabs(y - yex(x)); if (ee > emax) emax = ee; }
        if (!fila_visible(i, n)) continue;
        H("  %3d | %14.8f | %16.10f | %15.10f ", i, x, y, z);
        if (YEX[0]) H("| %14.10f | %10.3e", yex(x), fabs(y - yex(x)));
        H("\n");
    }
    H("\nRESULTADO:  y(%.10g) ~ %.10f      y'(%.10g) ~ %.10f\n", x, y, x, z);
    if (YEX[0]) {
        H("   Valor exacto:          y(%.10g) = %.10f\n", x, yex(x));
        H("   Error absoluto exacto: %.3e\n", fabs(y - yex(x)));
        H("   Error maximo de la tabla: %.3e\n", emax);
    }
    hoja_que_escribir(tipo == 1 ? "El cambio de variable z = y' y el sistema que queda" : "El sistema tal como lo da el enunciado",
                      "Las condiciones iniciales de las DOS incognitas",
                      "Las formulas del metodo aplicadas al sistema",
                      "La primera iteracion con los dos valores calculados a la vez",
                      "La tabla con x, y, y' (y el error si dan la exacta)",
                      (const char *)0);
    P("\nRESULTADO:  y(%.10g) ~ %.10f    y'(%.10g) ~ %.10f\n", x, y, x, z);
    if (YEX[0]) P("Exacta = %.10f   error = %.3e\n", yex(x), fabs(y - yex(x)));
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void op_evaluar(void) {
    double x = leer_num("x = "), y = leer_num("y = ");
    P("f(%.10g , %.10g) = %.10f\n", x, y, fxy(x, y));
    if (YEX[0]) P("y exacta en x = %.10g:  %.10f\n", x, yex(x));
}

/* ------------------------------------------------------------------ */
int main(void) {
    abrir_log("salida_edo.txt");
    P("\n####  ECUACIONES DIFERENCIALES ORDINARIAS  dy/dx = f(x,y)   (Guia 9)  ####\n");
    P("\nTodo lo que calcula queda tambien en el archivo salida_edo.txt\n");
    cargar();
    for (;;) {
        P("\n======================== MENU ========================\n");
        P("  dy/dx = %s     y(%.10g) = %.10g     h = %.10g     hasta x = %.10g\n",
          F, X0, Y0, HP, X0 + NPASOS * HP);
        P("\n  OPCION                          USALA SI EL ENUNCIADO DICE...\n");
        P("  1) Cargar otro problema\n");
        P("  2) Euler                        'metodo de Euler', 'Euler explicito/hacia adelante'\n");
        P("  3) Heun (Euler mejorado)        'Heun', 'Euler mejorado', 'predictor-corrector'\n");
        P("  4) Punto medio                  'punto medio', 'midpoint', 'Euler modificado'\n");
        P("  5) Ralston (RK2)                'Runge-Kutta de 2do orden', 'Ralston'\n");
        P("  6) Runge-Kutta 4                'RK4', 'Runge-Kutta de 4to orden', 'Runge-Kutta clasico'\n");
        P("  7) Comparar TODOS los metodos   'compare los metodos', 'cual es mas preciso'\n");
        P("  8) Efecto del paso h            'repita con h/2', 'como influye el paso', 'convergencia'\n");
        P("  9) METODO A MEDIDA              'se propone un metodo', 'implementelo', 'pseudocodigo'\n");
        P(" 10) RK a medida                 'otro esquema de Runge-Kutta', 'RK4 3/8', k1..k4 dados\n");
        P(" 11) Orden superior / SISTEMA    'y\" ', 'segundo orden', 'baje a primer orden', 'sistema'\n");
        P(" 12) Evaluar f(x,y) o y(x)        para controlar una cuenta a mano\n");
        P(" 13) SISTEMA de 2 EDO: EULER      dos ecuaciones de primer orden\n");
        P(" 14) SISTEMA de 3 EDO: EULER      tres ecuaciones de primer orden\n");
        P("  0) Salir\n");
        int op = leer_int("Opcion: ");
        if (op == 0) break;
        switch (op) {
            case 1: cargar(); break;
            case 2: correr(0); break;
            case 3: correr(1); break;
            case 4: correr(2); break;
            case 5: correr(3); break;
            case 6: correr(4); break;
            case 7: op_comparar(); break;
            case 8: op_efecto_h(); break;
            case 9: op_multipaso(); break;
            case 10: op_rk_medida(); break;
            case 11: op_sistema(); break;
            case 12: op_evaluar(); break;
            case 13: op_sistema_euler(2); break;
            case 14: op_sistema_euler(3); break;
            default: P("Opcion invalida.\n");
        }
        if (op >= 1 && op <= 14) pausa();
    }
    if (LOG_MN) fclose(LOG_MN);
    return 0;
}
