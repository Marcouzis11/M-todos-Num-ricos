/* splines.cpp -- INTERPOLACION SEGMENTARIA: CURVAS SPLINE   (Guia 6)
 *
 *  Trazadores (splines) lineal, cuadratico y cubico (natural y sujeto/condicionado).
 *  Muestra el sistema tridiagonal con sus numeros, la resolucion, el polinomio de
 *  CADA TRAMO (en forma de Chapra y en forma a + b(x-xi) + c(x-xi)^2 + d(x-xi)^3),
 *  la evaluacion en un punto, la derivada, la integral y la comparacion contra la
 *  interpolacion polinomial de grado alto.
 *
 *  Todo queda en salida_splines.txt
 */
#include "mn_comun.h"

static int NP = 0;
static double X[MAXPUNTOS], Y[MAXPUNTOS];
static char FEX[256] = "";
/* coeficientes del spline por tramo:  S_i(x) = A + B(x-xi) + C(x-xi)^2 + D(x-xi)^3 */
static double CA[MAXPUNTOS], CB[MAXPUNTOS], CC[MAXPUNTOS], CD[MAXPUNTOS];
static double M2[MAXPUNTOS];      /* segundas derivadas en los nodos (spline cubico) */
static int GRADO = 0;             /* 0 = no hay spline armado, 1 / 2 / 3 */
static int TIPO3 = 0;             /* 0 = natural , 1 = sujeto */

/* ------------------------------------------------------------------ */
static int leer_lista(const char *msg, double *v, int maxc) {
    char buf[2048];
    for (;;) {
        leer_linea(msg, buf, 2048);
        if (!buf[0]) return 0;
        int c = 0, ok = 1;
        char *t = strtok(buf, " \t;|");
        while (t) {
            if (c >= maxc) { ok = 0; break; }
            ep = t; ex = 0; eerr = 0;
            double val = e_expr();
            e_sp();
            if (eerr || *ep) { ok = 0; break; }
            v[c++] = val;
            t = strtok(NULL, " \t;|");
        }
        if (ok) return c;
        printf("   Escribi los numeros separados por ESPACIOS (ej: 3 4.5 7 9).\n");
    }
}

static void glosario(void) {
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   n+1 puntos (nodos)  ->  n TRAMOS (intervalos) entre nodo y nodo\n");
    P("   S_i(x) = el polinomio que vale en el tramo [x_i , x_(i+1)]\n");
    P("   h_i = x_(i+1) - x_i  = ancho del tramo i\n");
    P("   S''(x_i) = segunda derivada del spline en el nodo i (las incognitas del sistema)\n");
    P("   NATURAL = se impone S'' = 0 en los dos extremos (el spline 'se endereza' al salir)\n");
    P("   SUJETO (o condicionado) = se imponen las PENDIENTES S' en los dos extremos\n");
    P("\nIdea del spline: en vez de UN polinomio de grado alto por todos los puntos\n");
    P("(que oscila), se usa un polinomio de grado BAJO en cada tramo, pegados de\n");
    P("manera que la curva quede suave (continua, y con derivadas continuas).\n");
}

static void cargar(void) {
    P("\n=== CARGAR PUNTOS ===\n");
    glosario();
    do { NP = leer_lista("\nValores de x  (ORDENADOS de menor a mayor, separados por espacio): ", X, MAXPUNTOS); } while (NP < 2);
    for (int i = 0; i < NP - 1; i++)
        if (X[i + 1] <= X[i]) { P("\nOJO: los x tienen que estar ordenados y sin repetir (x%d >= x%d).\n", i, i + 1); }
    for (;;) {
        int c = leer_lista("Valores de y = f(x)  (en el mismo orden; ENTER = calcularlos con una f(x)): ", Y, MAXPUNTOS);
        if (c == 0) { leer_funcion("f(x) = ", FEX); for (int i = 0; i < NP; i++) Y[i] = evalf(FEX, X[i]); break; }
        if (c == NP) {
            FEX[0] = 0;
            leer_linea("Funcion EXACTA de donde salen los datos, si el enunciado la da (ENTER = no): ", FEX, 256);
            if (FEX[0] && !expr_valida(FEX)) { printf("   No entendi esa funcion: se ignora.\n"); FEX[0] = 0; }
            break;
        }
        printf("   Cargaste %d valores de x pero %d de y.\n", NP, c);
    }
    GRADO = 0;
    P("\n%d puntos  ->  %d tramos.  (Un polinomio interpolante unico seria de grado %d.)\n", NP, NP - 1, NP - 1);
}

/* ------------------------------------------------------------------ */
/* Tabla de datos y h_i                                                */
/* ------------------------------------------------------------------ */
static void tabla_datos(void) {
    H("Datos (%d puntos  ->  %d tramos):\n", NP, NP - 1);
    H("    i |       x_i      |       y_i      |   h_i = x_(i+1) - x_i\n");
    for (int i = 0; i < NP; i++) {
        H("  %3d | %14.8f | %14.8f |", i, X[i], Y[i]);
        if (i < NP - 1) H(" %14.8f\n", X[i + 1] - X[i]); else H("      ---\n");
    }
}

/* Escribe el polinomio de cada tramo en forma desarrollada por potencias de (x - xi) */
static void mostrar_tramos(void) {
    H("\nPOLINOMIO DE CADA TRAMO   (forma  S_i(x) = a + b*(x - x_i) + c*(x - x_i)^2 + d*(x - x_i)^3):\n");
    for (int i = 0; i < NP - 1; i++) {
        H("\n  Tramo %d:  %.10g <= x <= %.10g\n", i + 1, X[i], X[i + 1]);
        H("    S%d(x) = %.10f", i + 1, CA[i]);
        if (GRADO >= 1) H(" + (%.10f)*(x - %.10g)", CB[i], X[i]);
        if (GRADO >= 2) H(" + (%.10f)*(x - %.10g)^2", CC[i], X[i]);
        if (GRADO >= 3) H("\n            + (%.10f)*(x - %.10g)^3", CD[i], X[i]);
        H("\n");
    }
}

/* Control: el spline tiene que pasar por los puntos y pegar bien */
static double spl_eval(double x, int *tramo) {
    int i = 0;
    while (i < NP - 2 && x > X[i + 1]) i++;
    if (tramo) *tramo = i;
    double t = x - X[i];
    return CA[i] + CB[i] * t + CC[i] * t * t + CD[i] * t * t * t;
}
static double spl_deriv(double x) {
    int i = 0;
    while (i < NP - 2 && x > X[i + 1]) i++;
    double t = x - X[i];
    return CB[i] + 2 * CC[i] * t + 3 * CD[i] * t * t;
}
static double spl_deriv2(double x) {
    int i = 0;
    while (i < NP - 2 && x > X[i + 1]) i++;
    double t = x - X[i];
    return 2 * CC[i] + 6 * CD[i] * t;
}

static void control_nodos(void) {
    H("\nCONTROL (el spline tiene que pasar EXACTAMENTE por los datos):\n");
    double mx = 0;
    for (int i = 0; i < NP; i++) {
        double v = spl_eval(X[i], (int *)0), d = fabs(v - Y[i]);
        if (d > mx) mx = d;
        H("   S(%.10g) = %.10f     y_i = %.10f     diferencia = %.1e\n", X[i], v, Y[i], d);
    }
    H("   Maxima diferencia: %.2e  ->  %s\n", mx, mx < 1e-8 ? "PASA por todos los puntos (correcto)" : "REVISAR");
    if (GRADO >= 2 && NP > 2) {
        H("\nCONTINUIDAD en los nodos interiores (es lo que hace al spline 'suave'):\n");
        for (int i = 1; i < NP - 1; i++) {
            double e = 1e-7 * (fabs(X[i]) + 1);
            H("   x = %.10g :  S' izquierda = %.8f   S' derecha = %.8f", X[i], spl_deriv(X[i] - e), spl_deriv(X[i] + e));
            if (GRADO == 3) H("   |   S'' izq = %.6f   S'' der = %.6f", spl_deriv2(X[i] - e), spl_deriv2(X[i] + e));
            H("\n");
        }
        if (GRADO == 3) H("   (coinciden: el spline cubico tiene S, S' y S'' continuas)\n");
        else H("   (coinciden: el spline cuadratico tiene S y S' continuas)\n");
    }
}

static void bloque_error_exacto(void) {
    if (!FEX[0]) return;
    H("\nERROR CONTRA LA FUNCION EXACTA  f(x) = %s:\n", FEX);
    H("   (se mira en el MEDIO de cada tramo, que es donde el spline mas se aparta)\n");
    double mx = 0;
    for (int i = 0; i < NP - 1; i++) {
        double xm = 0.5 * (X[i] + X[i + 1]);
        double s = spl_eval(xm, (int *)0), fe = evalf(FEX, xm), d = fabs(s - fe);
        if (d > mx) mx = d;
        H("   x = %10.6f :  S(x) = %14.10f   f(x) = %14.10f   |error| = %.3e\n", xm, s, fe, d);
    }
    H("   Error maximo en los puntos medios: %.3e\n", mx);
}

/* ------------------------------------------------------------------ */
/* SPLINE LINEAL                                                       */
/* ------------------------------------------------------------------ */
static void op_lineal(void) {
    P("\n=== SPLINE (TRAZADOR) LINEAL ===\n");
    P("\nSe une cada par de puntos consecutivos con una RECTA.\n");
    P("Es continuo, pero la derivada SALTA en los nodos: la curva tiene 'picos'.\n");
    hoja_reset();
    H("SPLINE LINEAL (trazador de primer grado)\n\n");
    tabla_datos();
    H("\nEn cada tramo se usa la recta que pasa por los dos extremos:\n");
    H("   S_i(x) = y_i + m_i * (x - x_i)      con  m_i = (y_(i+1) - y_i) / (x_(i+1) - x_i)\n");
    H("\nPendientes de cada tramo:\n");
    for (int i = 0; i < NP - 1; i++) {
        double h = X[i + 1] - X[i], m = (Y[i + 1] - Y[i]) / h;
        CA[i] = Y[i]; CB[i] = m; CC[i] = 0; CD[i] = 0;
        H("   m%d = (%.10f - %.10f)/(%.10g - %.10g) = %.10f\n", i + 1, Y[i + 1], Y[i], X[i + 1], X[i], m);
    }
    GRADO = 1;
    mostrar_tramos();
    control_nodos();
    bloque_error_exacto();
    H("\nOBSERVACION: la derivada NO es continua. Por ejemplo, en los nodos interiores\n");
    H("la pendiente cambia de golpe de un tramo al otro. Por eso se usan los splines\n");
    H("de grado mayor cuando se necesita suavidad.\n");
    hoja_que_escribir("La tabla de datos con los h_i",
                      "La pendiente m_i de cada tramo con su cuenta",
                      "El polinomio (recta) de cada tramo con su intervalo de validez",
                      "Que es continuo pero la derivada salta en los nodos",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* SPLINE CUADRATICO                                                   */
/* ------------------------------------------------------------------ */
static void op_cuadratico(void) {
    P("\n=== SPLINE (TRAZADOR) CUADRATICO ===\n");
    P("\nEn cada tramo:  S_i(x) = a_i + b_i(x - x_i) + c_i(x - x_i)^2\n");
    P("Son 3n incognitas. Las condiciones son:\n");
    P("   - pasa por los puntos (2n condiciones)\n");
    P("   - la derivada S' es continua en los n-1 nodos interiores\n");
    P("Faltando una condicion, se agrega la mas usada:  S''= 0 en el primer tramo\n");
    P("(es decir c_1 = 0: el primer tramo es una RECTA).\n");
    hoja_reset();
    H("SPLINE CUADRATICO (trazador de segundo grado)\n\n");
    tabla_datos();
    H("\nEn cada tramo:  S_i(x) = a_i + b_i*(x - x_i) + c_i*(x - x_i)^2\n");
    H("Condiciones:\n");
    H("   1) S_i(x_i) = y_i            ->   a_i = y_i\n");
    H("   2) S_i(x_(i+1)) = y_(i+1)    ->   a_i + b_i*h_i + c_i*h_i^2 = y_(i+1)\n");
    H("   3) S'_i(x_(i+1)) = S'_(i+1)(x_(i+1))  ->  b_i + 2*c_i*h_i = b_(i+1)\n");
    H("   4) Condicion extra:  c_1 = 0  (el primer tramo es una recta)\n");
    H("\nDe (4) y (2) sale b_1, y despues se avanza tramo por tramo:\n");
    H("   c_i = ( y_(i+1) - y_i - b_i*h_i ) / h_i^2      y      b_(i+1) = b_i + 2*c_i*h_i\n\n");
    double h0 = X[1] - X[0];
    CA[0] = Y[0]; CC[0] = 0; CB[0] = (Y[1] - Y[0]) / h0; CD[0] = 0;
    H("   Tramo 1:  c1 = 0   ->   b1 = (y1 - y0)/h0 = (%.10f - %.10f)/%.10g = %.10f\n",
      Y[1], Y[0], h0, CB[0]);
    for (int i = 1; i < NP - 1; i++) {
        double hi = X[i + 1] - X[i], hp = X[i] - X[i - 1];
        CB[i] = CB[i - 1] + 2 * CC[i - 1] * hp;
        CA[i] = Y[i]; CD[i] = 0;
        CC[i] = (Y[i + 1] - Y[i] - CB[i] * hi) / (hi * hi);
        H("   Tramo %d:  b%d = b%d + 2*c%d*h%d = %.10f + 2*(%.10f)*(%.10g) = %.10f\n",
          i + 1, i + 1, i, i, i, CB[i - 1], CC[i - 1], hp, CB[i]);
        H("             c%d = (y%d - y%d - b%d*h%d)/h%d^2 = (%.10f - %.10f - (%.10f)*(%.10g))/%.10g = %.10f\n",
          i + 1, i + 1, i, i + 1, i + 1, i + 1, Y[i + 1], Y[i], CB[i], hi, hi * hi, CC[i]);
    }
    GRADO = 2;
    mostrar_tramos();
    control_nodos();
    bloque_error_exacto();
    hoja_que_escribir("Las 4 familias de condiciones y por que hacen falta",
                      "La condicion extra que se eligio (c1 = 0, primer tramo recta)",
                      "El calculo de b_i y c_i tramo por tramo",
                      "El polinomio de cada tramo con su intervalo",
                      "El control: pasa por los puntos y S' es continua",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* SPLINE CUBICO (natural o sujeto)                                    */
/* ------------------------------------------------------------------ */
static void op_cubico(void) {
    double sub[MAXPUNTOS], dia[MAXPUNTOS], sup[MAXPUNTOS], rhs[MAXPUNTOS];
    double fp0 = 0, fpn = 0;
    int n = NP - 1;                        /* cantidad de tramos */
    P("\n=== SPLINE (TRAZADOR) CUBICO ===\n");
    P("\nEn cada tramo S_i es una CUBICA. Se pide que S, S' y S'' sean continuas:\n");
    P("la curva queda suave de verdad. Las incognitas comodas son las segundas\n");
    P("derivadas en los nodos, S''(x_i), que salen de un sistema TRIDIAGONAL.\n");
    P("\nCondiciones de borde:\n");
    P("  1) NATURAL  (los enunciados la llaman 'spline cubica NORMAL'):\n");
    P("     se impone  S''(x0) = S''(xn) = 0.  Es la que piden casi siempre.\n");
    P("  2) SUJETO (condicionado): se dan las pendientes S'(x0) y S'(xn)\n");
    TIPO3 = leer_int("Opcion (1-2): ") == 2 ? 1 : 0;
    if (TIPO3) {
        fp0 = leer_num("S'(x0)  (pendiente en el primer punto) = ");
        fpn = leer_num("S'(xn)  (pendiente en el ultimo punto) = ");
    }
    hoja_reset();
    H("SPLINE CUBICO %s\n\n", TIPO3 ? "SUJETO (condicionado)" : "NATURAL (= 'normal' en los enunciados)");
    tabla_datos();
    H("\nSe plantea el sistema para las segundas derivadas S''(x_i) (incognitas).\n");
    H("Para cada nodo INTERIOR i = 1 .. %d:\n", n - 1);
    H("   h_(i-1)*S''(x_(i-1)) + 2*(h_(i-1) + h_i)*S''(x_i) + h_i*S''(x_(i+1)) =\n");
    H("        6*[ (y_(i+1) - y_i)/h_i  -  (y_i - y_(i-1))/h_(i-1) ]\n");
    if (TIPO3) {
        H("\nBorde SUJETO: se agregan dos ecuaciones con las pendientes dadas:\n");
        H("   2*h_0*S''(x0) + h_0*S''(x1) = 6*[ (y1 - y0)/h_0 - S'(x0) ]\n");
        H("   h_(n-1)*S''(x_(n-1)) + 2*h_(n-1)*S''(xn) = 6*[ S'(xn) - (yn - y_(n-1))/h_(n-1) ]\n");
        H("   con S'(x0) = %.10g  y  S'(xn) = %.10g\n", fp0, fpn);
    } else {
        H("\nBorde NATURAL:  S''(x0) = 0  y  S''(xn) = 0  (se sacan del sistema).\n");
    }
    /* armado del sistema en las incognitas M2[0..n] */
    for (int i = 0; i <= n; i++) { sub[i] = dia[i] = sup[i] = rhs[i] = 0; }
    if (TIPO3) {
        double h0 = X[1] - X[0], hn = X[n] - X[n - 1];
        dia[0] = 2 * h0; sup[0] = h0; rhs[0] = 6 * ((Y[1] - Y[0]) / h0 - fp0);
        dia[n] = 2 * hn; sub[n] = hn; rhs[n] = 6 * (fpn - (Y[n] - Y[n - 1]) / hn);
    } else {
        dia[0] = 1; rhs[0] = 0;
        dia[n] = 1; rhs[n] = 0;
    }
    for (int i = 1; i < n; i++) {
        double hm = X[i] - X[i - 1], hi = X[i + 1] - X[i];
        sub[i] = hm; dia[i] = 2 * (hm + hi); sup[i] = hi;
        rhs[i] = 6 * ((Y[i + 1] - Y[i]) / hi - (Y[i] - Y[i - 1]) / hm);
    }
    H("\nCon los datos, las ecuaciones de los nodos interiores quedan:\n");
    for (int i = 1; i < n; i++) {
        double hm = X[i] - X[i - 1], hi = X[i + 1] - X[i];
        H("   i=%d:  %.10g*S''%d + 2*(%.10g + %.10g)*S''%d + %.10g*S''%d = 6*[ (%.6f - %.6f)/%.6g - (%.6f - %.6f)/%.6g ]\n",
          i, hm, i - 1, hm, hi, i, hi, i + 1, Y[i + 1], Y[i], hi, Y[i], Y[i - 1], hm);
        H("         %.10g*S''%d + %.10g*S''%d + %.10g*S''%d = %.10f\n",
          sub[i], i - 1, dia[i], i, sup[i], i + 1, rhs[i]);
    }
    H("\nMatriz TRIDIAGONAL del sistema  (incognitas S''0 .. S''%d):\n", n);
    for (int i = 0; i <= n; i++) {
        H("   [ ");
        for (int j = 0; j <= n; j++) {
            double v = (j == i) ? dia[i] : (j == i - 1 ? sub[i] : (j == i + 1 ? sup[i] : 0));
            H("%9.4f ", v);
        }
        H("| %11.6f ]\n", rhs[i]);
    }
    H("   (es tridiagonal: conviene resolverlo con el algoritmo de Thomas, O(n))\n");
    /* Thomas */
    for (int i = 1; i <= n; i++) {
        double m = sub[i] / dia[i - 1];
        dia[i] -= m * sup[i - 1];
        rhs[i] -= m * rhs[i - 1];
    }
    M2[n] = rhs[n] / dia[n];
    for (int i = n - 1; i >= 0; i--) M2[i] = (rhs[i] - sup[i] * M2[i + 1]) / dia[i];
    H("\nSOLUCION del sistema (segundas derivadas en los nodos):\n");
    for (int i = 0; i <= n; i++) H("   S''(x%d) = S''(%.10g) = %.10f\n", i, X[i], M2[i]);
    /* coeficientes por tramo */
    for (int i = 0; i < n; i++) {
        double h = X[i + 1] - X[i];
        CA[i] = Y[i];
        CB[i] = (Y[i + 1] - Y[i]) / h - h * (2 * M2[i] + M2[i + 1]) / 6;
        CC[i] = M2[i] / 2;
        CD[i] = (M2[i + 1] - M2[i]) / (6 * h);
    }
    GRADO = 3;
    H("\nDe las segundas derivadas se obtienen los coeficientes de cada tramo:\n");
    H("   a_i = y_i                                        (el valor en el nodo)\n");
    H("   b_i = (y_(i+1) - y_i)/h_i - h_i*(2*S''_i + S''_(i+1))/6     (la pendiente)\n");
    H("   c_i = S''_i / 2\n");
    H("   d_i = (S''_(i+1) - S''_i) / (6*h_i)\n");
    H("\n    i |      a_i      |      b_i      |      c_i      |      d_i\n");
    for (int i = 0; i < n; i++)
        H("  %3d | %13.8f | %13.8f | %13.8f | %13.8f\n", i + 1, CA[i], CB[i], CC[i], CD[i]);
    mostrar_tramos();
    control_nodos();
    bloque_error_exacto();
    if (!TIPO3) H("\nVerificacion del borde NATURAL: S''(x0) = %.2e y S''(xn) = %.2e (ambos cero).\n", M2[0], M2[n]);
    else H("\nVerificacion del borde SUJETO: S'(x0) = %.8f (pedido %.8f) , S'(xn) = %.8f (pedido %.8f).\n",
           spl_deriv(X[0]), fp0, spl_deriv(X[n]), fpn);
    hoja_que_escribir("La tabla de datos con los h_i",
                      "La ecuacion generica de los nodos interiores",
                      "Las condiciones de borde usadas (natural o sujeto) y por que",
                      "El sistema tridiagonal escrito con numeros",
                      "La solucion S''(x_i) de cada nodo",
                      "Las formulas de a, b, c, d y la tabla de coeficientes",
                      "El polinomio de cada tramo con su intervalo de validez",
                      "El control: pasa por los puntos, S' y S'' continuas, borde verificado",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void op_evaluar(void) {
    if (!GRADO) { P("\nPrimero arma un spline (opciones 2, 3 o 4).\n"); return; }
    double xq = leer_num("x donde evaluar  (el valor que pide el enunciado) = ");
    if (xq < X[0] || xq > X[NP - 1])
        P("\nAVISO: x = %.10g esta FUERA del rango [%.10g , %.10g]: es extrapolacion.\n", xq, X[0], X[NP - 1]);
    int tr;
    double v = spl_eval(xq, &tr);
    hoja_reset();
    H("EVALUACION DEL SPLINE %s EN x = %.10g\n",
      GRADO == 1 ? "LINEAL" : (GRADO == 2 ? "CUADRATICO" : (TIPO3 ? "CUBICO SUJETO" : "CUBICO NATURAL")), xq);
    H("\n1) SE BUSCA EN QUE TRAMO CAE x:\n");
    H("   %.10g <= %.10g <= %.10g   ->   corresponde al TRAMO %d\n", X[tr], xq, X[tr + 1], tr + 1);
    H("   (esto es lo primero que hay que justificar: cada tramo tiene su propio polinomio)\n");
    H("\n2) SE USA EL POLINOMIO DE ESE TRAMO:\n");
    H("   S%d(x) = %.10f", tr + 1, CA[tr]);
    if (GRADO >= 1) H(" + (%.10f)*(x - %.10g)", CB[tr], X[tr]);
    if (GRADO >= 2) H(" + (%.10f)*(x - %.10g)^2", CC[tr], X[tr]);
    if (GRADO >= 3) H(" + (%.10f)*(x - %.10g)^3", CD[tr], X[tr]);
    H("\n\n3) SE REEMPLAZA:\n");
    double t = xq - X[tr];
    H("   x - x%d = %.10g - %.10g = %.10f\n", tr, xq, X[tr], t);
    H("   S%d(%.10g) = %.10f", tr + 1, xq, CA[tr]);
    if (GRADO >= 1) H(" + (%.10f)*(%.10f)", CB[tr], t);
    if (GRADO >= 2) H(" + (%.10f)*(%.10f)^2", CC[tr], t);
    if (GRADO >= 3) H(" + (%.10f)*(%.10f)^3", CD[tr], t);
    H("\n            = %.10f\n", v);
    H("\nRESULTADO:  S(%.10g) = %.10f\n", xq, v);
    if (GRADO >= 2) {
        H("\nDerivadas en ese punto (si el enunciado las pide):\n");
        H("   S'(%.10g)  = %.10f      (pendiente / velocidad)\n", xq, spl_deriv(xq));
        if (GRADO == 3) H("   S''(%.10g) = %.10f      (curvatura / aceleracion)\n", xq, spl_deriv2(xq));
    }
    if (FEX[0]) {
        double fe = evalf(FEX, xq), ea = fabs(v - fe);
        H("\nERROR CONTRA LA FUNCION EXACTA:\n");
        H("   f(%.10g) = %.10f        (de f(x) = %s)\n", xq, fe, FEX);
        H("   Error absoluto exacto = |%.10f - %.10f| = %.3e\n", v, fe, ea);
        if (fe != 0) H("   Error relativo porcentual = %.6f %%\n", 100 * ea / fabs(fe));
    }
    hoja_que_escribir("En que tramo cae el x y por que",
                      "El polinomio de ESE tramo",
                      "El reemplazo con numeros y el resultado",
                      FEX[0] ? "El error contra el valor exacto" : "Las derivadas si las piden",
                      (const char *)0);
    P("\nS(%.10g) = %.10f   (tramo %d)\n", xq, v, tr + 1);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* TABULAR EL SPLINE con paso constante: sirve para armar la tabla      */
/* equiespaciada que despues se deriva o se integra.                    */
/* ------------------------------------------------------------------ */
static void op_tabular(void) {
    if (!GRADO) { P("\nPrimero arma un spline (opciones 2, 3 o 4).\n"); return; }
    P("\n=== GENERAR TABLA EQUIESPACIADA CON EL SPLINE ===\n");
    P("\nEs el paso tipico de los parciales: los datos vienen con x DESIGUALMENTE\n");
    P("espaciados, y para poder usar Simpson o diferencias finitas hace falta una\n");
    P("tabla con paso CONSTANTE. Se genera evaluando el spline.\n");
    P("\n  1) Dar el paso h       ('genere una tabla con h = 0.1')\n");
    P("  2) Dar la cantidad n   ('divida el intervalo en 10 sub-intervalos')\n");
    int op = leer_int("Opcion (1-2): ");
    double a = X[0], b = X[NP - 1], h;
    int n;
    if (op == 2) { n = leer_int("n  (cantidad de subintervalos) = "); if (n < 1) return; h = (b - a) / n; }
    else { h = leer_num("h  (paso, ej: 0.1) = "); if (h <= 0) return; n = (int)floor((b - a) / h + 0.5); }
    if (n < 1 || n > MAXPUNTOS - 1) { P("Esa cantidad de puntos no entra (maximo %d).\n", MAXPUNTOS); return; }
    hoja_reset();
    H("TABLA EQUIESPACIADA GENERADA CON EL SPLINE %s\n",
      GRADO == 1 ? "LINEAL" : (GRADO == 2 ? "CUADRATICO" : (TIPO3 ? "CUBICO SUJETO" : "CUBICO NATURAL")));
    H("Intervalo [%.10g , %.10g]   n = %d subintervalos   h = %.10g\n", a, b, n, h);
    H("\nCada valor sale de evaluar el polinomio del tramo donde cae ese x.\n");
    H("\n    i |      x_i       |     S(x_i)      ");
    if (FEX[0]) H("|   f(x_i) exacta |  error abs.");
    H("\n  ----+----------------+-----------------");
    if (FEX[0]) H("+-----------------+------------");
    H("\n");
    double emax = 0;
    for (int i = 0; i <= n; i++) {
        double x = a + i * h, v = spl_eval(x, (int *)0);
        H("  %3d | %14.8f | %15.10f ", i, x, v);
        if (FEX[0]) {
            double fe = evalf(FEX, x), ea = fabs(v - fe);
            if (ea > emax) emax = ea;
            H("| %15.10f | %10.3e", fe, ea);
        }
        H("\n");
    }
    if (FEX[0]) H("\n  Error maximo del spline en la tabla: %.3e\n", emax);
    H("\n--- PARA PEGAR EN OTRO PROGRAMA (integracion.exe o derivacion.exe) ---\n");
    H("Valores de x:\n");
    for (int i = 0; i <= n; i++) H("%.10g%s", a + i * h, i == n ? "\n" : " ");
    H("Valores de y:\n");
    for (int i = 0; i <= n; i++) H("%.10f%s", spl_eval(a + i * h, (int *)0), i == n ? "\n" : " ");
    H("\n(Copia esas dos lineas: en integracion.exe elegi 'tengo una TABLA' y pegalas.)\n");
    hoja_que_escribir("Que la tabla se genero evaluando el spline en cada x equiespaciado",
                      "El valor de h usado",
                      "La tabla completa de x_i y S(x_i)",
                      FEX[0] ? "Los valores exactos y el error del spline en cada punto" : "En que tramo cae cada x si lo piden",
                      (const char *)0);
    P("\nTabla de %d puntos generada (esta en el texto para la hoja, lista para copiar).\n", n + 1);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void op_integrar(void) {
    if (!GRADO) { P("\nPrimero arma un spline (opciones 2, 3 o 4).\n"); return; }
    hoja_reset();
    H("INTEGRAL DEL SPLINE entre %.10g y %.10g\n", X[0], X[NP - 1]);
    H("\nSe integra tramo por tramo y se suma. En cada tramo, con t = x - x_i y h_i:\n");
    H("   integral de S_i = a_i*h_i + b_i*h_i^2/2 + c_i*h_i^3/3 + d_i*h_i^4/4\n\n");
    double tot = 0;
    H("    i |   desde    |   hasta    |      integral del tramo\n");
    for (int i = 0; i < NP - 1; i++) {
        double h = X[i + 1] - X[i];
        double v = CA[i] * h + CB[i] * h * h / 2 + CC[i] * pow(h, 3) / 3 + CD[i] * pow(h, 4) / 4;
        tot += v;
        H("  %3d | %10.6g | %10.6g | %20.10f\n", i + 1, X[i], X[i + 1], v);
    }
    H("\nINTEGRAL TOTAL = %.10f\n", tot);
    if (FEX[0]) {
        /* control con Simpson fino sobre la funcion exacta */
        int m = 2000; double hh = (X[NP - 1] - X[0]) / m, s = evalf(FEX, X[0]) + evalf(FEX, X[NP - 1]);
        for (int i = 1; i < m; i++) s += (i % 2 ? 4 : 2) * evalf(FEX, X[0] + i * hh);
        double ie = hh / 3 * s;
        H("\nControl con la funcion exacta f(x) = %s:\n", FEX);
        H("   integral exacta (Simpson con 2000 subintervalos) = %.10f\n", ie);
        H("   Error absoluto = %.3e\n", fabs(tot - ie));
    }
    hoja_que_escribir("La formula de la integral de cada tramo",
                      "La tabla con la integral de cada tramo",
                      "La suma total",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void op_comparar(void) {
    if (!GRADO) { P("\nPrimero arma un spline (opciones 2, 3 o 4).\n"); return; }
    P("\n=== SPLINE CONTRA POLINOMIO INTERPOLANTE DE GRADO ALTO ===\n");
    /* polinomio de Lagrange evaluado directamente */
    hoja_reset();
    H("COMPARACION: SPLINE  contra  POLINOMIO INTERPOLANTE UNICO de grado %d\n", NP - 1);
    H("\nCon %d puntos, el polinomio interpolante unico es de grado %d. Entre los nodos\n", NP, NP - 1);
    H("puede OSCILAR mucho (fenomeno de Runge), sobre todo cerca de los extremos.\n");
    H("El spline, al usar polinomios de grado bajo por tramo, no tiene ese problema.\n\n");
    H("        x      |    SPLINE     |  POLINOMIO gr.%d", NP - 1);
    if (FEX[0]) H("  |   f exacta    | err spline | err polin.");
    H("\n");
    int m = 4 * (NP - 1);
    for (int k = 0; k <= m; k++) {
        double x = X[0] + (X[NP - 1] - X[0]) * k / m;
        double s = spl_eval(x, (int *)0), pl = 0;
        for (int i = 0; i < NP; i++) {
            double L = 1;
            for (int j = 0; j < NP; j++) if (j != i) L *= (x - X[j]) / (X[i] - X[j]);
            pl += Y[i] * L;
        }
        H("  %13.8f | %13.8f | %15.8f", x, s, pl);
        if (FEX[0]) {
            double fe = evalf(FEX, x);
            H(" | %13.8f | %10.3e | %10.3e", fe, fabs(s - fe), fabs(pl - fe));
        }
        H("\n");
    }
    H("\nCONCLUSION: el spline se mantiene pegado a los datos en todo el rango, mientras\n");
    H("que el polinomio de grado alto se dispara entre los nodos. Por eso para muchos\n");
    H("puntos se prefiere la interpolacion SEGMENTARIA (spline) y no un unico polinomio.\n");
    hoja_que_escribir("La tabla comparativa spline / polinomio de grado alto",
                      "El nombre del problema del polinomio: oscilacion (fenomeno de Runge)",
                      "La conclusion de por que se usa el spline",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
int main(void) {
    abrir_log("salida_splines.txt");
    P("\n####  INTERPOLACION SEGMENTARIA: CURVAS SPLINE   (Guia 6)  ####\n");
    P("\nTodo lo que calcula queda tambien en el archivo salida_splines.txt\n");
    cargar();
    for (;;) {
        P("\n======================== MENU (%d puntos, %d tramos) ========================\n", NP, NP - 1);
        if (GRADO) P("  Spline armado: %s\n",
                     GRADO == 1 ? "LINEAL" : (GRADO == 2 ? "CUADRATICO" : (TIPO3 ? "CUBICO SUJETO" : "CUBICO NATURAL")));
        P("\n  OPCION                          USALA SI EL ENUNCIADO DICE...\n");
        P("  1) Cargar otros puntos\n");
        P("  2) Spline LINEAL                'trazador lineal', 'spline de primer grado'\n");
        P("  3) Spline CUADRATICO            'trazador cuadratico', 'spline de segundo grado'\n");
        P("  4) Spline CUBICO                'spline cubica NORMAL', 'natural', 'trazador cubico', 'sujeto'\n");
        P("  5) Evaluar en un x              'estime el valor en x = ...', 'calcule S(...)'\n");
        P("  6) TABLA equiespaciada          'tabla de datos equiespaciados', 'conservando el numero de puntos'\n");
        P("  7) Integral del spline          'integre usando el spline', 'area bajo la curva'\n");
        P("  8) Comparar con polinomio       'compare con el polinomio', 'oscilacion', 'Runge'\n");
        P("  0) Salir\n");
        int op = leer_int("Opcion: ");
        if (op == 0) break;
        switch (op) {
            case 1: cargar(); break;
            case 2: op_lineal(); break;
            case 3: op_cuadratico(); break;
            case 4: op_cubico(); break;
            case 5: op_evaluar(); break;
            case 6: op_tabular(); break;
            case 7: op_integrar(); break;
            case 8: op_comparar(); break;
            default: P("Opcion invalida.\n");
        }
        if (op >= 1 && op <= 8) pausa();
    }
    if (LOG_MN) fclose(LOG_MN);
    return 0;
}
