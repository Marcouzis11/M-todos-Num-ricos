/* sistemas.cpp -- SISTEMAS DE ECUACIONES LINEALES  A x = b   (Guias 3 y 4)
 *
 *  Gauss sin/con pivoteo (paso a paso), LU (Doolittle), Jacobi, Gauss-Seidel,
 *  SOR (y barrido de omega), diagonal dominante, radio espectral, reordenar filas.
 *  Todo queda guardado en salida_sistemas.txt
 */
#include "mn_comun.h"

static int N = 0;
static double A[MAXN][MAXN], B[MAXN];
static double XEXACTA[MAXN];   /* solucion exacta, si el enunciado la da */
static int HAYEXACTA = 0;

/* ------------------------------------------------------------------ */
/* Lee 'cant' numeros separados por espacios. Devuelve 0 si ENTER vacio y se permite. */
static int leer_valores(const char *msg, double *v, int cant, int permitir_vacio) {
    char buf[1024];
    for (;;) {
        leer_linea(msg, buf, 1024);
        if (!buf[0] && permitir_vacio) return 0;
        int c = 0, ok = 1;
        char *tok = strtok(buf, " \t;|");
        while (tok) {
            if (c >= cant) { ok = 0; break; }
            ep = tok; ex = 0; eerr = 0;
            double val = e_expr();
            e_sp();
            if (eerr || *ep) { ok = 0; break; }
            v[c++] = val;
            tok = strtok(NULL, " \t;|");
        }
        if (ok && c == cant) return 1;
        printf("   Tenes que escribir %d numeros separados por ESPACIOS (ej: 4 1 0 -1).\n", cant);
    }
}

static void imprimir_ampliada(double M[MAXN][MAXN], double b[MAXN]) {
    for (int i = 0; i < N; i++) {
        P("   [");
        for (int j = 0; j < N; j++) P(" %14.8f", M[i][j]);
        P("  | %14.8f ]\n", b[i]);
    }
}

static void imprimir_matriz(const char *nombre, double M[MAXN][MAXN]) {
    P("%s =\n", nombre);
    for (int i = 0; i < N; i++) {
        P("   [");
        for (int j = 0; j < N; j++) P(" %14.8f", M[i][j]);
        P(" ]\n");
    }
}

static void residuo(double x[MAXN]) {
    double r2 = 0, rmax = 0;
    P("Verificacion  r = A x - b :\n");
    for (int i = 0; i < N; i++) {
        double s = -B[i];
        for (int j = 0; j < N; j++) s += A[i][j] * x[j];
        P("   fila %d:  %.3e\n", i + 1, s);
        r2 += s * s;
        if (fabs(s) > rmax) rmax = fabs(s);
    }
    P("||Ax - b||_2 = %.3e    max|Ax - b| = %.3e\n", sqrt(r2), rmax);
}

static void cargar(void) {
    P("\n=== CARGAR SISTEMA ===\n");
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   n = cantidad de ecuaciones (y de incognitas)\n");
    P("   A = matriz de coeficientes ;  b = terminos independientes ;  x = incognitas\n");
    P("   [A|b] = matriz ampliada: coeficientes y, a la derecha de la barra, el termino b\n");
    P("   a_ij = coeficiente de la fila i columna j   (los a_ii son la DIAGONAL)\n");
    do { N = leer_int("Cantidad de ecuaciones n  (= cantidad de incognitas, 2 a 20): "); } while (N < 2 || N > MAXN);
    P("Escribi cada fila: los %d coeficientes y al final el termino independiente b, separados por ESPACIOS.\n", N);
    P("Ejemplo para  4x1 + x2 - x6 = 0  en un 6x6:   4 1 0 0 0 -1 0\n");
    P("(Si una variable no aparece, su coeficiente es 0.)\n");
    for (int i = 0; i < N; i++) {
        char msg[64];
        double v[MAXN + 1];
        sprintf(msg, "Fila %d  (los %d coeficientes y al final el termino b): ", i + 1, N);
        leer_valores(msg, v, N + 1, 0);
        for (int j = 0; j < N; j++) A[i][j] = v[j];
        B[i] = v[N];
    }
    P("\nSistema cargado [A | b]:\n");
    imprimir_ampliada(A, B);
}

/* ------------------------------------------------------------------ */
/* Radio espectral por ||T^k||^(1/k) (Gelfand), vale aunque haya autovalores complejos */
static double radio_espectral(double T[MAXN][MAXN]) {
    double M[MAXN][MAXN], R[MAXN][MAXN], logc = 0;
    int i, j, k, it;
    for (i = 0; i < N; i++) for (j = 0; j < N; j++) M[i][j] = T[i][j];
    double pot = 1;
    for (it = 0; it < 11; it++) {
        double nrm = 0;
        for (i = 0; i < N; i++) for (j = 0; j < N; j++) {
            double s = 0;
            for (k = 0; k < N; k++) s += M[i][k] * M[k][j];
            R[i][j] = s;
        }
        for (i = 0; i < N; i++) {
            double fila = 0;
            for (j = 0; j < N; j++) fila += fabs(R[i][j]);
            if (fila > nrm) nrm = fila;
        }
        pot *= 2;
        if (nrm == 0) return 0;
        logc = 2 * logc + log(nrm);
        for (i = 0; i < N; i++) for (j = 0; j < N; j++) M[i][j] = R[i][j] / nrm;
    }
    return exp(logc / pot);
}

static void analizar(void) {
    int i, j;
    P("\n=== ANALISIS DEL SISTEMA ===\n");
    imprimir_ampliada(A, B);
    P("\nDiagonal dominante por filas:  |a_ii|  vs  suma de |a_ij| de la fila (j distinto de i)\n");
    int estricta = 1, debil = 1, cero_diag = 0;
    for (i = 0; i < N; i++) {
        double s = 0;
        for (j = 0; j < N; j++) if (j != i) s += fabs(A[i][j]);
        double d = fabs(A[i][i]);
        if (d == 0) cero_diag = 1;
        const char *rel = d > s ? ">" : (d == s ? "=" : "<");
        P("   fila %d:  |%.6g| %s %.6g  %s\n", i + 1, A[i][i], rel, s, d > s ? "OK" : "NO cumple estricta");
        if (!(d > s)) estricta = 0;
        if (d < s) debil = 0;
    }
    if (estricta)
        P("=> ES diagonal dominante estricta: Jacobi y Gauss-Seidel CONVERGEN para cualquier x0 (condicion suficiente).\n");
    else if (debil)
        P("=> Diagonal dominante solo DEBIL (hay filas con =): no alcanza para garantizar convergencia, mirar el radio espectral.\n");
    else
        P("=> NO es diagonal dominante. Eso NO significa que diverja (es condicion suficiente, no necesaria):\n"
          "   mirar el radio espectral o probar reordenar filas (opcion 9).\n");
    if (cero_diag) { P("Hay un CERO en la diagonal: Jacobi/GS no se pueden aplicar asi. Reordena filas.\n"); return; }

    /* Matrices de iteracion */
    double TJ[MAXN][MAXN], TG[MAXN][MAXN];
    for (i = 0; i < N; i++) for (j = 0; j < N; j++) TJ[i][j] = (i == j) ? 0 : -A[i][j] / A[i][i];
    /* T_GS = -(D+L)^-1 U : columna j = resolver (D+L) y = -U e_j */
    for (j = 0; j < N; j++) {
        double y[MAXN];
        for (i = 0; i < N; i++) {
            double s = (j > i) ? -A[i][j] : 0;
            for (int k = 0; k < i; k++) s -= A[i][k] * y[k];
            y[i] = s / A[i][i];
        }
        for (i = 0; i < N; i++) TG[i][j] = y[i];
    }
    double rj = radio_espectral(TJ), rg = radio_espectral(TG);
    P("\nRadio espectral de la matriz de iteracion (converge si y solo si rho < 1):\n");
    P("   Jacobi       T = -D^-1 (L+U)     rho = %.6f  -> %s\n", rj, rj < 1 ? "CONVERGE" : "NO converge");
    P("   Gauss-Seidel T = -(D+L)^-1 U     rho = %.6f  -> %s\n", rg, rg < 1 ? "CONVERGE" : "NO converge");
    if (rg < 1) P("   (mientras mas chico rho, mas rapido converge)\n");

    hoja_reset();
    H("JUSTIFICACION DE LA CONVERGENCIA DE LOS METODOS ITERATIVOS\n\n");
    H("Sistema A x = b:\n");
    for (i = 0; i < N; i++) {
        H("   [");
        for (j = 0; j < N; j++) H(" %10.6g", A[i][j]);
        H("  | %10.6g ]\n", B[i]);
    }
    H("\n1) DIAGONAL DOMINANTE (se compara |a_ii| con la suma de los otros terminos de la fila):\n");
    for (i = 0; i < N; i++) {
        double s = 0;
        for (j = 0; j < N; j++) if (j != i) s += fabs(A[i][j]);
        H("   fila %d:  |%.6g| %s %.6g   %s\n", i + 1, A[i][i],
          fabs(A[i][i]) > s ? ">" : (fabs(A[i][i]) == s ? "=" : "<"), s,
          fabs(A[i][i]) > s ? "cumple" : "NO cumple");
    }
    if (estricta)
        H("   => La matriz ES diagonal dominante estricta. Esa es condicion SUFICIENTE, asi que\n"
          "      Jacobi y Gauss-Seidel convergen para cualquier vector inicial x0.\n\n");
    else
        H("   => La matriz NO es diagonal dominante estricta. Como esa condicion es suficiente pero\n"
          "      NO necesaria, esto no permite afirmar que el metodo diverja: hay que analizar el\n"
          "      radio espectral.\n\n");
    H("2) RADIO ESPECTRAL de la matriz de iteracion (condicion NECESARIA Y SUFICIENTE: rho < 1):\n");
    H("   Jacobi:        T = -D^-1 (L+U)      rho(T) = %.6f   ->  %s\n", rj, rj < 1 ? "converge" : "NO converge");
    H("   Gauss-Seidel:  T = -(D+L)^-1 U      rho(T) = %.6f   ->  %s\n\n", rg, rg < 1 ? "converge" : "NO converge");
    H("CONCLUSION: %s\n",
      (rg < 1 && rj < 1) ? "ambos metodos convergen, y Gauss-Seidel lo hace mas rapido porque su radio\nespectral es menor (cuanto menor es rho, mas rapido decrece el error)."
      : (rg < 1) ? "Gauss-Seidel converge, pero Jacobi no (su radio espectral es mayor o igual que 1)."
      : (rj < 1) ? "Jacobi converge, pero Gauss-Seidel no."
      : "ninguno de los dos converge con este ordenamiento de filas: conviene reordenar las filas\n(opcion 9) o resolver el sistema con un metodo directo como Gauss.");
    hoja_que_escribir("La tabla de diagonal dominante fila por fila", "El radio espectral de Jacobi y de Gauss-Seidel", "La conclusion: si converge, y cual metodo es mas rapido", (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void gauss(int piv) {
    double M[MAXN][MAXN], b[MAXN], x[MAXN];
    int i, j, k, swaps = 0;
    for (i = 0; i < N; i++) { for (j = 0; j < N; j++) M[i][j] = A[i][j]; b[i] = B[i]; }
    P("\n=== ELIMINACION DE GAUSS %s ===\n", piv ? "CON PIVOTEO PARCIAL" : "SIN PIVOTEO");
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   pivote = elemento de la diagonal con el que se hacen los ceros de esa columna\n");
    P("   m_ik = multiplicador = a_ik / a_kk ;   se aplica como  Fi <- Fi - m_ik * Fk\n");
    P("   Fi = fila i ;  U = matriz triangulada (ceros debajo de la diagonal)\n");
    P("   det(A) = producto de la diagonal de U, con un cambio de signo por intercambio\n");
    P("Matriz ampliada inicial:\n");
    imprimir_ampliada(M, b);
    hoja_reset();
    H("METODO DE ELIMINACION DE GAUSS %s\n\n", piv ? "CON PIVOTEO PARCIAL" : "(sin pivoteo)");
    H("Sistema A x = b.  Matriz ampliada [A|b]:\n");
    for (i = 0; i < N; i++) {
        H("   [");
        for (j = 0; j < N; j++) H(" %10.6g", M[i][j]);
        H("  | %10.6g ]\n", b[i]);
    }
    if (fabs(M[0][0]) < 1e-14)
        H("\nComo a11 = 0, ES NECESARIO pivotear (no se puede dividir por cero).\n");
    H("\nSe triangula haciendo ceros debajo de la diagonal con  Fi <- Fi - m_ik * Fk,\n");
    H("donde  m_ik = a_ik / a_kk.\n\n");
    for (k = 0; k < N - 1; k++) {
        if (piv) {
            int p = k;
            for (i = k + 1; i < N; i++) if (fabs(M[i][k]) > fabs(M[p][k])) p = i;
            if (p != k) {
                for (j = 0; j < N; j++) { double t = M[k][j]; M[k][j] = M[p][j]; M[p][j] = t; }
                double t = b[k]; b[k] = b[p]; b[p] = t;
                swaps++;
                P("\nPIVOTEO: en la columna %d el mayor |a| esta en la fila %d -> intercambio fila %d <-> fila %d\n", k + 1, p + 1, k + 1, p + 1);
                H("Pivoteo: en la columna %d el mayor valor absoluto esta en la fila %d,\n", k + 1, p + 1);
                H("         por eso se intercambian F%d <-> F%d (el determinante cambia de signo).\n", k + 1, p + 1);
            }
        }
        if (fabs(M[k][k]) < 1e-14) {
            if (!piv) P("\nPIVOTE NULO: a%d%d = 0. Sin pivoteo no se puede seguir (division por cero) -> usar Gauss CON pivoteo.\n", k + 1, k + 1);
            else P("\nToda la columna %d es cero desde la fila %d: la matriz es SINGULAR (det = 0).\n", k + 1, k + 1);
            return;
        }
        if (!piv && fabs(M[k][k]) < 1e-3) P("(Pivote muy chico: conviene pivotear para no amplificar errores de redondeo)\n");
        P("\nPaso %d  (pivote a%d%d = %.10f):\n", k + 1, k + 1, k + 1, M[k][k]);
        H("Paso %d  (pivote a%d%d = %.6g):\n", k + 1, k + 1, k + 1, M[k][k]);
        for (i = k + 1; i < N; i++) {
            double m = M[i][k] / M[k][k];
            P("   m%d%d = %.8f / %.8f = %.10f     F%d <- F%d - m%d%d * F%d\n", i + 1, k + 1, M[i][k], M[k][k], m, i + 1, i + 1, i + 1, k + 1, k + 1);
            H("   m%d%d = %.6g / %.6g = %.8f      F%d <- F%d - m%d%d * F%d\n", i + 1, k + 1, M[i][k], M[k][k], m, i + 1, i + 1, i + 1, k + 1, k + 1);
            if (k == 0) {
                H("      elemento por elemento:\n");
                for (j = k; j < N; j++)
                    H("         a%d%d:  %.6g - (%.8f)*(%.6g) = %.6g\n", i + 1, j + 1, M[i][j], m, M[k][j], M[i][j] - m * M[k][j]);
                H("         b%d :  %.6g - (%.8f)*(%.6g) = %.6g\n", i + 1, b[i], m, b[k], b[i] - m * b[k]);
            }
            for (j = k; j < N; j++) M[i][j] -= m * M[k][j];
            M[i][k] = 0.0;
            b[i] -= m * b[k];
        }
        imprimir_ampliada(M, b);
    }
    if (fabs(M[N - 1][N - 1]) < 1e-14) { P("\nUltimo pivote = 0: matriz SINGULAR.\n"); return; }
    H("\nMatriz triangulada [U|b']:\n");
    for (i = 0; i < N; i++) {
        H("   [");
        for (j = 0; j < N; j++) H(" %10.6g", M[i][j]);
        H("  | %10.6g ]\n", b[i]);
    }
    H("\nSustitucion hacia atras:  x_i = ( b_i - suma_(j>i) a_ij * x_j ) / a_ii\n");
    P("\nSustitucion hacia atras:  x_i = (b_i - suma_{j>i} a_ij x_j) / a_ii\n");
    for (i = N - 1; i >= 0; i--) {
        double s = 0;
        for (j = i + 1; j < N; j++) s += M[i][j] * x[j];
        x[i] = (b[i] - s) / M[i][i];
        P("   x%d = (%.10f - (%.10f)) / %.10f = %.10f\n", i + 1, b[i], s, M[i][i], x[i]);
        H("   x%d = ( %.6g - (%.6g) ) / %.6g = %.10f\n", i + 1, b[i], s, M[i][i], x[i]);
    }
    double det = (swaps % 2) ? -1 : 1;
    for (i = 0; i < N; i++) det *= M[i][i];
    linea();
    P("SOLUCION:\n");
    for (i = 0; i < N; i++) P("   x%d = %.10f\n", i + 1, x[i]);
    P("det(A) = %s producto de la diagonal = %.10f   (intercambios de filas: %d)\n", (swaps % 2) ? "-" : "+", det, swaps);
    residuo(x);
    linea();
    H("\nSOLUCION:\n");
    for (i = 0; i < N; i++) H("   x%d = %.10f\n", i + 1, x[i]);
    H("\ndet(A) = (-1)^%d * (producto de la diagonal de U) = %.10f\n", swaps, det);
    if (piv && swaps == 0) H("No hizo falta intercambiar filas: los pivotes ya eran los de mayor valor absoluto.\n");
    H("\nVerificacion (reemplazando en las ecuaciones originales): A x - b = 0 en todas las filas.\n");
    hoja_que_escribir("La matriz ampliada [A|b]", "Si hace falta pivotear y por que (pivote cero o muy chico) o por que basta la eliminacion simple", "Cada paso: los multiplicadores m_ik y la matriz que queda", "La sustitucion hacia atras con los numeros", "La solucion y el determinante", "La verificacion: reemplazar la solucion en las ecuaciones originales", (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void lu(void) {
    double L[MAXN][MAXN], U[MAXN][MAXN], y[MAXN], x[MAXN];
    int i, j, k;
    P("\n=== FACTORIZACION LU (Doolittle: L con unos en la diagonal, sin pivoteo) ===\n");
    for (i = 0; i < N; i++) for (j = 0; j < N; j++) { U[i][j] = A[i][j]; L[i][j] = (i == j) ? 1 : 0; }
    for (k = 0; k < N - 1; k++) {
        if (fabs(U[k][k]) < 1e-14) { P("Pivote nulo en U%d%d: LU sin pivoteo no se puede. Reordena filas (opcion 9).\n", k + 1, k + 1); return; }
        for (i = k + 1; i < N; i++) {
            double m = U[i][k] / U[k][k];
            L[i][k] = m;
            for (j = k; j < N; j++) U[i][j] -= m * U[k][j];
            U[i][k] = 0.0;
        }
    }
    imprimir_matriz("L", L);
    imprimir_matriz("U", U);
    P("\n1) L y = b  (sustitucion hacia adelante):\n");
    for (i = 0; i < N; i++) {
        double s = B[i];
        for (j = 0; j < i; j++) s -= L[i][j] * y[j];
        y[i] = s;
        P("   y%d = %.10f\n", i + 1, y[i]);
    }
    P("2) U x = y  (sustitucion hacia atras):\n");
    for (i = N - 1; i >= 0; i--) {
        double s = y[i];
        for (j = i + 1; j < N; j++) s -= U[i][j] * x[j];
        if (fabs(U[i][i]) < 1e-14) { P("U%d%d = 0: matriz singular.\n", i + 1, i + 1); return; }
        x[i] = s / U[i][i];
    }
    linea();
    double det = 1;
    for (i = 0; i < N; i++) { P("   x%d = %.10f\n", i + 1, x[i]); det *= U[i][i]; }
    P("det(A) = producto de la diagonal de U = %.10f\n", det);
    residuo(x);
    linea();
}

/* ------------------------------------------------------------------ */
static int CRIT_IT;
static double TOL_IT;
static int NMAX_IT;

static void pedir_criterio_it(void) {
    P("\nCriterio de corte:\n");
    P("  1) max |x_i nuevo - x_i viejo| < tol       (error absoluto, norma infinito)\n");
    P("  2) ||x nuevo - x viejo||_2 < tol           (error absoluto, norma euclidea)\n");
    P("  3) error IUA en %% (el mayor de las componentes) < tol\n");
    P("  4) Cantidad FIJA de iteraciones\n");
    do { CRIT_IT = leer_int("Opcion (1-4): "); } while (CRIT_IT < 1 || CRIT_IT > 4);
    if (CRIT_IT == 4) { NMAX_IT = leer_int("Cantidad de iteraciones a realizar  (ej: 10): "); TOL_IT = -1; }
    else { TOL_IT = leer_num("Tolerancia  (error maximo admitido, ej: 1e-4): "); NMAX_IT = leer_int("Maximo de iteraciones  (tope por si no converge, ej: 200): "); }
    {
        char msg[160];
        sprintf(msg, "Solucion EXACTA, si el enunciado la da o si la sacaste por Gauss (%d valores; ENTER = no): ", N);
        HAYEXACTA = leer_valores(msg, XEXACTA, N, 1);
    }
}

/* tipo: 0 Jacobi, 1 Gauss-Seidel, 2 SOR. Devuelve iteraciones si converge, -1 si no. */
static int iterar(int tipo, double w, double x0[MAXN], int verbose) {
    double x[MAXN], xn[MAXN];
    int i, j, k;
    for (i = 0; i < N; i++) {
        x[i] = x0[i];
        if (A[i][i] == 0) { if (verbose) P("a%d%d = 0 en la diagonal: reordena filas (opcion 9).\n", i + 1, i + 1); return -1; }
    }
    if (verbose) {
        P("\n   k |");
        for (j = 0; j < N; j++) P("      x%-2d      |", j + 1);
        P(" max|dx|    | ||dx||_2   | e IUA%% max\n");
        P("   0 |");
        for (j = 0; j < N; j++) P(" %13.8f |", x[j]);
        P("\n");
    }
    double dinf = 0, d2 = 0, eiua = 0;
    for (k = 1; k <= NMAX_IT; k++) {
            if (verbose && k == 1) { P("  (a mano, iteracion 1)\n"); H("Iteracion 1:\n"); }
        for (i = 0; i < N; i++) {
            double s = 0;
            for (j = 0; j < N; j++) {
                if (j == i) continue;
                double xj = (tipo == 0 || j > i) ? x[j] : xn[j];
                s += A[i][j] * xj;
            }
            double gs = (B[i] - s) / A[i][i];
            xn[i] = (tipo == 2) ? w * gs + (1 - w) * x[i] : gs;
            if (verbose && k == 1) {
                P("     x%d = (b%d - suma a%dj*xj) / a%d%d = (%.6f - (%.6f)) / %.6f = %.10f", i + 1, i + 1, i + 1, i + 1, i + 1, B[i], s, A[i][i], gs);
                if (tipo == 2) P("  -> SOR: w*%.8f + (1-w)*%.8f = %.10f", gs, x[i], xn[i]);
                P("\n");
                {
                    char det[700];
                    int o = sprintf(det, "( %.6g", B[i]);
                    for (int jj = 0; jj < N; jj++) {
                        if (jj == i || A[i][jj] == 0) continue;
                        double xj = (tipo == 0 || jj > i) ? x[jj] : xn[jj];
                        o += sprintf(det + o, " - (%.6g)*(%.6g)", A[i][jj], xj);
                    }
                    sprintf(det + o, " ) / %.6g", A[i][i]);
                    H("   x%d = %s = %.10f\n", i + 1, det, gs);
                }
                if (tipo == 2) H("        SOR: %.4g * %.8f + (1 - %.4g) * %.8f = %.10f\n", w, gs, w, x[i], xn[i]);
            }
        }
        dinf = 0; d2 = 0; eiua = 0;
        for (i = 0; i < N; i++) {
            double d = fabs(xn[i] - x[i]);
            if (d > dinf) dinf = d;
            d2 += d * d;
            double e = eps_iua(xn[i], x[i]);
            if (e > eiua) eiua = e;
        }
        d2 = sqrt(d2);
        for (i = 0; i < N; i++) x[i] = xn[i];
        if (verbose) {
            P("%4d |", k);
            for (j = 0; j < N; j++) P(" %13.8f |", x[j]);
            P(" %.3e | %.3e | %.3e\n", dinf, d2, eiua);
            if (k == 1) {
                H("\nTabla de iteraciones:\n   k |");
                for (j = 0; j < N; j++) H("      x%-2d      |", j + 1);
                if (HAYEXACTA) H("   max|dx|  |  ||dx||_2 |  eps %%   | err exacto\n");
            else H("   max|dx|  |  ||dx||_2 |  eps %%\n");
            }
            H("%4d |", k);
            for (j = 0; j < N; j++) H(" %13.8f |", x[j]);
            if (HAYEXACTA) {
                double ee = 0;
                for (int q = 0; q < N; q++) { double d = fabs(x[q] - XEXACTA[q]); if (d > ee) ee = d; }
                H(" %.3e | %.3e | %.3e | %.3e\n", dinf, d2, eiua, ee);
            } else H(" %.3e | %.3e | %.3e\n", dinf, d2, eiua);
        }
        if (!(dinf < 1e12)) { if (verbose) P("DIVERGE: los valores crecen sin limite.\n"); return -1; }
        int corta = 0;
        if (CRIT_IT == 1) corta = dinf < TOL_IT;
        if (CRIT_IT == 2) corta = d2 < TOL_IT;
        if (CRIT_IT == 3) corta = eiua < TOL_IT;
        if (CRIT_IT == 4) corta = (k >= NMAX_IT);
        if (corta) break;
    }
    int conv = (k <= NMAX_IT);
    if (k > NMAX_IT) k = NMAX_IT;
    if (verbose) {
        linea();
        if (!conv) P("ATENCION: se hicieron %d iteraciones SIN cumplir la tolerancia.\n", k);
        P("SOLUCION (%s, %d iteraciones):\n", tipo == 0 ? "Jacobi" : (tipo == 1 ? "Gauss-Seidel" : "SOR"), k);
        for (i = 0; i < N; i++) P("   x%d = %.10f\n", i + 1, x[i]);
        P("Ultimo error: max|dx| = %.4e   ||dx||_2 = %.4e   e IUA = %.4e %%\n", dinf, d2, eiua);
        if (HAYEXACTA) {
            double ee = 0;
            for (i = 0; i < N; i++) { double d = fabs(x[i] - XEXACTA[i]); if (d > ee) ee = d; }
            P("Error absoluto EXACTO (contra la solucion del enunciado): %.6e\n", ee);
        }
        residuo(x);
        linea();
        H("\nRESULTADO tras %d iteracion%s:\n", k, k == 1 ? "" : "es");
        for (i = 0; i < N; i++) H("   x%d = %.10f\n", i + 1, x[i]);
        H("\n   ERRORES de la ultima iteracion (todos son ESTIMADOS: comparan x(k+1) con x(k)):\n");
        H("   1) max |x_i(k+1) - x_i(k)| = %.4e     (norma infinito: el mayor cambio entre componentes)\n", dinf);
        H("   2) ||x(k+1) - x(k)||_2    = %.4e     (norma euclidea: raiz de la suma de los cuadrados)\n", d2);
        H("   3) error porcentual maximo = %.4e %%   (formula # aplicada componente por componente)\n", eiua);
        H("   4) El residuo ||A x - b|| que figura abajo NO es el error de x: es la VERIFICACION\n");
        H("      de que la solucion cumple las ecuaciones.\n");
        H("      (El error EXACTO |x - x_verdadero| solo se puede calcular si se conoce la\n");
        H("      solucion exacta; se la puede obtener resolviendo el sistema por Gauss.)\n");
        if (!conv) H("   ATENCION: se corto por maximo de iteraciones sin alcanzar la tolerancia.\n");
        H("   Verificacion: al reemplazar en A x = b el residuo es practicamente cero.\n");
        hoja_que_escribir("Si la matriz es diagonal dominante (fila por fila) o si hubo que reordenar filas", "Los despejes de cada incognita", "La 1ra iteracion con los numeros", "El criterio de corte y la NORMA usada (max|dx| o ||dx||_2)", "La solucion con 10 decimales, la cantidad de iteraciones y la tabla o la captura", "Los errores: el estimado y, si se conoce la solucion, el exacto", (const char *)0);
        hoja_mostrar();
    }
    return conv ? k : -1;
}

static int PERM[MAXN], USADA[MAXN];

static int buscar_dd(int pos) {       /* backtracking: fila para cada posicion */
    if (pos == N) return 1;
    for (int r = 0; r < N; r++) {
        if (USADA[r]) continue;
        double s = 0;
        for (int j = 0; j < N; j++) if (j != pos) s += fabs(A[r][j]);
        if (fabs(A[r][pos]) > s) {
            USADA[r] = 1; PERM[pos] = r;
            if (buscar_dd(pos + 1)) return 1;
            USADA[r] = 0;
        }
    }
    return 0;
}


/* Antes de iterar: si hay un 0 en la diagonal (o no es diagonal dominante) avisa y reordena solo */
static void preparar_diagonal(void) {
    int i, j, hay_cero = 0, dd = 1;
    for (i = 0; i < N; i++) {
        double s2 = 0;
        if (A[i][i] == 0) hay_cero = 1;
        for (j = 0; j < N; j++) if (j != i) s2 += fabs(A[i][j]);
        if (fabs(A[i][i]) <= s2) dd = 0;
    }
    if (dd) return;
    P("\n*****************************************************************************\n");
    if (hay_cero) P("ATENCION: hay un CERO en la diagonal. Asi como esta, el metodo NO se puede usar\n(habria que dividir por a_ii = 0).\n");
    else P("ATENCION: la matriz, en el orden cargado, NO es diagonal dominante estricta.\n");
    for (i = 0; i < N; i++) USADA[i] = 0;
    if (!buscar_dd(0)) {
        P("No existe un orden de filas diagonal dominante. Se sigue con el orden cargado;\nmira la opcion 2 (radio espectral) para saber si converge.\n");
        P("*****************************************************************************\n");
        return;
    }
    double TA[MAXN][MAXN], TB[MAXN];
    P("SE REORDENAN LAS FILAS AUTOMATICAMENTE (fila nueva <- fila original): ");
    for (i = 0; i < N; i++) P(" %d<-%d ", i + 1, PERM[i] + 1);
    P("\n");
    for (i = 0; i < N; i++) { for (j = 0; j < N; j++) TA[i][j] = A[PERM[i]][j]; TB[i] = B[PERM[i]]; }
    for (i = 0; i < N; i++) { for (j = 0; j < N; j++) A[i][j] = TA[i][j]; B[i] = TB[i]; }
    P("Sistema reordenado (reordenar filas NO cambia la solucion) y ahora diagonal dominante:\n");
    imprimir_ampliada(A, B);
    P("EN LA HOJA: 'Como a_ii = 0 / no es diagonal dominante, se intercambian filas; con el nuevo\n");
    P("orden la matriz es diagonal dominante estricta y el metodo converge para cualquier x0.'\n");
    P("*****************************************************************************\n");
}

static void iterativo(int tipo) {
    double x0[MAXN], w = 1;
    const char *nom[] = {"JACOBI", "GAUSS-SEIDEL", "SOR"};
    P("\n=== %s ===\n", nom[tipo]);
    preparar_diagonal();
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   x(k) = vector de la iteracion k ;  x0 = vector inicial para arrancar\n");
    P("   x_i = componente i del vector (la incognita numero i)\n");
    P("   max|dx| = mayor cambio entre una iteracion y la anterior (norma infinito)\n");
    P("   ||dx||_2 = raiz de la suma de los cuadrados de esos cambios (norma euclidea)\n");
    P("   residuo ||Ax - b|| = verificacion de que la solucion cumple las ecuaciones\n");
    if (tipo == 0) P("x_i(k+1) = ( b_i - suma_{j!=i} a_ij x_j(k) ) / a_ii     (usa SOLO valores de la iteracion anterior)\n");
    else P("x_i(k+1) = ( b_i - suma_{j<i} a_ij x_j(k+1) - suma_{j>i} a_ij x_j(k) ) / a_ii   (usa los valores recien calculados)\n");
    if (tipo == 2) P("SOR: x_i(k+1) = w * [valor de Gauss-Seidel] + (1 - w) * x_i(k).  w=1 es Gauss-Seidel. 1<w<2 sobre-relajacion.\n");
    char msg[128];
    sprintf(msg, "x0  (vector inicial: %d valores separados por espacio; ENTER = todos ceros): ", N);
    if (!leer_valores(msg, x0, N, 1)) for (int i = 0; i < N; i++) x0[i] = 0;
    int barrido = 0;
    if (tipo == 2) {
        int hay;
        w = leer_num_opcional("omega  (factor de relajacion, entre 0 y 2; ENTER = barrido para buscar el optimo): ", &hay);
        barrido = !hay;
    }
    pedir_criterio_it();
    hoja_reset();
    H("METODO DE %s\n\n", nom[tipo]);
    H("Sistema A x = b:\n");
    {
        int i, j, estricta = 1;
        for (i = 0; i < N; i++) {
            H("   [");
            for (j = 0; j < N; j++) H(" %10.6g", A[i][j]);
            H("  | %10.6g ]\n", B[i]);
        }
        H("\nAnalisis de diagonal dominante (|a_ii| vs suma de |a_ij| con j distinto de i):\n");
        for (i = 0; i < N; i++) {
            double s = 0;
            for (j = 0; j < N; j++) if (j != i) s += fabs(A[i][j]);
            H("   fila %d:  |%.6g| %s %.6g\n", i + 1, A[i][i], fabs(A[i][i]) > s ? ">" : "<=", s);
            if (!(fabs(A[i][i]) > s)) estricta = 0;
        }
        H(estricta
          ? "=> La matriz ES diagonal dominante estricta, por lo tanto el metodo converge\n   para cualquier vector inicial (condicion suficiente).\n"
          : "=> NO es diagonal dominante estricta: esa condicion suficiente no se cumple.\n   Conviene reordenar filas o justificar con el radio espectral (opcion 2 del menu).\n");
        H("\nDespejando cada incognita de su ecuacion:\n");
        for (i = 0; i < N; i++) {
            H("   x%d = ( %.6g", i + 1, B[i]);
            for (j = 0; j < N; j++) if (j != i && A[i][j] != 0) H(" %s %.6g*x%d", A[i][j] > 0 ? "-" : "+", fabs(A[i][j]), j + 1);
            H(" ) / %.6g\n", A[i][i]);
        }
        if (tipo == 0) H("(Jacobi: todos los x_j son valores de la iteracion anterior.)\n");
        else H("(Gauss-Seidel: cada x_j recien calculado se usa enseguida en la ecuacion siguiente.)\n");
        if (tipo == 2) H("(SOR: x_i = w * [valor de Gauss-Seidel] + (1 - w) * x_i anterior, con w = %.4g.)\n", w);
        H("\nVector inicial x0 = (");
        for (i = 0; i < N; i++) H("%s%.6g", i ? " , " : "", x0[i]);
        H(")\n");
        if (CRIT_IT == 4) H("Criterio de corte: realizar %d iteraciones.\n\n", NMAX_IT);
        else H("Criterio de corte: %s < %g\n\n",
               CRIT_IT == 1 ? "max|x_i(k+1) - x_i(k)|" : (CRIT_IT == 2 ? "||x(k+1) - x(k)||_2" : "error porcentual (formula #)"), TOL_IT);
    }
    if (!barrido) { iterar(tipo, w, x0, 1); return; }
    if (CRIT_IT == 4) { P("Para el barrido hace falta una tolerancia (criterio 1, 2 o 3).\n"); return; }
    P("\n  omega | iteraciones\n");
    double wbest = 1;
    int best = -1;
    for (int i = 1; i <= 19; i++) {
        double ww = 0.1 * i;
        int it = iterar(2, ww, x0, 0);
        if (it < 0) P("   %.1f  | no converge en %d\n", ww, NMAX_IT);
        else P("   %.1f  | %d\n", ww, it);
        if (it > 0 && (best < 0 || it < best)) { best = it; wbest = ww; }
    }
    if (best < 0) { P("Ningun omega convergio.\n"); return; }
    P("omega optimo (paso 0.1) = %.1f con %d iteraciones. Detalle:\n", wbest, best);
    iterar(2, wbest, x0, 1);
}

/* ------------------------------------------------------------------ */
static void reordenar(void) {
    P("\n=== REORDENAR FILAS ===\n");
    P("  1) Buscar automaticamente un orden de filas que sea diagonal dominante\n");
    P("  2) Intercambiar dos filas a mano\n");
    int op = leer_int("Opcion: ");
    double TA[MAXN][MAXN], TB[MAXN];
    int i, j;
    if (op == 1) {
        for (i = 0; i < N; i++) USADA[i] = 0;
        if (!buscar_dd(0)) { P("No existe ningun orden de filas que haga la matriz diagonal dominante estricta.\n"); return; }
        P("Orden encontrado (fila nueva <- fila original): ");
        for (i = 0; i < N; i++) P(" %d<-%d ", i + 1, PERM[i] + 1);
        P("\n");
        for (i = 0; i < N; i++) { for (j = 0; j < N; j++) TA[i][j] = A[PERM[i]][j]; TB[i] = B[PERM[i]]; }
        for (i = 0; i < N; i++) { for (j = 0; j < N; j++) A[i][j] = TA[i][j]; B[i] = TB[i]; }
    } else if (op == 2) {
        int f1 = leer_int("Fila 1  (numero de la primera fila a intercambiar): ") - 1, f2 = leer_int("Fila 2  (numero de la segunda fila): ") - 1;
        if (f1 < 0 || f2 < 0 || f1 >= N || f2 >= N) { P("Fila invalida.\n"); return; }
        for (j = 0; j < N; j++) { double t = A[f1][j]; A[f1][j] = A[f2][j]; A[f2][j] = t; }
        double t = B[f1]; B[f1] = B[f2]; B[f2] = t;
    } else return;
    P("Sistema reordenado (reordenar filas NO cambia la solucion):\n");
    imprimir_ampliada(A, B);
}

static void verificar(void) {
    double x[MAXN];
    char msg[128];
    sprintf(msg, "Escribi los %d valores de x  (la solucion que queres verificar): ", N);
    leer_valores(msg, x, N, 0);
    residuo(x);
}

int main(void) {
    abrir_log("salida_sistemas.txt");
    P("\n############  SISTEMAS DE ECUACIONES LINEALES  A x = b   (Guias 3 y 4)  ############\n");
    cargar();
    for (;;) {
        P("\n======================== MENU (sistema %dx%d) ========================\n", N, N);
        P("  OPCION                          USALA SI EL ENUNCIADO DICE...\n");
        P("  1) Cargar otro sistema\n");
        P("  2) Analizar                     'es posible usar Gauss-Seidel?', 'converge?', 'diagonal dominante'\n");
        P("  3) Gauss SIN pivoteo            'eliminacion simple', 'sin pivoteo', 'basta con eliminacion simple?'\n");
        P("  4) Gauss CON pivoteo            'eliminacion gaussiana', 'pivoteo parcial', 'solucion exacta', 'determinante'\n");
        P("  5) Factorizacion LU             'LU', 'varios vectores b con la misma matriz'\n");
        P("  6) Jacobi                       'Jacobi'\n");
        P("  7) Gauss-Seidel                 'Gauss-Seidel', 'error absoluto de 10^-4', 'primera iteracion'\n");
        P("  8) SOR / barrido de omega       'SOR', 'relajacion', 'omega optimo', 'acelerar la convergencia'\n");
        P("  9) Reordenar filas              'intercambio de filas', hay un 0 en la diagonal\n");
        P(" 10) Verificar una solucion       'verifique', residuo A x - b\n");
        P("  0) Salir\n");
        int op = leer_int("Opcion: ");
        if (op == 0) break;
        switch (op) {
            case 1: cargar(); break;
            case 2: analizar(); break;
            case 3: gauss(0); break;
            case 4: gauss(1); break;
            case 5: lu(); break;
            case 6: iterativo(0); break;
            case 7: iterativo(1); break;
            case 8: iterativo(2); break;
            case 9: reordenar(); break;
            case 10: verificar(); break;
            default: P("Opcion invalida.\n");
        }
        if (op >= 1 && op <= 10) pausa();
    }
    if (LOG_MN) fclose(LOG_MN);
    return 0;
}
