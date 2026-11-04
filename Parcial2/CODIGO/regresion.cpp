/* regresion.cpp -- REGRESION POR MINIMOS CUADRADOS   (Guia 5, segunda parte)
 *
 *  Recta, polinomica de grado m, base de funciones a eleccion (a sin x + b cos x,
 *  a e^(x^2) + b, a + b x + c e^x ...), exponencial, potencial y saturacion
 *  (linealizadas). Muestra ecuaciones normales, Sr, St, r^2, r y residuos.
 *  Todo queda guardado en salida_regresion.txt
 */
#include "mn_comun.h"

#define MAXB 10

static int NP = 0;
static double X[MAXPUNTOS], Y[MAXPUNTOS];

/* Modelo actual: y = suma C[k] * phi_k(x)  (con transformacion si TRANSF != 0) */
static int M = 0;
static char BASE[MAXB][128];
static double C[MAXB];
static int TRANSF = 0;           /* 0 ninguna, 1 exponencial, 2 potencial, 3 saturacion */
static double AA, BB;            /* coeficientes originales de los modelos linealizados */
static int HAY_MODELO = 0;

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
        printf("   Escribi los numeros separados por ESPACIOS.\n");
    }
}

static void cargar(void) {
    P("\n=== CARGAR DATOS ===\n");
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   x_i , y_i = los datos medidos ;  n = cantidad de datos\n");
    P("   y ajustado = lo que da el modelo en ese x  (la curva NO pasa por los puntos)\n");
    P("   residuo = y_i - y ajustado ;  Sr = suma de residuos al cuadrado (se hace MINIMA)\n");
    P("   St = suma de (y_i - promedio)^2 ;  r^2 = (St - Sr)/St   (1 = ajuste perfecto)\n");
    P("   phi_k(x) = funciones de la base ;  c_k = coeficientes que se calculan\n");
    do { NP = leer_lista("Valores de x  (las abscisas de la tabla, separadas por espacio): ", X, MAXPUNTOS); } while (NP < 2);
    for (;;) {
        int c = leer_lista("Valores de y  (las ordenadas, en el mismo orden que los x): ", Y, MAXPUNTOS);
        if (c == NP) break;
        printf("   Cargaste %d valores de x y %d de y.\n", NP, c);
    }
    P("\n  i |        x_i        |        y_i\n");
    for (int i = 0; i < NP; i++) P("%3d | %17.10f | %17.10f\n", i + 1, X[i], Y[i]);
    HAY_MODELO = 0;
}

/* Gauss con pivoteo para el sistema normal M x M */
static int resolver(double A[MAXB][MAXB], double b[MAXB], int n, double x[MAXB]) {
    double T[MAXB][MAXB + 1];
    int i, j, k;
    for (i = 0; i < n; i++) { for (j = 0; j < n; j++) T[i][j] = A[i][j]; T[i][n] = b[i]; }
    for (k = 0; k < n; k++) {
        int p = k;
        for (i = k + 1; i < n; i++) if (fabs(T[i][k]) > fabs(T[p][k])) p = i;
        for (j = 0; j <= n; j++) { double t = T[k][j]; T[k][j] = T[p][j]; T[p][j] = t; }
        if (fabs(T[k][k]) < 1e-300) return 0;
        for (i = k + 1; i < n; i++) {
            double m = T[i][k] / T[k][k];
            for (j = k; j <= n; j++) T[i][j] -= m * T[k][j];
        }
    }
    for (i = n - 1; i >= 0; i--) {
        double s = T[i][n];
        for (j = i + 1; j < n; j++) s -= T[i][j] * x[j];
        x[i] = s / T[i][i];
    }
    return 1;
}


/* Escribe la deduccion de las ecuaciones normales para la base actual */
static void deduccion_normales(const char *nv) {
    int j, k;
    H("DEDUCCION DE LAS ECUACIONES NORMALES:\n");
    H("   El modelo es  %s = ", nv);
    for (k = 0; k < M; k++) H("%sc%d*%s", k ? " + " : "", k + 1, BASE[k]);
    H("\n");
    if (M == 2) H("   (c1 y c2 son los coeficientes a buscar; en el enunciado suelen llamarse a y b)\n");
    H("\n   Minimos cuadrados: se eligen los coeficientes que hacen MINIMA la suma de los\n");
    H("   cuadrados de los errores entre los datos y el modelo:\n");
    H("      S(c1");
    for (k = 1; k < M; k++) H(",c%d", k + 1);
    H(") = suma_i [ y_i");
    for (k = 0; k < M; k++) H(" - c%d*%s(i)", k + 1, BASE[k]);
    H(" ]^2\n\n");
    H("   En el minimo, las derivadas parciales respecto de cada coeficiente valen cero:\n");
    for (j = 0; j < M; j++) {
        H("      dS/dc%d = -2 * suma_i %s(i) * [ y_i", j + 1, BASE[j]);
        for (k = 0; k < M; k++) H(" - c%d*%s(i)", k + 1, BASE[k]);
        H(" ] = 0\n");
    }
    H("\n   Dividiendo por -2 y distribuyendo la suma, queda un sistema LINEAL en los coeficientes\n");
    H("   (esas son las ECUACIONES NORMALES):\n");
    for (j = 0; j < M; j++) {
        H("      ");
        for (k = 0; k < M; k++) H("%s[suma %s*%s] c%d", k ? " + " : "", BASE[j], BASE[k], k + 1);
        H("  =  suma %s*y\n", BASE[j]);
    }
    H("\n   Calculando esas sumas con los datos de la tabla:\n\n");
    P("\n(La deduccion de las ecuaciones normales quedo en el TEXTO PARA COPIAR EN LA HOJA)\n");
}

/* Ajusta y = suma C[k] phi_k(u) sobre los datos (u, v) e imprime el sistema normal */
static int ajustar(const double *u, const double *v, const char *nu, const char *nv) {
    double A[MAXB][MAXB], b[MAXB];
    int j, k, i;
    for (j = 0; j < M; j++) {
        for (k = 0; k < M; k++) {
            double s = 0;
            for (i = 0; i < NP; i++) s += evalf(BASE[j], u[i]) * evalf(BASE[k], u[i]);
            A[j][k] = s;
        }
        double s = 0;
        for (i = 0; i < NP; i++) s += evalf(BASE[j], u[i]) * v[i];
        b[j] = s;
    }
    P("\nModelo lineal en los coeficientes:  %s = ", nv);
    for (k = 0; k < M; k++) P("%sc%d*[%s]", k ? " + " : "", k + 1, BASE[k]);
    P("      (variable: %s = %s)\n", nu[0] ? "u" : "x", nu[0] ? nu : "x");
    P("\nECUACIONES NORMALES  (Phi^T Phi) c = Phi^T y :   elemento (j,k) = suma_i phi_j(x_i)*phi_k(x_i)\n");
    deduccion_normales(nv);
    H("ECUACIONES NORMALES:  (Phi^T Phi) c = Phi^T y\n");
    H("El elemento (j,k) es la suma de phi_j(x_i)*phi_k(x_i) sobre los %d datos,\n", NP);
    H("y el termino independiente j es la suma de phi_j(x_i)*y_i.\n\n");
    for (j = 0; j < M; j++) {
        P("   [");
        for (k = 0; k < M; k++) P(" %16.8f", A[j][k]);
        P(" ] [c%d]   [ %16.8f ]\n", j + 1, b[j]);
        H("   [");
        for (k = 0; k < M; k++) H(" %14.8f", A[j][k]);
        H(" ] [c%d]  =  [ %14.8f ]\n", j + 1, b[j]);
    }
    H("\nResolviendo ese sistema con eliminacion de Gauss:\n");
    if (!resolver(A, b, M, C)) { P("El sistema normal es singular: la base tiene funciones repetidas o faltan datos.\n"); return 0; }
    P("\nCoeficientes (Gauss con pivoteo):\n");
    for (k = 0; k < M; k++) {
        P("   c%d = %.10f     (acompania a %s)\n", k + 1, C[k], BASE[k]);
        H("   c%d = %.10f     (coeficiente de %s)\n", k + 1, C[k], BASE[k]);
    }
    return 1;
}

static void hoja_datos_reg(const char *titulo) {
    hoja_reset();
    H("%s\n\n", titulo);
    H("Datos (%d puntos):\n", NP);
    H("   x: ");
    for (int i = 0; i < NP; i++) H("%s%.6g", i ? " ; " : "", X[i]);
    H("\n   y: ");
    for (int i = 0; i < NP; i++) H("%s%.6g", i ? " ; " : "", Y[i]);
    H("\n\nCriterio de minimos cuadrados: se eligen los coeficientes que hacen minima\n");
    H("Sr = suma (y_i - y_ajustado_i)^2.\n\n");
}

static double modelo(double x) {
    if (TRANSF == 1) return AA * exp(BB * x);
    if (TRANSF == 2) return AA * pow(x, BB);
    if (TRANSF == 3) return AA * x / (BB + x);
    double s = 0;
    for (int k = 0; k < M; k++) s += C[k] * evalf(BASE[k], x);
    return s;
}

/* Estadisticos sobre (u, v) con prediccion vhat */
static void estadisticos(const double *v, const double *vhat, const char *titulo, int params) {
    double media = 0, St = 0, Sr = 0;
    int i;
    for (i = 0; i < NP; i++) media += v[i];
    media /= NP;
    for (i = 0; i < NP; i++) { St += (v[i] - media) * (v[i] - media); Sr += (v[i] - vhat[i]) * (v[i] - vhat[i]); }
    double r2 = St > 0 ? (St - Sr) / St : 1;
    P("\n%s\n", titulo);
    P("   media       = %.10f\n", media);
    P("   St = suma (y_i - media)^2  = %.10e\n", St);
    P("   Sr = suma (y_i - yhat_i)^2 = %.10e\n", Sr);
    P("   r^2 = (St - Sr)/St         = %.10f\n", r2);
    P("   r   = raiz(r^2)            = %.10f\n", r2 >= 0 ? sqrt(r2) : 0.0);
    if (NP > 1) P("   Sy  = raiz(St/(n-1))       = %.10e   (desvio estandar de los datos)\n", sqrt(St / (NP - 1)));
    if (NP > params) P("   Sy/x = raiz(Sr/(n-%d))      = %.10e   (error estandar del ajuste)\n", params, sqrt(Sr / (NP - params)));
    P("   ECM = raiz(Sr/n)           = %.10e\n", sqrt(Sr / NP));
    P("   Interpretacion: r^2 cerca de 1 = buen ajuste (el modelo explica el %.4f %% de la variacion).\n", 100 * r2);

    H("\n%s\n", titulo);
    H("   promedio de y = %.10f\n", media);
    H("   St = suma (y_i - promedio)^2  = %.10e\n", St);
    H("   Sr = suma (y_i - y_ajustado)^2 = %.10e\n", Sr);
    H("   r^2 = (St - Sr)/St = %.10f        r = %.10f\n", r2, r2 >= 0 ? sqrt(r2) : 0.0);
    if (NP > params) H("   Sy/x = raiz(Sr/(n-%d)) = %.6e   (error estandar del ajuste)\n", params, sqrt(Sr / (NP - params)));
    H("   Conclusion: r^2 = %.6f, el modelo explica el %.4f %% de la variacion de los datos.\n\n", r2, 100 * r2);
    H("   QUE ES CADA COSA:\n");
    H("   - residuo de un punto = y_i - y_ajustado_i . No es un error del metodo: es cuanto\n");
     H("     se aparta ese dato del modelo (en regresion la curva NO pasa por los puntos).\n");
    H("   - Sr suma esos residuos al cuadrado; es lo que el metodo hace MINIMO.\n");
    H("   - Sy/x = raiz(Sr/(n-p)) es el ERROR ESTANDAR del ajuste: el desvio tipico entre los\n");
     H("     datos y el modelo, en las mismas unidades que y.\n");
    H("   - r^2 no tiene unidades: va de 0 a 1 y dice que proporcion de la variacion explica\n");
     H("     el modelo. Un r^2 alto NO garantiza que el modelo sea el correcto: hay que mirar\n");
     H("     que los residuos no tengan una forma (si la tienen, falta curvatura en el modelo).\n");
}

static void tabla_residuos(void) {
    double yh[MAXPUNTOS];
    P("\n  i |       x_i       |       y_i       |  yhat = modelo  |  residuo y - yhat\n");
    H("\nTabla de residuos:\n   i |      x_i      |      y_i      |   y ajustado  |  residuo\n");
    for (int i = 0; i < NP; i++) {
        yh[i] = modelo(X[i]);
        P("%3d | %15.8f | %15.8f | %15.8f | %15.8e\n", i + 1, X[i], Y[i], yh[i], Y[i] - yh[i]);
        H("  %2d | %13.6g | %13.6g | %13.8f | %+.6f\n", i + 1, X[i], Y[i], yh[i], Y[i] - yh[i]);
    }
    estadisticos(Y, yh, TRANSF ? "BONDAD DEL AJUSTE EN LAS VARIABLES ORIGINALES (x, y):" : "BONDAD DEL AJUSTE:", TRANSF ? 2 : M);
}

/* ------------------------------------------------------------------ */

/* Aproxima la raiz de f(x) con la raiz del modelo ajustado y calcula su error
   contra el valor exacto que da el enunciado. */
static void raiz_del_modelo(int es_recta) {
    int hay;
    double xe = leer_num_opcional("Valor EXACTO de la raiz de f(x), si el enunciado lo da (ej: 1.2620326547; ENTER = no): ", &hay);
    if (!hay) return;
    double lo = X[0], hi = X[0];
    for (int i = 1; i < NP; i++) { if (X[i] < lo) lo = X[i]; if (X[i] > hi) hi = X[i]; }
    double ancho = hi - lo;
    lo -= 0.5 * ancho; hi += 0.5 * ancho;
    /* buscar cambios de signo del modelo y quedarse con la raiz mas cercana a la exacta */
    int N = 4000, encontrada = 0;
    double mejor = 0, xa = lo, fa = modelo(xa);
    for (int i = 1; i <= N; i++) {
        double xb = lo + (hi - lo) * i / N, fb = modelo(xb);
        if (fa == fa && fb == fb && fa * fb <= 0) {
            double a = xa, b = xb, fA = fa;
            for (int k = 0; k < 200; k++) {
                double m = 0.5 * (a + b), fm = modelo(m);
                if (fA * fm <= 0) b = m; else { a = m; fA = fm; }
            }
            double r = 0.5 * (a + b);
            if (!encontrada || fabs(r - xe) < fabs(mejor - xe)) mejor = r;
            encontrada = 1;
        }
        xa = xb; fa = fb;
    }
    H("\nAPROXIMACION DE LA RAIZ DE f(x) USANDO EL MODELO AJUSTADO:\n");
    P("\nAPROXIMACION DE LA RAIZ DE f(x) USANDO EL MODELO AJUSTADO:\n");
    if (!encontrada) {
        H("   El modelo no corta el eje x cerca del rango de los datos: no da una raiz.\n");
        P("   El modelo no corta el eje x cerca del rango de los datos.\n");
        return;
    }
    if (es_recta) {
        H("   La raiz es donde la recta vale cero:  0 = a0 + a1*x   =>   x = -a0/a1\n");
        H("   x = -(%.10f)/(%.10f) = %.10f\n", C[0], C[1], mejor);
    } else {
        H("   La raiz es donde el modelo vale cero; se busca por biseccion sobre el modelo:\n");
        H("   x = %.10f      (control: modelo(x) = %.2e)\n", mejor, modelo(mejor));
    }
    double ea = fabs(mejor - xe);
    H("   Valor exacto de la raiz (dato del enunciado): %.10f\n", xe);
    H("   ERROR ABSOLUTO = |x_aproximada - x_exacta| = |%.10f - %.10f| = %.10f\n", mejor, xe, ea);
    if (xe != 0) H("   ERROR RELATIVO = 100*|x_aprox - x_exacta|/|x_exacta| = %.4f %%\n", 100 * ea / fabs(xe));
    P("   Raiz aproximada x = %.10f    exacta = %.10f    error absoluto = %.10f", mejor, xe, ea);
    if (xe != 0) P("    error relativo = %.4f %%", 100 * ea / fabs(xe));
    P("\n");
}

static void op_recta(void) {
    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    P("\n=== REGRESION LINEAL  y = a0 + a1 x ===\n");
    for (int i = 0; i < NP; i++) { sx += X[i]; sy += Y[i]; sxx += X[i] * X[i]; sxy += X[i] * Y[i]; }
    P("n = %d   suma x = %.10f   suma y = %.10f   suma x^2 = %.10f   suma xy = %.10f\n", NP, sx, sy, sxx, sxy);
    double a1 = (NP * sxy - sx * sy) / (NP * sxx - sx * sx), a0 = sy / NP - a1 * sx / NP;
    hoja_datos_reg("REGRESION LINEAL   y = a0 + a1*x   (minimos cuadrados)");
    H("Tabla de sumas:\n    i |      x_i      |      y_i      |     x_i^2     |    x_i*y_i\n");
    for (int i = 0; i < NP; i++)
        H("   %2d | %13.6g | %13.6g | %13.6g | %13.6g\n", i + 1, X[i], Y[i], X[i] * X[i], X[i] * Y[i]);
    {
        char sx1[700], sy1[700];
        int o1 = sprintf(sx1, "   suma x = ");
        int o2 = sprintf(sy1, "   suma y = ");
        for (int i = 0; i < NP; i++) {
            o1 += sprintf(sx1 + o1, "%s%.6g", i ? " + " : "", X[i]);
            o2 += sprintf(sy1 + o2, "%s%.6g", i ? " + " : "", Y[i]);
        }
        H("%s = %.8g\n", sx1, sx);
        H("%s = %.8g\n", sy1, sy);
        H("   suma x^2 = %.8g       suma xy = %.8g       n = %d\n\n", sxx, sxy, NP);
    }
    H("DEDUCCION (minimos cuadrados para la recta y = a0 + a1*x):\n");
    H("   S(a0,a1) = suma_i (y_i - a0 - a1*x_i)^2  se hace minima cuando sus derivadas valen cero:\n");
    H("   dS/da0 = -2*suma (y_i - a0 - a1*x_i) = 0        =>   n*a0 + (suma x)*a1 = suma y\n");
    H("   dS/da1 = -2*suma x_i*(y_i - a0 - a1*x_i) = 0    =>   (suma x)*a0 + (suma x^2)*a1 = suma xy\n");
    H("   Con los datos:   %d*a0 + %.8g*a1 = %.8g      y      %.8g*a0 + %.8g*a1 = %.8g\n", NP, sx, sy, sx, sxx, sxy);
    H("   Resolviendo ese sistema de 2x2 se obtienen las formulas:\n\n");
    H("a1 = ( n*suma(xy) - suma(x)*suma(y) ) / ( n*suma(x^2) - (suma x)^2 )\n");
    H("   = ( %d*%.8g - %.8g*%.8g ) / ( %d*%.8g - %.8g^2 )\n", NP, sxy, sx, sy, NP, sxx, sx);
    H("   = %.8g / %.8g = %.10f\n\n", NP * sxy - sx * sy, NP * sxx - sx * sx, a1);
    H("a0 = promedio(y) - a1*promedio(x) = %.8f - (%.10f)*(%.8f) = %.10f\n\n", sy / NP, a1, sx / NP, a0);
    P("a1 = (n*sum(xy) - sum(x)*sum(y)) / (n*sum(x^2) - sum(x)^2) = (%d*%.8f - %.8f*%.8f) / (%d*%.8f - %.8f^2) = %.10f\n",
      NP, sxy, sx, sy, NP, sxx, sx, a1);
    P("a0 = media(y) - a1*media(x) = %.10f - %.10f*%.10f = %.10f\n", sy / NP, a1, sx / NP, a0);
    TRANSF = 0; M = 2;
    strcpy(BASE[0], "1"); strcpy(BASE[1], "x");
    C[0] = a0; C[1] = a1;
    HAY_MODELO = 1;
    linea();
    P("RECTA:  y = %.10f %s %.10f x\n", a0, a1 < 0 ? "-" : "+", fabs(a1));
    if (a1 != 0) P("Corta al eje x (y = 0) en x = -a0/a1 = %.10f\n", -a0 / a1);
    H("RECTA DE AJUSTE:  y = %.10f %s %.10f x\n", a0, a1 < 0 ? "-" : "+", fabs(a1));
    if (a1 != 0) H("Corta al eje x (y = 0) en  x = -a0/a1 = %.10f\n", -a0 / a1);
    tabla_residuos();
    linea();
    raiz_del_modelo(1);
    hoja_que_escribir("La tabla de sumas: x, y, x^2, x*y", "Las formulas de a1 y a0 con los numeros reemplazados", "La recta obtenida", "r^2 y r (coeficiente de correlacion) con su interpretacion", "Los residuos: si tienen patron (+ - - - +), la recta no es el modelo adecuado", "Si piden raiz: x = -a0/a1 y su error respecto del valor exacto", (const char *)0);
    hoja_mostrar();
}

static void op_base(int polinomica) {
    TRANSF = 0;
    if (polinomica) {
        int g = leer_int("Grado del polinomio  (1 = recta, 2 = parabola, 3 = cubica): ");
        if (g < 1 || g + 1 > MAXB || g + 1 > NP) { P("Grado invalido (necesitas al menos grado+1 puntos).\n"); return; }
        M = g + 1;
        for (int k = 0; k < M; k++) { if (k == 0) strcpy(BASE[k], "1"); else if (k == 1) strcpy(BASE[k], "x"); else sprintf(BASE[k], "x^%d", k); }
        P("\n=== REGRESION POLINOMICA DE GRADO %d ===\n", g);
    } else {
        P("\n=== REGRESION CON BASE DE FUNCIONES A ELECCION ===\n");
        P("El modelo es  y = c1*phi1(x) + c2*phi2(x) + ...   Ejemplos:\n");
        P("   y = a sin(x) + b cos(x)      ->  2 funciones:  sin(x)   cos(x)\n");
        P("   y = a e^(x^2) + b            ->  2 funciones:  exp(x^2) 1\n");
        P("   y = a + b x + c e^x          ->  3 funciones:  1  x  exp(x)\n");
        P("(El termino constante se escribe como la funcion 1.)\n");
        do { M = leer_int("Cantidad de funciones de la base  (ej: 2 para a*sin(x) + b*cos(x)): "); } while (M < 1 || M > MAXB);
        for (int k = 0; k < M; k++) {
            char msg[32];
            sprintf(msg, "phi%d(x) =  (funcion %d de la base, ej: sin(x) o 1): ", k + 1, k + 1);
            leer_funcion(msg, BASE[k]);
        }
    }
    {
        char tit[300];
        sprintf(tit, "REGRESION POR MINIMOS CUADRADOS   y = c1*%s%s", BASE[0], M > 1 ? " + ..." : "");
        hoja_datos_reg(tit);
        H("Modelo propuesto:  y = ");
        for (int k = 0; k < M; k++) H("%sc%d*[%s]", k ? " + " : "", k + 1, BASE[k]);
        H("\n\n");
    }
    if (!ajustar(X, Y, "", "y")) return;
    HAY_MODELO = 1;
    linea();
    P("MODELO:  y = ");
    H("\nMODELO AJUSTADO:  y = ");
    for (int k = 0; k < M; k++) {
        P("%s%.10f*%s", k ? (C[k] < 0 ? " - " : " + ") : (C[k] < 0 ? "-" : ""), fabs(C[k]), BASE[k]);
        H("%s%.10f*%s", k ? (C[k] < 0 ? " - " : " + ") : (C[k] < 0 ? "-" : ""), fabs(C[k]), BASE[k]);
    }
    P("\n");
    H("\n");
    tabla_residuos();
    linea();
    raiz_del_modelo(0);
    hoja_que_escribir("El modelo y las funciones de la base", "La matriz normal y el vector independiente", "Los coeficientes obtenidos", "Sr, St, r^2 y r con la conclusion sobre el ajuste", (const char *)0);
    hoja_mostrar();
}

static void op_linealizado(int tipo) {
    double u[MAXPUNTOS], v[MAXPUNTOS];
    const char *nom[] = {"", "EXPONENCIAL  y = a e^(b x)", "POTENCIAL  y = a x^b", "SATURACION  y = a x / (b + x)"};
    P("\n=== MODELO %s (linealizado) ===\n", nom[tipo]);
    if (tipo == 1) P("Se toma ln:  ln y = ln a + b x   ->  recta con u = x, v = ln y.  a = e^(c1), b = c2\n");
    if (tipo == 2) P("Se toma ln:  ln y = ln a + b ln x   ->  recta con u = ln x, v = ln y.  a = e^(c1), b = c2\n");
    if (tipo == 3) P("Se invierte:  1/y = 1/a + (b/a)(1/x)   ->  recta con u = 1/x, v = 1/y.  a = 1/c1, b = c2/c1\n");
    for (int i = 0; i < NP; i++) {
        if ((tipo == 1 || tipo == 2) && Y[i] <= 0) { P("Hay y <= 0: no se puede tomar logaritmo.\n"); return; }
        if (tipo == 2 && X[i] <= 0) { P("Hay x <= 0: no se puede tomar logaritmo.\n"); return; }
        if (tipo == 3 && (X[i] == 0 || Y[i] == 0)) { P("Hay x = 0 o y = 0: no se puede invertir.\n"); return; }
        u[i] = (tipo == 2) ? log(X[i]) : (tipo == 3 ? 1 / X[i] : X[i]);
        v[i] = (tipo == 3) ? 1 / Y[i] : log(Y[i]);
    }
    P("\n  i |        u        |        v\n");
    {
        char tit[200];
        sprintf(tit, "REGRESION - MODELO %s (linealizado)", nom[tipo]);
        hoja_datos_reg(tit);
        if (tipo == 1) H("Se toman logaritmos:  ln y = ln a + b*x   ->  recta con u = x , v = ln y\n");
        if (tipo == 2) H("Se toman logaritmos:  ln y = ln a + b*ln x  ->  recta con u = ln x , v = ln y\n");
        if (tipo == 3) H("Se invierte:  1/y = 1/a + (b/a)*(1/x)  ->  recta con u = 1/x , v = 1/y\n");
        H("\nDatos transformados:\n    i |       u       |       v\n");
        for (int i = 0; i < NP; i++) H("   %2d | %13.8f | %13.8f\n", i + 1, u[i], v[i]);
        H("\n");
    }
    for (int i = 0; i < NP; i++) P("%3d | %15.10f | %15.10f\n", i + 1, u[i], v[i]);
    TRANSF = 0; M = 2;
    strcpy(BASE[0], "1"); strcpy(BASE[1], "x");
    if (!ajustar(u, v, tipo == 1 ? "x" : (tipo == 2 ? "ln x" : "1/x"), tipo == 3 ? "1/y" : "ln y")) return;
    double vh[MAXPUNTOS];
    for (int i = 0; i < NP; i++) vh[i] = C[0] + C[1] * u[i];
    estadisticos(v, vh, "BONDAD DEL AJUSTE EN LAS VARIABLES TRANSFORMADAS (u, v):", 2);
    if (tipo == 3) { AA = 1 / C[0]; BB = C[1] / C[0]; }
    else { AA = exp(C[0]); BB = C[1]; }
    TRANSF = tipo;
    HAY_MODELO = 1;
    linea();
    if (tipo == 1) P("MODELO:  y = %.10f * e^(%.10f x)\n", AA, BB);
    if (tipo == 2) P("MODELO:  y = %.10f * x^(%.10f)\n", AA, BB);
    if (tipo == 3) P("MODELO:  y = %.10f * x / (%.10f + x)\n", AA, BB);
    H("\nVolviendo a las variables originales:\n");
    if (tipo == 1) H("   a = e^(c1) = e^(%.10f) = %.10f     b = c2 = %.10f\n   MODELO:  y = %.10f * e^(%.10f x)\n", C[0], AA, BB, AA, BB);
    if (tipo == 2) H("   a = e^(c1) = e^(%.10f) = %.10f     b = c2 = %.10f\n   MODELO:  y = %.10f * x^(%.10f)\n", C[0], AA, BB, AA, BB);
    if (tipo == 3) H("   a = 1/c1 = %.10f     b = c2/c1 = %.10f\n   MODELO:  y = %.10f * x / (%.10f + x)\n", AA, BB, AA, BB);
    tabla_residuos();
    linea();
    raiz_del_modelo(0);
    hoja_que_escribir("La transformacion usada (logaritmo o inversa) y por que", "La tabla de datos transformados", "Los coeficientes y como se vuelve a a y b", "r^2, aclarando en que variables se calculo", (const char *)0);
    hoja_mostrar();
}

static void op_evaluar(void) {
    if (!HAY_MODELO) { P("Primero ajusta un modelo.\n"); return; }
    P("\n=== USAR EL ULTIMO MODELO ===\n  1) Evaluar y en un x\n  2) Buscar x tal que modelo(x) = c   (c = 0 -> raiz)\n");
    int op = leer_int("Opcion: ");
    if (op == 1) {
        double x = leer_num("x  (punto donde evaluar el modelo) = ");
        P("y(%.10g) = %.10f\n", x, modelo(x));
    } else if (op == 2) {
        double c = leer_num("c  (valor buscado: modelo(x) = c; poner 0 para la raiz) = ");
        double a = leer_num("Intervalo donde buscar:  a = "), b = leer_num("                         b = ");
        double fa = modelo(a) - c, fb = modelo(b) - c;
        if (fa * fb > 0) { P("modelo(a)-c y modelo(b)-c tienen el mismo signo. Elegi otro intervalo.\n"); return; }
        for (int k = 0; k < 200; k++) {
            double m = 0.5 * (a + b), fm = modelo(m) - c;
            if (fa * fm <= 0) b = m; else { a = m; fa = fm; }
        }
        P("x = %.10f   (biseccion sobre modelo(x) - c)\n", 0.5 * (a + b));
    }
}

int main(void) {
    abrir_log("salida_regresion.txt");
    P("\n############  REGRESION POR MINIMOS CUADRADOS   (Guia 5)  ############\n");
    cargar();
    for (;;) {
        P("\n======================== MENU (%d datos) ========================\n", NP);
        P("  OPCION                          USALA SI EL ENUNCIADO DICE...\n");
        P("  1) Cargar otros datos\n");
        P("  2) Recta y = a0 + a1 x          'regresion lineal', 'recta de mejor ajuste', 'funcion lineal'\n");
        P("  3) Polinomica de grado m        'regresion cuadratica' (m = 2), 'cubica' (m = 3)\n");
        P("  4) Base a eleccion              te dan el modelo: 'a sin(x) + b cos(x)', 'a e^(x^2) + b', '1, x, e^x'\n");
        P("  5) Exponencial y = a e^(bx)     'ajuste exponencial', 'crecimiento o decaimiento'\n");
        P("  6) Potencial y = a x^b          'ajuste potencial', 'ley de potencia'\n");
        P("  7) Saturacion y = a x/(b+x)     'saturacion'\n");
        P("  8) Usar el ultimo modelo        'aproxime la raiz con la recta', 'prediga y en x = ...'\n");
        P("  0) Salir\n");
        int op = leer_int("Opcion: ");
        if (op == 0) break;
        switch (op) {
            case 1: cargar(); break;
            case 2: op_recta(); break;
            case 3: op_base(1); break;
            case 4: op_base(0); break;
            case 5: op_linealizado(1); break;
            case 6: op_linealizado(2); break;
            case 7: op_linealizado(3); break;
            case 8: op_evaluar(); break;
            default: P("Opcion invalida.\n");
        }
        if (op >= 1 && op <= 8) pausa();
    }
    if (LOG_MN) fclose(LOG_MN);
    return 0;
}
