/* interpolacion.cpp -- INTERPOLACION POLINOMIAL   (Guia 5, primera parte)
 *
 *  Lagrange (evaluar y escribir el polinomio), Newton por diferencias divididas,
 *  interpolacion local de grado g (ej. "cubica" con los 4 puntos mas cercanos),
 *  error contra la funcion exacta y busqueda de x tal que P(x) = c.
 *  Todo queda guardado en salida_interpolacion.txt
 */
#include "mn_comun.h"

static int NP = 0;
static double X[MAXPUNTOS], Y[MAXPUNTOS];
static char FEX[256] = "";

/* Lee numeros separados por espacio. Devuelve la cantidad (0 si ENTER vacio). */
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
        printf("   Escribi los numeros separados por ESPACIOS (ej: 1 3 5 7 13).\n");
    }
}

static void cargar(void) {
    P("\n=== CARGAR PUNTOS ===\n");
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   x_i , y_i = los datos de la tabla (abscisas y ordenadas)\n");
    P("   n+1 puntos  ->  el polinomio interpolante tiene grado n como maximo\n");
    P("   P(x) = polinomio interpolante: pasa EXACTAMENTE por todos los puntos\n");
    P("   xq = valor de x donde se quiere estimar (el que pide el enunciado)\n");
    P("   L_k(x) = polinomios de Lagrange: valen 1 en su nodo y 0 en los demas\n");
    P("   f[x_i,...] = diferencias divididas (las restas y divisiones de la tabla de Newton)\n");
    do { NP = leer_lista("Valores de x  (las abscisas de la tabla, separadas por espacio): ", X, MAXPUNTOS); } while (NP < 2);
    for (;;) {
        int c = leer_lista("Valores de y  (las ordenadas, en el mismo orden; ENTER = calcularlos con una f(x)): ", Y, MAXPUNTOS);
        if (c == 0) {
            leer_funcion("f(x) =  (funcion exacta de donde salen los datos): ", FEX);
            for (int i = 0; i < NP; i++) Y[i] = evalf(FEX, X[i]);
            break;
        }
        if (c == NP) {
            FEX[0] = 0;
            leer_linea("Funcion EXACTA de donde salen los datos, si el enunciado la da (ej: ln(x^2+1) - sin(x); ENTER = no): ", FEX, 256);
            if (FEX[0] && !expr_valida(FEX)) { printf("   No entendi esa funcion: se ignora.\n"); FEX[0] = 0; }
            if (FEX[0]) P("Se usaran los y de la TABLA para interpolar y f(x) = %s para calcular el error exacto.\n", FEX);
            break;
        }
        printf("   Cargaste %d valores de x pero %d de y.\n", NP, c);
    }
    for (int i = 0; i < NP; i++)
        for (int j = i + 1; j < NP; j++)
            if (X[i] == X[j]) P("ATENCION: x%d = x%d = %g. No puede haber x repetidos.\n", i, j, X[i]);
    P("\n  i |        x_i        |        y_i\n");
    for (int i = 0; i < NP; i++) P("%3d | %17.10f | %17.10f\n", i, X[i], Y[i]);
    P("Con %d puntos el polinomio interpolante es de grado %d (como maximo).\n", NP, NP - 1);
}

static double lagrange(const double *xs, const double *ys, int n, double xq) {
    double s = 0;
    for (int k = 0; k < n; k++) {
        double L = 1;
        for (int j = 0; j < n; j++) if (j != k) L *= (xq - xs[j]) / (xs[k] - xs[j]);
        s += ys[k] * L;
    }
    return s;
}

/* Coeficientes en forma canonica: P(x) = c[0] + c[1] x + ... + c[n-1] x^(n-1) */
static void coef_canonicos(const double *xs, const double *ys, int n, double *c) {
    for (int i = 0; i < n; i++) c[i] = 0;
    for (int k = 0; k < n; k++) {
        double num[MAXPUNTOS + 1], den = 1;
        int g = 0;
        num[0] = 1;
        for (int j = 0; j < n; j++) {
            if (j == k) continue;
            num[g + 1] = 0;
            for (int p = g + 1; p >= 1; p--) num[p] = num[p - 1] - xs[j] * num[p];
            num[0] = -xs[j] * num[0];
            g++;
            den *= (xs[k] - xs[j]);
        }
        for (int p = 0; p < n; p++) c[p] += ys[k] / den * num[p];
    }
}

static double horner(const double *c, int n, double x) {
    double s = 0;
    for (int i = n - 1; i >= 0; i--) s = s * x + c[i];
    return s;
}

static void imprimir_canonico(const double *c, int n) {
    P("P(x) = %.10f", c[0]);
    for (int i = 1; i < n; i++) {
        P(" %s %.10f x", c[i] < 0 ? "-" : "+", fabs(c[i]));
        if (i > 1) P("^%d", i);
    }
    P("\n");
}

static void imprimir_forma_lagrange(const double *xs, const double *ys, int n) {
    P("Forma de Lagrange:  P(x) = suma_k  y_k * L_k(x),   L_k(x) = prod_{j!=k} (x - x_j)/(x_k - x_j)\n");
    for (int k = 0; k < n; k++) {
        double den = 1;
        P("   L%d(x) = ", k);
        for (int j = 0; j < n; j++) if (j != k) { P("(x - %g)", xs[j]); den *= xs[k] - xs[j]; }
        P(" / (%.10g)\n", den);
    }
    P("   P(x) = ");
    for (int k = 0; k < n; k++) P("%s%.10g*L%d(x)", k ? " + " : "", ys[k], k);
    P("\n");
}

static void comparar_exacta(double xq, double pq) {
    if (!FEX[0]) return;
    double fq = evalf(FEX, xq);
    P("f(x) exacta = %.10f   error = |P - f| = %.6e   error relativo = %.6e %%\n", fq, fabs(pq - fq), fq != 0 ? 100 * fabs(pq - fq) / fabs(fq) : 0.0);
}

/* ------------------------------------------------------------------ */
static void hoja_datos(void) {
    H("Datos (%d puntos  =>  el polinomio interpolante es de grado %d):\n", NP, NP - 1);
    for (int i = 0; i < NP; i++) H("   x%d = %-12.6g  y%d = %.6g\n", i, X[i], i, Y[i]);
    H("\n");
}

static void op_lagrange(void) {
    P("\n=== LAGRANGE: EVALUAR ===\n");
    double xq = leer_num("x donde evaluar  (el valor que pide el enunciado) = ");
    if (!FEX[0]) {
        leer_linea("Funcion EXACTA para calcular el error exacto (ENTER = no se conoce): ", FEX, 256);
        if (FEX[0] && !expr_valida(FEX)) { printf("   No entendi esa funcion: se ignora.\n"); FEX[0] = 0; }
    }
    hoja_reset();
    H("INTERPOLACION POLINOMIAL - METODO DE LAGRANGE\n\n");
    hoja_datos();
    H("Formula:  P(x) = suma_k  y_k * L_k(x)      con  L_k(x) = producto_(j != k) (x - x_j)/(x_k - x_j)\n\n");
    H("Evaluacion en x = %.10g:\n", xq);
    double s = 0;
    for (int k = 0; k < NP; k++) {
        double L = 1, num = 1, den = 1;
        for (int j = 0; j < NP; j++) if (j != k) { num *= (xq - X[j]); den *= (X[k] - X[j]); }
        L = num / den;
        P("   L%d(%g) = %.10f     y%d * L%d = %.10f * %.10f = %.10f\n", k, xq, L, k, k, Y[k], L, Y[k] * L);
        {
            char nf[500], df[500];
            int a1 = 0, b1 = 0;
            for (int j = 0; j < NP; j++) if (j != k) {
                a1 += sprintf(nf + a1, "(%.6g - %.6g)", xq, X[j]);
                b1 += sprintf(df + b1, "(%.6g - %.6g)", X[k], X[j]);
            }
            H("   L%d(%.10g) = %s / %s\n", k, xq, nf, df);
            H("            = (%.8g) / (%.8g) = %.10f        y%d * L%d = %.6g * %.10f = %.10f\n", num, den, L, k, k, Y[k], L, Y[k] * L);
        }
        s += Y[k] * L;
    }
    linea();
    P("P(%.10g) = %.10f   (grado %d, con los %d puntos)\n", xq, s, NP - 1, NP);
    H("\n   (control: la suma de todos los L_k debe dar 1)\n");
    H("\nRESULTADO:  P(%.10g) = %.10f\n", xq, s);
    H("\nERRORES:\n");
    if (FEX[0]) {
        double fq = evalf(FEX, xq);
        H("   Valor EXACTO f(%.10g) = %.10f     (se conoce la funcion de donde salen los datos)\n", xq, fq);
        H("   1) Error absoluto EXACTO = |P(x) - f(x)| = |%.10f - %.10f| = %.6e\n", s, fq, fabs(s - fq));
        if (fq != 0) H("   2) Error relativo EXACTO = 100*|P - f| / |f| = %.4f %%\n", 100 * fabs(s - fq) / fabs(fq));
        H("   Es un error EXACTO (o verdadero) porque se compara contra el valor real,\n");
        H("   no contra otra aproximacion.\n");
    } else {
        H("   No se puede calcular el error EXACTO: no se conoce la funcion f(x) de donde\n");
        H("   salen los datos, solo la tabla. El error de interpolacion se puede ESTIMAR\n");
        H("   agregando un punto mas y viendo cuanto cambia P(x), o con la formula\n");
        H("   error = f^(n+1)(z)/(n+1)! * (x - x0)(x - x1)...(x - xn).\n");
        H("   En los nodos el error es cero; crece entre nodos y mucho mas fuera del rango.\n");
    }
    comparar_exacta(xq, s);
    linea();
    {
        char g1[160];
        sprintf(g1, "Con %d puntos el polinomio es de grado %d", NP, NP - 1);
        hoja_que_escribir(g1, "La formula de Lagrange", "Al menos un L_k desarrollado con numeros (el programa los muestra todos)", "El valor P(x) pedido", "Si se conoce la funcion: el error absoluto exacto |P - f|", (const char *)0);
    }
    hoja_mostrar();
}

static void op_polinomio(void) {
    double c[MAXPUNTOS];
    P("\n=== POLINOMIO INTERPOLANTE (grado %d) ===\n", NP - 1);
    imprimir_forma_lagrange(X, Y, NP);
    coef_canonicos(X, Y, NP, c);
    P("\nForma canonica (desarrollado):\n");
    imprimir_canonico(c, NP);
    P("Chequeo P(x_i) = y_i:\n");
    for (int i = 0; i < NP; i++) P("   P(%g) = %.10f   y = %.10f\n", X[i], horner(c, NP, X[i]), Y[i]);

    hoja_reset();
    H("POLINOMIO INTERPOLANTE DE GRADO %d (metodo de Lagrange)\n\n", NP - 1);
    hoja_datos();
    H("Forma de Lagrange:  P(x) = suma_k y_k * L_k(x)\n");
    for (int k = 0; k < NP; k++) {
        double den = 1;
        H("   L%d(x) = ", k);
        for (int j = 0; j < NP; j++) if (j != k) { H("(x - %.6g)", X[j]); den *= X[k] - X[j]; }
        H(" / (%.8g)\n", den);
    }
    H("\n   P(x) = ");
    for (int k = 0; k < NP; k++) H("%s%.6g*L%d(x)", k ? " + " : "", Y[k], k);
    H("\n\nDesarrollando y agrupando potencias de x:\n   P(x) = %.10f", c[0]);
    for (int i = 1; i < NP; i++) {
        H(" %s %.10f x", c[i] < 0 ? "-" : "+", fabs(c[i]));
        if (i > 1) H("^%d", i);
    }
    H("\n\nControl (el polinomio pasa por todos los puntos):\n");
    for (int i = 0; i < NP; i++) H("   P(%.6g) = %.8f     y_i = %.8f\n", X[i], horner(c, NP, X[i]), Y[i]);
    {
        char g1[160];
        sprintf(g1, "Con %d puntos el polinomio es de grado %d", NP, NP - 1);
        hoja_que_escribir(g1, "El polinomio en forma de Lagrange (con los denominadores)", "El polinomio desarrollado", "El control: P(x_i) = y_i en todos los nodos", (const char *)0);
    }
    hoja_mostrar();
}

static void op_newton(void) {
    double T[MAXPUNTOS][MAXPUNTOS];
    int n = NP, i, j;
    P("\n=== NEWTON POR DIFERENCIAS DIVIDIDAS ===\n");
    P("f[x_i,...,x_i+j] = (f[x_i+1,...,x_i+j] - f[x_i,...,x_i+j-1]) / (x_i+j - x_i)\n\n");
    for (i = 0; i < n; i++) T[i][0] = Y[i];
    for (j = 1; j < n; j++)
        for (i = 0; i < n - j; i++) T[i][j] = (T[i + 1][j - 1] - T[i][j - 1]) / (X[i + j] - X[i]);
    P("  i |      x_i       |");
    for (j = 0; j < n; j++) P("   DD orden %-2d  |", j);
    P("\n");
    for (i = 0; i < n; i++) {
        P("%3d | %14.8f |", i, X[i]);
        for (j = 0; j < n - i; j++) P(" %14.8f |", T[i][j]);
        P("\n");
    }
    P("\nb0, b1, b2, ... = coeficientes del polinomio de Newton = PRIMERA FILA de la tabla.\n");
    P("P(x) = b0 + b1(x - x0) + b2(x - x0)(x - x1) + ...  (cada termino agrega un factor)\n");
    P("\nCoeficientes (primera fila): ");
    for (j = 0; j < n; j++) P(" b%d = %.10f ", j, T[0][j]);
    P("\nP(x) = b0");
    for (j = 1; j < n; j++) {
        P(" + b%d", j);
        for (i = 0; i < j; i++) P("(x - %g)", X[i]);
    }
    P("\nP(x) = %.10f", T[0][0]);
    for (j = 1; j < n; j++) {
        P(" %s %.10f", T[0][j] < 0 ? "-" : "+", fabs(T[0][j]));
        for (i = 0; i < j; i++) P("(x - %g)", X[i]);
    }
    P("\n");
    hoja_reset();
    H("INTERPOLACION - NEWTON POR DIFERENCIAS DIVIDIDAS\n\n");
    hoja_datos();
    H("f[x_i, x_i+1] = (f[x_i+1] - f[x_i]) / (x_i+1 - x_i)  y asi sucesivamente.\n\n");
    H("Tabla de diferencias divididas:\n");
    for (i = 0; i < n; i++) {
        H("   x%d = %-10.6g", i, X[i]);
        for (j = 0; j < n - i; j++) H("  %14.8f", T[i][j]);
        H("\n");
    }
    H("\nQUE SON b0, b1, b2, ...:\n");
    H("   Son los COEFICIENTES del polinomio de Newton, o sea los numeros que multiplican a\n");
    H("   cada termino. Salen de la PRIMERA FILA de la tabla de diferencias divididas.\n");
    H("   El polinomio se arma asi (cada termino tiene un factor (x - x_i) mas que el anterior):\n");
    H("      P(x) = b0 + b1(x - x0) + b2(x - x0)(x - x1) + b3(x - x0)(x - x1)(x - x2) + ...\n\n");
    for (j = 0; j < n; j++) {
        if (j == 0) H("   b0 = f[x0] = y0 = %.10f      (el primer dato)\n", T[0][0]);
        else if (j == 1) H("   b1 = f[x0,x1] = (y1 - y0)/(x1 - x0) = (%.6g - (%.6g))/(%.6g - %.6g) = %.10f      (pendiente entre los 2 primeros puntos)\n", Y[1], Y[0], X[1], X[0], T[0][1]);
        else H("   b%d = f[x0,...,x%d] = %.10f      (diferencia dividida de orden %d)\n", j, j, T[0][j], j);
    }
    H("   Los factores usan los nodos x0 a x%d; el ultimo nodo (x%d = %.6g) no aparece en ningun factor.\n", n - 2, n - 1, X[n - 1]);
    H("\nP(x) = %.10f", T[0][0]);
    for (j = 1; j < n; j++) {
        H(" %s %.10f", T[0][j] < 0 ? "-" : "+", fabs(T[0][j]));
        for (i = 0; i < j; i++) H("(x - %.6g)", X[i]);
    }
    H("\n");
    int hay;
    double xq = leer_num_opcional("x donde evaluar (ENTER = no evaluar): ", &hay);
    if (hay) {
        double s = T[0][n - 1];
        for (j = n - 2; j >= 0; j--) s = s * (xq - X[j]) + T[0][j];
        P("P(%.10g) = %.10f\n", xq, s);
        H("\nEvaluando en x = %.10g:  P(%.10g) = %.10f\n", xq, xq, s);
        if (FEX[0]) {
            double fq = evalf(FEX, xq);
            H("Valor exacto f(%.10g) = %.10f     error = %.6e\n", xq, fq, fabs(s - fq));
        }
        comparar_exacta(xq, s);
    }
    {
        char g1[160];
        sprintf(g1, "Con %d puntos el polinomio es de grado %d", NP, NP - 1);
        hoja_que_escribir(g1, "La tabla de diferencias divididas", "Los coeficientes b0 ... bn (primera fila de la tabla) y el polinomio de Newton armado con ellos", "Si piden un valor: P(x) evaluado", (const char *)0);
    }
    hoja_mostrar();
}

static void op_local(void) {
    P("\n=== INTERPOLACION LOCAL (grado g con los g+1 puntos MAS CERCANOS a x) ===\n");
    P("Ej: \"interpolacion cubica\" -> g = 3 (4 puntos).  Lineal -> g = 1.  Cuadratica -> g = 2.\n");
    int g = leer_int("Grado g  (1 = lineal, 2 = cuadratica, 3 = cubica): ");
    if (g < 1 || g >= NP) { P("El grado tiene que estar entre 1 y %d.\n", NP - 1); return; }
    double xq = leer_num("x donde evaluar  (el valor que pide el enunciado) = ");
    int idx[MAXPUNTOS], i, j;
    for (i = 0; i < NP; i++) idx[i] = i;
    for (i = 0; i < NP; i++)                  /* ordeno por distancia (estable) */
        for (j = i + 1; j < NP; j++)
            if (fabs(X[idx[j]] - xq) < fabs(X[idx[i]] - xq) ||
                (fabs(X[idx[j]] - xq) == fabs(X[idx[i]] - xq) && idx[j] < idx[i])) { int t = idx[i]; idx[i] = idx[j]; idx[j] = t; }
    if (g + 1 < NP && fabs(X[idx[g]] - xq) == fabs(X[idx[g + 1]] - xq))
        P("ATENCION: hay empate de distancias entre x = %g y x = %g. Revisa que puntos pide el enunciado.\n", X[idx[g]], X[idx[g + 1]]);
    int sel[MAXPUNTOS];
    for (i = 0; i <= g; i++) sel[i] = idx[i];
    for (i = 0; i <= g; i++) for (j = i + 1; j <= g; j++) if (sel[j] < sel[i]) { int t = sel[i]; sel[i] = sel[j]; sel[j] = t; }
    double xs[MAXPUNTOS], ys[MAXPUNTOS], c[MAXPUNTOS];
    P("Puntos usados: ");
    for (i = 0; i <= g; i++) { xs[i] = X[sel[i]]; ys[i] = Y[sel[i]]; P("(%g, %g) ", xs[i], ys[i]); }
    P("\n");
    imprimir_forma_lagrange(xs, ys, g + 1);
    coef_canonicos(xs, ys, g + 1, c);
    imprimir_canonico(c, g + 1);
    double pq = lagrange(xs, ys, g + 1, xq);
    linea();
    P("P%d(%.10g) = %.10f\n", g, xq, pq);

    hoja_reset();
    H("INTERPOLACION %s (grado %d) EN x = %.10g\n\n",
      g == 1 ? "LINEAL" : (g == 2 ? "CUADRATICA" : (g == 3 ? "CUBICA" : "LOCAL")), g, xq);
    H("Con grado %d hacen falta %d puntos: se eligen los %d MAS CERCANOS a x = %.10g.\n\n", g, g + 1, g + 1, xq);
    H("Distancias a x = %.10g:\n", xq);
    for (i = 0; i < NP; i++) H("   |%.6g - %.6g| = %.6g%s\n", X[i], xq, fabs(X[i] - xq),
                               (i == sel[0] || (g >= 1 && i == sel[1]) || (g >= 2 && i == sel[2]) || (g >= 3 && i == sel[3])) ? "   <- se usa" : "");
    H("\nPuntos usados:\n");
    for (i = 0; i <= g; i++) H("   (%.6g , %.6g)\n", xs[i], ys[i]);
    H("\nPolinomio local (forma canonica):\n   P(x) = %.10f", c[0]);
    for (i = 1; i <= g; i++) {
        H(" %s %.10f x", c[i] < 0 ? "-" : "+", fabs(c[i]));
        if (i > 1) H("^%d", i);
    }
    H("\n\nRESULTADO:  P(%.10g) = %.10f\n", xq, pq);
    if (FEX[0]) {
        double fq = evalf(FEX, xq);
        H("Valor exacto f(%.10g) = %.10f     error = %.6e\n", xq, fq, fabs(pq - fq));
    }
    comparar_exacta(xq, pq);
    linea();
    {
        char g1[200];
        sprintf(g1, "Que grado se usa (%d) y cuantos puntos hacen falta (%d)", g, g + 1);
        hoja_que_escribir(g1, "Por que esos puntos: son los mas cercanos al valor buscado (con las distancias)", "El polinomio usado", "El valor estimado", (const char *)0);
    }
    hoja_mostrar();
}

static void op_error(void) {
    P("\n=== ERROR  e(x) = |P(x) - f(x)|  EN UN RANGO ===\n");
    if (!FEX[0]) leer_funcion("f(x) exacta  (para comparar el error) = ", FEX);
    double c[MAXPUNTOS];
    coef_canonicos(X, Y, NP, c);
    double a = leer_num("Desde x  (inicio del rango) = "), b = leer_num("Hasta x  (fin del rango) = ");
    int n = leer_int("Cantidad de pasos  (en cuantos pedazos dividir el rango, ej: 20): ");
    if (n < 1) n = 20;
    P("\n        x        |       P(x)       |       f(x)       |   |P - f|\n");
    for (int i = 0; i <= n; i++) {
        double x = a + (b - a) * i / n, p = horner(c, NP, x), f = evalf(FEX, x);
        int nodo = 0;
        for (int k = 0; k < NP; k++) if (fabs(x - X[k]) < 1e-12) nodo = 1;
        P("%16.10f | %16.10f | %16.10f | %.4e %s\n", x, p, f, fabs(p - f), nodo ? " <- nodo (error 0 si y=f exacto)" : "");
    }
    P("El error es 0 en los nodos y crece entre ellos y, sobre todo, lejos de los nodos (extrapolar es peligroso).\n");
}

static void op_buscar(void) {
    P("\n=== BUSCAR x TAL QUE P(x) = c   (c = 0 -> raiz del polinomio) ===\n");
    double co[MAXPUNTOS];
    coef_canonicos(X, Y, NP, co);
    double c = leer_num("c  (valor buscado: se resuelve P(x) = c; poner 0 para la raiz) = ");
    double a = leer_num("Intervalo donde buscar:  a = "), b = leer_num("                         b = ");
    double fa = horner(co, NP, a) - c, fb = horner(co, NP, b) - c;
    if (fa * fb > 0) { P("P(a)-c y P(b)-c tienen el mismo signo (%.4g, %.4g). Elegi otro intervalo.\n", fa, fb); return; }
    for (int k = 0; k < 200; k++) {
        double m = 0.5 * (a + b), fm = horner(co, NP, m) - c;
        if (fa * fm <= 0) { b = m; } else { a = m; fa = fm; }
    }
    P("x = %.10f   (biseccion sobre P(x) - c, 200 iteraciones)\n", 0.5 * (a + b));
}

int main(void) {
    abrir_log("salida_interpolacion.txt");
    P("\n############  INTERPOLACION POLINOMIAL   (Guia 5)  ############\n");
    cargar();
    for (;;) {
        P("\n======================== MENU (%d puntos) ========================\n", NP);
        P("  OPCION                          USALA SI EL ENUNCIADO DICE...\n");
        P("  1) Cargar otros puntos\n");
        P("  2) Lagrange: evaluar en un x    'estime el valor en x = ...', 'Lagrange', 'calcule P(1.6)'\n");
        P("  3) Escribir el polinomio        'obtenga / escriba el polinomio interpolador (de Lagrange)'\n");
        P("  4) Newton dif. divididas        'diferencias divididas', 'polinomio de Newton', 'tabla'\n");
        P("  5) Interpolacion LOCAL grado g  'interpolacion lineal / cuadratica / cubica en x = ...'\n");
        P("  6) Error |P(x) - f(x)|          'error absoluto exacto', 'grafique el error', 'comportamiento cerca de los nodos'\n");
        P("  7) Buscar x con P(x) = c        'para que x se alcanza ...', 'raiz del polinomio', 'interpolacion inversa'\n");
        P("  0) Salir\n");
        int op = leer_int("Opcion: ");
        if (op == 0) break;
        switch (op) {
            case 1: cargar(); break;
            case 2: op_lagrange(); break;
            case 3: op_polinomio(); break;
            case 4: op_newton(); break;
            case 5: op_local(); break;
            case 6: op_error(); break;
            case 7: op_buscar(); break;
            default: P("Opcion invalida.\n");
        }
        if (op >= 1 && op <= 7) pausa();
    }
    if (LOG_MN) fclose(LOG_MN);
    return 0;
}
