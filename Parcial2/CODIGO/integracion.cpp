/* integracion.cpp -- INTEGRACION NUMERICA   (Guias 7 y 8)
 *
 *  Trapecio (simple y compuesto), Simpson 1/3, Simpson 3/8, combinacion de
 *  Simpson cuando n es impar, cuadratura de Gauss-Legendre (1 a 6 puntos),
 *  integracion de tablas de datos, comparacion de metodos, formulas de error
 *  teorico (con derivadas calculadas por el programa) y busqueda del n
 *  necesario para una tolerancia dada.
 *
 *  Todo lo que se imprime queda en salida_integracion.txt
 */
#include "mn_comun.h"

#define MAXINT_NODOS 20001   /* el parcial puede pedir n = 1000 o mas */
static double XS[MAXINT_NODOS], FS[MAXINT_NODOS];

static char F[256] = "";          /* f(x) del enunciado (modo funcion) */
static int MODO = 0;              /* 0 = funcion f(x) ,  1 = tabla de datos */
static int NP = 0;                /* cantidad de puntos de la tabla */
static double X[MAXPUNTOS], Y[MAXPUNTOS];
static double A, B;               /* limites de integracion */
static double IEX;                /* valor exacto de la integral */
static int HAYIEX = 0;

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
        printf("   Escribi los numeros separados por ESPACIOS (ej: 0 0.2 0.4 0.6 0.8).\n");
    }
}

static double f(double x) { return evalf(F, x); }

/* Muestra, debajo de una evaluacion, el valor de cada funcion interna */
static void aux_fn(const char *e, double x) {
    char b[1000];
    if (detalle_funciones(e, x, b) >= 2) H("            con  %s\n", b);
}



/* ------------------------------------------------------------------ */
static void glosario(void) {
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   I = valor de la integral definida de f(x) entre a y b (el area bajo la curva)\n");
    P("   a , b = limites de integracion (a = inferior, b = superior)\n");
    P("   n = cantidad de SUBINTERVALOS (pedazos) en que se parte [a , b]\n");
    P("   h = ancho de cada subintervalo = (b - a)/n     (tambien se llama 'paso')\n");
    P("   x_i = a + i*h = los puntos donde se evalua f   (x_0 = a , x_n = b)\n");
    P("   n+1 puntos  ->  n subintervalos  (OJO: no es lo mismo n que la cantidad de puntos)\n");
    P("   Et = error de TRUNCAMIENTO teorico (la formula con la derivada)\n");
    P("   error exacto = |I numerica - I exacta|   (solo si el enunciado da la exacta)\n");
}

static void cargar(void) {
    char buf[256];
    P("\n=== CARGAR LOS DATOS ===\n");
    P("  1) Tengo la FUNCION f(x)   ('integre f(x) = ... entre a y b')\n");
    P("  2) Tengo una TABLA de datos ('los datos de la tabla', mediciones, no hay formula)\n");
    int op = leer_int("Opcion (1-2): ");
    if (op == 2) {
        MODO = 1;
        do { NP = leer_lista("Valores de x  (las abscisas de la tabla, separadas por espacio): ", X, MAXPUNTOS); } while (NP < 2);
        for (;;) {
            int c = leer_lista("Valores de y = f(x)  (en el mismo orden): ", Y, MAXPUNTOS);
            if (c == NP) break;
            printf("   Cargaste %d valores de x pero %d de y.\n", NP, c);
        }
        A = X[0]; B = X[NP - 1];
        P("Se integrara entre a = %.10g y b = %.10g con los %d puntos de la tabla.\n", A, B, NP);
        F[0] = 0;
        leer_linea("Funcion EXACTA de donde salen los datos, si el enunciado la da (ENTER = no): ", F, 256);
        if (F[0] && !expr_valida(F)) { printf("   No entendi esa funcion: se ignora.\n"); F[0] = 0; }
    } else {
        MODO = 0;
        leer_funcion("\nf(x) =  (la funcion a integrar, ej: 0.2 + 25*x - 200*x^2 + 675*x^3): ", F);
        A = leer_num("a  (limite INFERIOR de integracion) = ");
        B = leer_num("b  (limite SUPERIOR de integracion) = ");
    }
    IEX = leer_num_opcional("Valor EXACTO de la integral, si el enunciado lo da (ENTER = no se conoce): ", &HAYIEX);
}

/* ------------------------------------------------------------------ */
/* Bloque de errores comun a todos los metodos                         */
/* ------------------------------------------------------------------ */
static void bloque_errores(double I, const char *nombre) {
    H("\nERRORES:\n");
    if (HAYIEX) {
        double ea = fabs(I - IEX);
        H("   Valor exacto de la integral (dato del enunciado): I = %.10f\n", IEX);
        H("   Error absoluto EXACTO = |I aproximada - I exacta| = |%.10f - %.10f| = %.3e\n", I, IEX, ea);
        if (IEX != 0)
            H("   Error RELATIVO porcentual = 100*|I aprox - I exacta|/|I exacta| = %.6f %%\n", 100 * ea / fabs(IEX));
        P("\nError absoluto exacto = %.6e", ea);
        if (IEX != 0) P("    error relativo = %.6f %%", 100 * fabs(I - IEX) / fabs(IEX));
        P("\n");
    } else {
        H("   El enunciado no da el valor exacto, asi que el error EXACTO no se puede calcular.\n");
        H("   Se informa el error de TRUNCAMIENTO teorico de %s (ver arriba) y, si se piden\n", nombre);
        H("   dos valores de n, se puede estimar el error comparando ambos resultados.\n");
    }
}

/* Escribe la formula del error teorico. orden = 2 (trapecio) o 4 (Simpson).
   Se usa la derivada PROMEDIO en [a,b], que es como lo hace el libro:
      f(k) promedio = ( f(k-1)(b) - f(k-1)(a) ) / (b - a)
   y ademas se informa el maximo de |f(k)| como COTA del error. */
static void error_teorico(int orden, double h, int n, int compuesto) {
    char d[5][600];
    if (!F[0]) return;
    strcpy(d[0], F);
    for (int i = 1; i <= orden; i++)
        if (!derivar_texto(d[i - 1], d[i])) return;
    const char *pr = (orden == 2) ? "''" : "''''";
    const char *pra = (orden == 2) ? "'" : "'''";
    double prom = (evalf(d[orden - 1], B) - evalf(d[orden - 1], A)) / (B - A);
    double mx = 0, xmx = A;
    for (int i = 0; i <= 2000; i++) {
        double x = A + (B - A) * i / 2000.0, v = fabs(evalf(d[orden], x));
        if (v == v && v > mx) { mx = v; xmx = x; }
    }
    H("\nERROR DE TRUNCAMIENTO TEORICO:\n");
    if (orden == 2) {
        if (compuesto) H("   Et = -(b - a)*h^2/12 * f''(xi) = -(b - a)^3/(12*n^2) * f''(xi)   con xi en (a , b)\n");
        else           H("   Et = -(b - a)^3/12 * f''(xi)     con xi en (a , b)\n");
    } else {
        if (compuesto) H("   Et = -(b - a)*h^4/180 * f''''(xi) = -(b - a)^5/(180*n^4) * f''''(xi)\n");
        else           H("   Et = -(b - a)^5/2880 * f''''(xi)     con xi en (a , b)   [Simpson 1/3 simple]\n");
    }
    for (int i = 1; i <= orden; i++)
        H("   f%.*s(x) = %s\n", i, "''''", d[i]);
    H("   (derivadas calculadas por el programa con las reglas de derivacion)\n");
    if (mx < 1e-12) {
        H("   La derivada de orden %d es CERO en todo el intervalo:\n", orden);
        H("   el metodo es EXACTO para esta funcion (error de truncamiento nulo).\n");
        return;
    }
    H("\n   xi es un punto desconocido de (a , b). Se usa el VALOR PROMEDIO de f%s en [a , b],\n", pr);
    H("   que se calcula exacto con el teorema fundamental del calculo:\n");
    H("      f%s promedio = [ f%s(b) - f%s(a) ] / (b - a) = [ %.10f - %.10f ] / %.10g = %.10f\n",
      pr, pra, pra, evalf(d[orden - 1], B), evalf(d[orden - 1], A), B - A, prom);
    double et;
    if (orden == 2) et = compuesto ? -(B - A) * h * h / 12.0 * prom : -pow(B - A, 3) / 12.0 * prom;
    else            et = compuesto ? -(B - A) * pow(h, 4) / 180.0 * prom : -pow(B - A, 5) / 2880.0 * prom;
    H("      Et ~ %.6e\n", et);
    H("   COTA del error (usando el maximo): max|f%s(x)| = %.10f  en x = %.10g\n", pr, mx, xmx);
    double cota;
    if (orden == 2) cota = compuesto ? (B - A) * h * h / 12.0 * mx : pow(B - A, 3) / 12.0 * mx;
    else            cota = compuesto ? (B - A) * pow(h, 4) / 180.0 * mx : pow(B - A, 5) / 2880.0 * mx;
    H("      |Et| <= %.6e\n", cota);
    if (compuesto)
        H("   (el error baja con 1/n^%d: si se duplica n, se divide por %d)\n", orden, orden == 2 ? 4 : 16);
}

/* ------------------------------------------------------------------ */
/* Valores de f en los nodos (modo funcion o modo tabla)               */
/* ------------------------------------------------------------------ */
/* Devuelve 1 si se pudieron obtener los n+1 valores. */
static int nodos(int n, double *xs, double *fs) {
    if (n + 1 > MAXINT_NODOS) { P("\nn es demasiado grande (maximo %d subintervalos).\n", MAXINT_NODOS - 1); return 0; }
    double h = (B - A) / n;
    if (MODO == 0) {
        for (int i = 0; i <= n; i++) { xs[i] = A + i * h; fs[i] = f(xs[i]); }
        return 1;
    }
    /* modo tabla: solo se puede si n coincide con la cantidad de subintervalos */
    if (n != NP - 1) {
        P("\nCon una TABLA de %d puntos solo se puede usar n = %d subintervalos.\n", NP, NP - 1);
        P("(Para otro n hace falta la funcion: cargala con la opcion 1.)\n");
        return 0;
    }
    for (int i = 0; i <= n; i++) { xs[i] = X[i]; fs[i] = Y[i]; }
    return 1;
}

/* Avisa si la tabla no tiene paso constante */
static int paso_constante(void) {
    if (MODO == 0) return 1;
    double h0 = X[1] - X[0];
    for (int i = 1; i < NP - 1; i++)
        if (fabs((X[i + 1] - X[i]) - h0) > 1e-9 * (fabs(h0) + 1)) return 0;
    return 1;
}

/* ------------------------------------------------------------------ */
/* TRAPECIO                                                            */
/* ------------------------------------------------------------------ */
static double calc_trapecio(int n, double *xs, double *fs) {
    double h = (xs[n] - xs[0]) / n, s = fs[0] + fs[n];
    for (int i = 1; i < n; i++) s += 2 * fs[i];
    return h / 2 * s;
}

static void op_trapecio(void) {
    double *xs = XS, *fs = FS;
    char sub[520];
    P("\n=== REGLA DEL TRAPECIO ===\n");
    glosario();
    P("\nIdea: se reemplaza f(x) por una RECTA en cada subintervalo y se suma el area\n");
    P("de cada trapecio.  Area de un trapecio = base * (altura1 + altura2)/2.\n");
    int n;
    if (MODO == 1) { n = NP - 1; P("\nTabla de %d puntos  ->  n = %d subintervalos.\n", NP, n); }
    else n = leer_int("\nn  (cantidad de subintervalos; 1 = trapecio SIMPLE): ");
    if (n < 1) { P("n tiene que ser 1 o mas.\n"); return; }
    if (!nodos(n, xs, fs)) return;
    if (!paso_constante()) P("\nAVISO: la tabla no tiene paso constante; igual se suma trapecio por trapecio.\n");
    double h = (B - A) / n;
    double I = calc_trapecio(n, xs, fs);

    hoja_reset();
    H("REGLA DEL TRAPECIO %s\n", n == 1 ? "(SIMPLE: un solo trapecio)" : "COMPUESTA (o de aplicacion multiple)");
    if (F[0]) H("f(x) = %s\n", F);
    H("Intervalo: a = %.10g , b = %.10g", A, B);
    H("   n = %d subintervalos   h = (b - a)/n = (%.10g - %.10g)/%d = %.10g\n", n, B, A, n, h);
    if (n == 1) {
        H("\nFormula:  I = (b - a) * [ f(a) + f(b) ] / 2\n");
        if (MODO == 0) {
            sustituir(F, A, sub); H("   f(a) = f(%.10g) = %s = %.10f\n", A, sub, fs[0]); aux_fn(F, A);
            sustituir(F, B, sub); H("   f(b) = f(%.10g) = %s = %.10f\n", B, sub, fs[1]); aux_fn(F, B);
        } else {
            H("   f(a) = %.10f     f(b) = %.10f     (de la tabla)\n", fs[0], fs[1]);
        }
        H("   I = (%.10g - %.10g) * (%.10f + %.10f)/2 = %.10f\n", B, A, fs[0], fs[1], I);
    } else {
        H("\nFormula:  I = h/2 * [ f(x0) + 2*( f(x1) + f(x2) + ... + f(x_(n-1)) ) + f(xn) ]\n");
        H("(los extremos pesan 1 y todos los puntos interiores pesan 2)\n");
        H("\nTabla de valores:\n");
        H("    i |        x_i        |      f(x_i)       | peso\n");
        for (int i = 0; i <= n; i++)
            H("  %3d | %17.10f | %17.10f |  %d\n", i, xs[i], fs[i], (i == 0 || i == n) ? 1 : 2);
        double sint = 0;
        for (int i = 1; i < n; i++) sint += fs[i];
        H("\n   suma de los interiores = %.10f\n", sint);
        H("   I = %.10g/2 * [ %.10f + 2*(%.10f) + %.10f ]\n", h, fs[0], sint, fs[n]);
        H("     = %.10g * %.10f = %.10f\n", h / 2, fs[0] + 2 * sint + fs[n], I);
    }
    H("\nRESULTADO:  I ~ %.10f\n", I);
    error_teorico(2, h, n, n > 1);
    bloque_errores(I, "la regla del trapecio");
    hoja_que_escribir("La formula del trapecio que usaste (simple o compuesta)",
                      "El valor de h con la cuenta (b - a)/n",
                      "La tabla de x_i, f(x_i) y los pesos (1 en los extremos, 2 en el medio)",
                      "La cuenta final con los numeros reemplazados",
                      "El resultado de la integral con todos los decimales",
                      "El error (teorico y, si dan la exacta, el error absoluto y relativo)",
                      (const char *)0);
    P("\nRESULTADO:  I ~ %.10f   (n = %d , h = %.10g)\n", I, n, h);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* SIMPSON 1/3                                                         */
/* ------------------------------------------------------------------ */
static double calc_simpson13(int n, double *fs, double h) {
    double s = fs[0] + fs[n];
    for (int i = 1; i < n; i++) s += (i % 2 ? 4 : 2) * fs[i];
    return h / 3 * s;
}

static void op_simpson13(void) {
    double *xs = XS, *fs = FS;
    char sub[520];
    P("\n=== REGLA DE SIMPSON 1/3 ===\n");
    glosario();
    P("\nIdea: se reemplaza f(x) por una PARABOLA cada DOS subintervalos (3 puntos).\n");
    P("IMPORTANTE: n tiene que ser PAR (la cantidad de puntos n+1 tiene que ser IMPAR).\n");
    int n;
    if (MODO == 1) {
        n = NP - 1;
        P("\nTabla de %d puntos  ->  n = %d.\n", NP, n);
        if (n % 2) { P("\nn = %d es IMPAR: Simpson 1/3 no se puede aplicar a toda la tabla.\n", n);
                     P("Usa la opcion 5 (Simpson 1/3 + 3/8 combinados) o el trapecio.\n"); return; }
    } else {
        n = leer_int("\nn  (cantidad de subintervalos, PAR; 2 = Simpson simple): ");
        if (n < 2 || n % 2) { P("n tiene que ser PAR y por lo menos 2.\n"); return; }
    }
    if (!nodos(n, xs, fs)) return;
    double h = (B - A) / n;
    double I = calc_simpson13(n, fs, h);

    hoja_reset();
    H("REGLA DE SIMPSON 1/3 %s\n", n == 2 ? "(SIMPLE: una sola parabola)" : "COMPUESTA");
    if (F[0]) H("f(x) = %s\n", F);
    H("Intervalo: a = %.10g , b = %.10g   n = %d (PAR)   h = (b - a)/n = %.10g\n", A, B, n, h);
    if (n == 2) {
        H("\nFormula:  I = h/3 * [ f(x0) + 4*f(x1) + f(x2) ]   con x1 = punto medio\n");
        H("(tambien se escribe  I = (b - a) * [f(x0) + 4 f(x1) + f(x2)] / 6)\n");
        if (MODO == 0) {
            for (int q = 0; q <= 2; q++) {
                sustituir(F, xs[q], sub);
                H("   f(x%d) = f(%.10g) = %s = %.10f\n", q, xs[q], sub, fs[q]);
                aux_fn(F, xs[q]);
            }
        }
        H("   I = %.10g/3 * [ %.10f + 4*(%.10f) + %.10f ] = %.10f\n", h, fs[0], fs[1], fs[2], I);
    } else {
        H("\nFormula:  I = h/3 * [ f(x0) + 4*(impares) + 2*(pares interiores) + f(xn) ]\n");
        H("(extremos peso 1 ; indices IMPARES peso 4 ; indices PARES interiores peso 2)\n");
        H("\nTabla de valores:\n");
        H("    i |        x_i        |      f(x_i)       | peso\n");
        for (int i = 0; i <= n; i++)
            H("  %3d | %17.10f | %17.10f |  %d\n", i, xs[i], fs[i], (i == 0 || i == n) ? 1 : (i % 2 ? 4 : 2));
        double si = 0, sp = 0;
        for (int i = 1; i < n; i++) { if (i % 2) si += fs[i]; else sp += fs[i]; }
        H("\n   suma de los IMPARES = %.10f        suma de los PARES interiores = %.10f\n", si, sp);
        H("   I = %.10g/3 * [ %.10f + 4*(%.10f) + 2*(%.10f) + %.10f ]\n", h, fs[0], si, sp, fs[n]);
        H("     = %.10g * %.10f = %.10f\n", h / 3, fs[0] + 4 * si + 2 * sp + fs[n], I);
    }
    H("\nRESULTADO:  I ~ %.10f\n", I);
    H("\nNOTA: Simpson 1/3 es EXACTO para polinomios de grado 3 o menor (aunque use parabolas).\n");
    error_teorico(4, h, n, n > 2);
    bloque_errores(I, "Simpson 1/3");
    hoja_que_escribir("Que n es PAR y por que hace falta (Simpson 1/3 usa 3 puntos por parabola)",
                      "La formula con los pesos 1-4-2-4-...-1",
                      "El valor de h y la tabla de x_i, f(x_i) y pesos",
                      "La cuenta final con los numeros y el resultado",
                      "El error teorico (depende de la derivada CUARTA) y el exacto si lo dan",
                      (const char *)0);
    P("\nRESULTADO:  I ~ %.10f   (n = %d , h = %.10g)\n", I, n, h);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* SIMPSON 3/8                                                         */
/* ------------------------------------------------------------------ */
static double calc_simpson38(int n, double *fs, double h) {
    double s = fs[0] + fs[n];
    for (int i = 1; i < n; i++) s += (i % 3 == 0 ? 2 : 3) * fs[i];
    return 3 * h / 8 * s;
}

static void op_simpson38(void) {
    double *xs = XS, *fs = FS;
    char sub[520];
    P("\n=== REGLA DE SIMPSON 3/8 ===\n");
    glosario();
    P("\nIdea: se reemplaza f(x) por una CUBICA cada TRES subintervalos (4 puntos).\n");
    P("IMPORTANTE: n tiene que ser MULTIPLO DE 3.\n");
    int n;
    if (MODO == 1) {
        n = NP - 1;
        P("\nTabla de %d puntos  ->  n = %d.\n", NP, n);
        if (n % 3) { P("\nn = %d no es multiplo de 3: Simpson 3/8 no se puede aplicar.\n", n); return; }
    } else {
        n = leer_int("\nn  (cantidad de subintervalos, MULTIPLO DE 3; 3 = simple): ");
        if (n < 3 || n % 3) { P("n tiene que ser multiplo de 3 y por lo menos 3.\n"); return; }
    }
    if (!nodos(n, xs, fs)) return;
    double h = (B - A) / n;
    double I = calc_simpson38(n, fs, h);

    hoja_reset();
    H("REGLA DE SIMPSON 3/8 %s\n", n == 3 ? "(SIMPLE: una sola cubica)" : "COMPUESTA");
    if (F[0]) H("f(x) = %s\n", F);
    H("Intervalo: a = %.10g , b = %.10g   n = %d (multiplo de 3)   h = (b - a)/n = %.10g\n", A, B, n, h);
    if (n == 3) {
        H("\nFormula:  I = 3h/8 * [ f(x0) + 3*f(x1) + 3*f(x2) + f(x3) ]\n");
        H("(tambien:  I = (b - a) * [f0 + 3 f1 + 3 f2 + f3] / 8)\n");
        if (MODO == 0)
            for (int i = 0; i <= 3; i++) { sustituir(F, xs[i], sub); H("   f(x%d) = f(%.10g) = %s = %.10f\n", i, xs[i], sub, fs[i]); aux_fn(F, xs[i]); }
        H("   I = 3*%.10g/8 * [ %.10f + 3*(%.10f) + 3*(%.10f) + %.10f ] = %.10f\n", h, fs[0], fs[1], fs[2], fs[3], I);
    } else {
        H("\nFormula:  I = 3h/8 * [ f(x0) + 3*(los que NO son multiplo de 3) + 2*(los multiplos de 3 interiores) + f(xn) ]\n");
        H("(pesos: 1, 3, 3, 2, 3, 3, 2, ... , 3, 3, 1)\n");
        H("\nTabla de valores:\n");
        H("    i |        x_i        |      f(x_i)       | peso\n");
        for (int i = 0; i <= n; i++)
            H("  %3d | %17.10f | %17.10f |  %d\n", i, xs[i], fs[i], (i == 0 || i == n) ? 1 : (i % 3 == 0 ? 2 : 3));
        double s3 = 0, s2 = 0;
        for (int i = 1; i < n; i++) { if (i % 3 == 0) s2 += fs[i]; else s3 += fs[i]; }
        H("\n   suma peso 3 = %.10f        suma peso 2 = %.10f\n", s3, s2);
        H("   I = 3*%.10g/8 * [ %.10f + 3*(%.10f) + 2*(%.10f) + %.10f ] = %.10f\n", h, fs[0], s3, s2, fs[n], I);
    }
    H("\nRESULTADO:  I ~ %.10f\n", I);
    H("\nNOTA: Simpson 3/8 tambien es exacto hasta grado 3. Se usa cuando n es impar\n");
    H("(ahi 1/3 solo no se puede) o cuando el enunciado lo pide explicitamente.\n");
    error_teorico(4, h, n, n > 3);
    bloque_errores(I, "Simpson 3/8");
    hoja_que_escribir("Que n es multiplo de 3 y por que (3/8 usa 4 puntos por tramo)",
                      "La formula con los pesos 1-3-3-2-3-3-...-1",
                      "El valor de h y la tabla de x_i, f(x_i) y pesos",
                      "La cuenta final y el resultado",
                      "El error teorico y el exacto si lo dan",
                      (const char *)0);
    P("\nRESULTADO:  I ~ %.10f   (n = %d , h = %.10g)\n", I, n, h);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* SIMPSON COMBINADO (n impar: 1/3 en los pares y 3/8 en los ultimos 3)*/
/* ------------------------------------------------------------------ */
static void op_combinado(void) {
    double *xs = XS, *fs = FS;
    P("\n=== SIMPSON COMBINADO  1/3 + 3/8   (para n IMPAR) ===\n");
    glosario();
    P("\nCuando n es IMPAR no se puede usar Simpson 1/3 en todo el intervalo.\n");
    P("La receta: Simpson 1/3 en los primeros n-3 subintervalos (cantidad par) y\n");
    P("Simpson 3/8 en los ultimos 3.  Despues se SUMAN las dos integrales.\n");
    int n;
    if (MODO == 1) { n = NP - 1; P("\nTabla de %d puntos  ->  n = %d.\n", NP, n); }
    else n = leer_int("\nn  (cantidad de subintervalos, IMPAR y mayor o igual a 5): ");
    if (n < 3) { P("Hace falta n >= 3.\n"); return; }
    if (!nodos(n, xs, fs)) return;
    double h = (B - A) / n;
    int n13 = n - 3;
    double I13 = 0, I38, I;
    hoja_reset();
    H("SIMPSON COMBINADO 1/3 + 3/8   (n = %d es impar)\n", n);
    if (F[0]) H("f(x) = %s\n", F);
    H("a = %.10g , b = %.10g , n = %d , h = %.10g\n", A, B, n, h);
    H("\nTabla de valores:\n");
    H("    i |        x_i        |      f(x_i)\n");
    for (int i = 0; i <= n; i++) H("  %3d | %17.10f | %17.10f\n", i, xs[i], fs[i]);
    if (n13 >= 2) {
        double s = fs[0] + fs[n13];
        for (int i = 1; i < n13; i++) s += (i % 2 ? 4 : 2) * fs[i];
        I13 = h / 3 * s;
        H("\n1) Simpson 1/3 desde x0 = %.10g hasta x%d = %.10g  (%d subintervalos, par):\n", xs[0], n13, xs[n13], n13);
        H("   I1 = h/3 * [ f(x0) + 4*(impares) + 2*(pares int.) + f(x%d) ] = %.10f\n", n13, I13);
    } else {
        H("\n1) No queda tramo para Simpson 1/3 (n - 3 = %d).\n", n13);
    }
    I38 = 3 * h / 8 * (fs[n - 3] + 3 * fs[n - 2] + 3 * fs[n - 1] + fs[n]);
    H("\n2) Simpson 3/8 en los ultimos 3 subintervalos, de x%d = %.10g a x%d = %.10g:\n", n - 3, xs[n - 3], n, xs[n]);
    H("   I2 = 3h/8 * [ f(x%d) + 3*f(x%d) + 3*f(x%d) + f(x%d) ]\n", n - 3, n - 2, n - 1, n);
    H("      = 3*%.10g/8 * [ %.10f + 3*(%.10f) + 3*(%.10f) + %.10f ] = %.10f\n",
      h, fs[n - 3], fs[n - 2], fs[n - 1], fs[n], I38);
    I = I13 + I38;
    H("\n3) Total:  I = I1 + I2 = %.10f + %.10f = %.10f\n", I13, I38, I);
    H("\nRESULTADO:  I ~ %.10f\n", I);
    bloque_errores(I, "Simpson combinado");
    hoja_que_escribir("Por que hay que combinar (n impar: 1/3 necesita n par)",
                      "Donde corta cada metodo (1/3 hasta x_(n-3), 3/8 en los ultimos 3)",
                      "Las dos integrales parciales I1 y I2 con sus cuentas",
                      "La suma final I = I1 + I2",
                      (const char *)0);
    P("\nRESULTADO:  I ~ %.10f   (I_1/3 = %.10f + I_3/8 = %.10f)\n", I, I13, I38);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* CUADRATURA DE GAUSS-LEGENDRE                                        */
/* ------------------------------------------------------------------ */
static void gauss_tabla(int np, double *t, double *w) {
    switch (np) {
        case 1: t[0] = 0; w[0] = 2; break;
        case 2: t[0] = -0.5773502691896257; t[1] = 0.5773502691896257;
                w[0] = w[1] = 1; break;
        case 3: t[0] = -0.7745966692414834; t[1] = 0; t[2] = 0.7745966692414834;
                w[0] = w[2] = 0.5555555555555556; w[1] = 0.8888888888888889; break;
        case 4: t[0] = -0.8611363115940526; t[1] = -0.3399810435848563;
                t[2] = 0.3399810435848563;  t[3] = 0.8611363115940526;
                w[0] = w[3] = 0.3478548451374538; w[1] = w[2] = 0.6521451548625461; break;
        case 5: t[0] = -0.9061798459386640; t[1] = -0.5384693101056831; t[2] = 0;
                t[3] = 0.5384693101056831;  t[4] = 0.9061798459386640;
                w[0] = w[4] = 0.2369268850561891; w[1] = w[3] = 0.4786286704993665;
                w[2] = 0.5688888888888889; break;
        default: t[0] = -0.9324695142031521; t[1] = -0.6612093864662645; t[2] = -0.2386191860831969;
                 t[3] = 0.2386191860831969;  t[4] = 0.6612093864662645;  t[5] = 0.9324695142031521;
                 w[0] = w[5] = 0.1713244923791704; w[1] = w[4] = 0.3607615730481386;
                 w[2] = w[3] = 0.4679139345726910; break;
    }
}

static void op_gauss(void) {
    double t[6], w[6];
    char sub[520];
    P("\n=== CUADRATURA DE GAUSS-LEGENDRE ===\n");
    if (MODO == 1 && !F[0]) {
        P("\nGauss-Legendre necesita la FUNCION: evalua f en puntos que NO son los de la tabla.\n");
        P("Carga la funcion con la opcion 1.\n");
        return;
    }
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   t_i = nodos de Gauss, tabulados en [-1 , 1]  (NO son equiespaciados)\n");
    P("   w_i = pesos de Gauss (de la tabla; no se calculan)\n");
    P("   x_i = el nodo t_i llevado al intervalo [a , b] con el cambio de variable\n");
    P("\nIdea: en vez de usar puntos equiespaciados, Gauss ELIGE los puntos y los pesos\n");
    P("para que la formula sea exacta para polinomios del mayor grado posible.\n");
    P("Con m puntos es EXACTA para polinomios de grado <= 2m - 1.\n");
    int m = leer_int("\nCantidad de puntos m  (1 a 6; lo habitual es 2 o 3): ");
    if (m < 1 || m > 6) { P("Tiene que ser entre 1 y 6.\n"); return; }
    gauss_tabla(m, t, w);
    double c1 = (B - A) / 2, c2 = (B + A) / 2, I = 0;

    hoja_reset();
    H("CUADRATURA DE GAUSS-LEGENDRE con %d punto%s\n", m, m > 1 ? "s" : "");
    H("f(x) = %s        a = %.10g , b = %.10g\n", F, A, B);
    H("\n1) CAMBIO DE VARIABLE para llevar [a , b] a [-1 , 1]:\n");
    H("   x = ( (b - a)*t + (b + a) ) / 2 = ( %.10g*t + %.10g ) / 2 = %.10g*t + %.10g\n",
      B - A, B + A, c1, c2);
    H("   dx = (b - a)/2 dt = %.10g dt\n", c1);
    H("   Entonces:  I = integral de f(x) dx entre a y b = %.10g * integral de f(x(t)) dt entre -1 y 1\n", c1);
    H("\n2) NODOS Y PESOS de la tabla de Gauss-Legendre (%d puntos):\n", m);
    H("    i |        t_i         |        w_i\n");
    for (int i = 0; i < m; i++) H("  %3d | %18.10f | %18.10f\n", i + 1, t[i], w[i]);
    H("\n3) SE LLEVA CADA NODO AL INTERVALO [a , b] Y SE EVALUA f:\n");
    for (int i = 0; i < m; i++) {
        double xi = c1 * t[i] + c2, fi = f(xi);
        H("   t%d = %.10f  ->  x%d = %.10g*(%.10f) + %.10g = %.10f\n", i + 1, t[i], i + 1, c1, t[i], c2, xi);
        if (MODO == 0) { sustituir(F, xi, sub); H("        f(x%d) = %s = %.10f\n", i + 1, sub, fi); aux_fn(F, xi); }
        else H("        f(x%d) = %.10f\n", i + 1, fi);
        I += w[i] * fi;
    }
    H("\n4) SUMA PONDERADA:\n");
    H("   suma w_i*f(x_i) = ");
    for (int i = 0; i < m; i++) H("%s%.10f*%.10f", i ? " + " : "", w[i], f(c1 * t[i] + c2));
    H(" = %.10f\n", I);
    I *= c1;
    H("   I = (b - a)/2 * suma = %.10g * %.10f = %.10f\n", c1, I / c1, I);
    H("\nRESULTADO:  I ~ %.10f\n", I);
    H("\nEXACTITUD: con %d punto%s la formula es exacta para polinomios de grado <= %d.\n",
      m, m > 1 ? "s" : "", 2 * m - 1);
    H("Si f es un polinomio de grado <= %d, el resultado es EXACTO (error de truncamiento cero).\n", 2 * m - 1);
    bloque_errores(I, "la cuadratura de Gauss");
    hoja_que_escribir("El cambio de variable x = ((b-a)t + (b+a))/2 y dx = (b-a)/2 dt",
                      "La tabla de nodos t_i y pesos w_i que usaste",
                      "El paso de cada t_i a su x_i con la cuenta",
                      "Los valores f(x_i) y la suma ponderada",
                      "La cuenta final I = (b-a)/2 * suma w_i f(x_i)",
                      "La exactitud: con m puntos es exacta hasta grado 2m - 1",
                      (const char *)0);
    P("\nRESULTADO:  I ~ %.10f   (Gauss-Legendre de %d puntos)\n", I, m);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* COMPARAR TODOS LOS METODOS                                          */
/* ------------------------------------------------------------------ */
static void op_comparar(void) {
    double *xs = XS, *fs = FS; double t[6], w[6];
    P("\n=== COMPARAR TODOS LOS METODOS ===\n");
    int n;
    if (MODO == 1) { n = NP - 1; P("Tabla de %d puntos -> n = %d.\n", NP, n); }
    else n = leer_int("n  (cantidad de subintervalos para trapecio y Simpson, ej: 6): ");
    if (n < 1 || !nodos(n, xs, fs)) return;
    double h = (B - A) / n;
    hoja_reset();
    H("COMPARACION DE METODOS DE INTEGRACION\n");
    if (F[0]) H("f(x) = %s", F);
    H("     a = %.10g , b = %.10g , n = %d , h = %.10g\n\n", A, B, n, h);
    H("  METODO                      |        I         ");
    if (HAYIEX) H("|  error absoluto |  error rel. %%");
    H("\n");
    H("  ----------------------------+------------------");
    if (HAYIEX) H("+-----------------+---------------");
    H("\n");
    double It = calc_trapecio(n, xs, fs);
    const char *nm[8]; double vals[8]; int k = 0;
    nm[k] = "Trapecio compuesto"; vals[k++] = It;
    if (n % 2 == 0) { nm[k] = "Simpson 1/3 compuesto"; vals[k++] = calc_simpson13(n, fs, h); }
    if (n % 3 == 0) { nm[k] = "Simpson 3/8 compuesto"; vals[k++] = calc_simpson38(n, fs, h); }
    if (MODO == 0 || F[0]) {
        double c1 = (B - A) / 2, c2 = (B + A) / 2;
        static const char *gn[3] = {"Gauss-Legendre 2 puntos", "Gauss-Legendre 3 puntos", "Gauss-Legendre 4 puntos"};
        for (int m = 2; m <= 4; m++) {
            gauss_tabla(m, t, w);
            double s = 0;
            for (int i = 0; i < m; i++) s += w[i] * f(c1 * t[i] + c2);
            nm[k] = gn[m - 2]; vals[k++] = c1 * s;
        }
    }
    for (int i = 0; i < k; i++) {
        H("  %-27s | %16.10f ", nm[i], vals[i]);
        if (HAYIEX) {
            double ea = fabs(vals[i] - IEX);
            H("| %15.6e | %13.6f ", ea, IEX ? 100 * ea / fabs(IEX) : 0.0);
        }
        H("\n");
    }
    if (HAYIEX) H("\n  Valor EXACTO: I = %.10f\n", IEX);
    H("\nCONCLUSION TIPICA: con el mismo n, Simpson es mas preciso que el trapecio\n");
    H("(error O(h^4) contra O(h^2)); y Gauss logra la misma o mejor precision con\n");
    H("muchas MENOS evaluaciones de f, porque elige los nodos de forma optima.\n");
    hoja_que_escribir("La tabla comparativa de los metodos con sus valores",
                      "Cual es mas preciso y POR QUE (orden del error de cada uno)",
                      "La cantidad de evaluaciones de f que necesita cada uno",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* CUANTOS SUBINTERVALOS PARA UNA TOLERANCIA                           */
/* ------------------------------------------------------------------ */
static void op_cuantos_n(void) {
    P("\n=== CUANTOS SUBINTERVALOS HACEN FALTA PARA UN ERROR DADO ===\n");
    if (!F[0]) { P("Hace falta la funcion f(x). Cargala con la opcion 1.\n"); return; }
    P("\n  1) Trapecio compuesto      (error ~ (b-a)^3/(12 n^2) * max|f''|)\n");
    P("  2) Simpson 1/3 compuesto   (error ~ (b-a)^5/(180 n^4) * max|f''''|)\n");
    int met = leer_int("Opcion (1-2): ");
    if (met < 1 || met > 2) return;
    double tol = leer_num("Tolerancia (error maximo admitido, ej: 1e-4): ");
    char d[5][600];
    int orden = (met == 1) ? 2 : 4;
    strcpy(d[0], F);
    for (int i = 1; i <= orden; i++)
        if (!derivar_texto(d[i - 1], d[i])) { P("No pude derivar la funcion %d veces.\n", orden); return; }
    /* maximo de |derivada| en [a,b] por muestreo */
    double mx = 0, xmx = A;
    for (int i = 0; i <= 2000; i++) {
        double x = A + (B - A) * i / 2000.0, v = fabs(evalf(d[orden], x));
        if (v == v && v > mx) { mx = v; xmx = x; }
    }
    hoja_reset();
    H("CANTIDAD DE SUBINTERVALOS NECESARIOS   (%s, tolerancia %.3e)\n",
      met == 1 ? "trapecio compuesto" : "Simpson 1/3 compuesto", tol);
    H("f(x) = %s     a = %.10g , b = %.10g\n", F, A, B);
    for (int i = 1; i <= orden; i++) H("   derivada %d: %s\n", i, d[i]);
    H("\nSe acota la derivada en [a , b]:  max|f%s(x)| = %.10f  (en x = %.10g)\n",
      orden == 2 ? "''" : "''''", mx, xmx);
    if (mx < 1e-14) {
        H("\nLa derivada es CERO: el metodo es exacto para esta funcion, alcanza con n = %d.\n", met == 1 ? 1 : 2);
        hoja_mostrar(); return;
    }
    double n;
    if (met == 1) {
        H("\n|Et| = (b - a)^3/(12 n^2) * max|f''| <= tol\n");
        H("   n >= raiz( (b - a)^3 * max|f''| / (12 * tol) )\n");
        n = sqrt(pow(B - A, 3) * mx / (12 * tol));
        H("   n >= raiz( %.10g * %.10f / (12 * %.3e) ) = %.6f\n", pow(B - A, 3), mx, tol, n);
    } else {
        H("\n|Et| = (b - a)^5/(180 n^4) * max|f''''| <= tol\n");
        H("   n >= ( (b - a)^5 * max|f''''| / (180 * tol) )^(1/4)\n");
        n = pow(pow(B - A, 5) * mx / (180 * tol), 0.25);
        H("   n >= ( %.10g * %.10f / (180 * %.3e) )^(1/4) = %.6f\n", pow(B - A, 5), mx, tol, n);
    }
    int ne = (int)ceil(n - 1e-12);
    if (ne < 1) ne = 1;
    if (met == 2) { if (ne % 2) ne++; if (ne < 2) ne = 2; }
    H("\nSe redondea para ARRIBA%s:  n = %d\n", met == 2 ? " al primer PAR (Simpson 1/3 lo exige)" : "", ne);
    H("   h = (b - a)/n = %.10g\n", (B - A) / ne);
    /* valor con ese n */
    double *xs = XS, *fs = FS;
    if (ne <= MAXPUNTOS - 1) {
        double hh = (B - A) / ne;
        for (int i = 0; i <= ne; i++) { xs[i] = A + i * hh; fs[i] = f(xs[i]); }
        double I = (met == 1) ? calc_trapecio(ne, xs, fs) : calc_simpson13(ne, fs, hh);
        H("   Con n = %d resulta  I ~ %.10f\n", ne, I);
        if (HAYIEX) H("   Error real cometido: |%.10f - %.10f| = %.3e  (menor que la tolerancia %.3e)\n",
                      I, IEX, fabs(I - IEX), tol);
    }
    hoja_que_escribir("La formula del error del metodo",
                      "La cota de la derivada en el intervalo y donde se alcanza",
                      "El despeje de n y el redondeo HACIA ARRIBA (y a par si es Simpson 1/3)",
                      "El n final, el h correspondiente y el valor de la integral",
                      (const char *)0);
    P("\nn necesario = %d   (h = %.10g)\n", ne, (B - A) / ne);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* METODO A MEDIDA: el enunciado da una formula nueva y hay que         */
/* implementarla. Se escribe el TERMINO de la sumatoria.                */
/* ------------------------------------------------------------------ */
static void op_a_medida(void) {
    char term[256], ps[256];
    double *xs = XS, *fs = FS;
    P("\n=== METODO DE INTEGRACION A MEDIDA (definido por vos) ===\n");
    P("\nSirve cuando el enunciado INVENTA un metodo y pide implementarlo.\n");
    P("Se escribe el TERMINO GENERAL de la sumatoria, o sea lo que va adentro de\n");
    P("   I = suma desde i=0 hasta n-1 de [ ... ]\n");
    P("\nVARIABLES QUE PODES USAR EN LA FORMULA:\n");
    P("   xi    = x_i           (extremo izquierdo del subintervalo i)\n");
    P("   xi1   = x_(i+1)       (extremo derecho)\n");
    P("   h     = x_(i+1) - x_i (ancho del subintervalo)\n");
    P("   fi    = f(x_i)        fi1 = f(x_(i+1))\n");
    P("   fm    = f del PUNTO MEDIO del subintervalo  (solo si cargaste la funcion)\n");
    P("   i     = numero de subintervalo ;  n = cantidad total\n");
    P("\nEJEMPLOS:\n");
    P("   Trapecio:                        h*(fi + fi1)/2\n");
    P("   Rectangulo por izquierda:        h*fi\n");
    P("   Punto medio (rectangulo medio):  h*fm\n");
    P("   El del parcial 2022:             (xi1-xi)*fi + 0.5*(xi1-xi)*(fi1-fi)\n");
    /* se registran las variables con un valor cualquiera para que el
       validador de expresiones acepte la formula al escribirla */
    vars_reset();
    var_set("xi", 1); var_set("xi1", 1); var_set("h", 1); var_set("fi", 1);
    var_set("fi1", 1); var_set("fm", 1); var_set("i", 0); var_set("n", 1);
    leer_funcion("\nTermino de la sumatoria = ", term);
    int n;
    if (MODO == 1) { n = NP - 1; P("\nTabla de %d puntos -> n = %d subintervalos.\n", NP, n); }
    else n = leer_int("\nn  (cantidad de subintervalos) = ");
    if (n < 1 || !nodos(n, xs, fs)) return;
    double h = (B - A) / n, I = 0;
    int mostrar = (n <= 25);

    hoja_reset();
    H("METODO DE INTEGRACION DEFINIDO EN EL ENUNCIADO\n");
    if (F[0]) H("f(x) = %s\n", F);
    H("a = %.10g , b = %.10g , n = %d subintervalos , h = (b - a)/n = %.10g\n", A, B, n, h);
    H("\nFormula implementada:\n");
    H("   I = suma_(i=0)^(n-1) [ %s ]\n", term);
    H("\nPSEUDOCODIGO (esto es lo que hay que escribir en la hoja si lo piden):\n");
    H("   ALGORITMO IntegracionMetodoPropuesto\n");
    H("   ENTRADA: f, a, b, n\n");
    H("   SALIDA:  I (valor aproximado de la integral)\n");
    H("   1   h <- (b - a)/n\n");
    H("   2   I <- 0\n");
    H("   3   PARA i = 0 HASTA n-1 HACER\n");
    H("   4       xi  <- a + i*h\n");
    H("   5       xi1 <- a + (i+1)*h\n");
    H("   6       fi  <- f(xi)\n");
    H("   7       fi1 <- f(xi1)\n");
    H("   8       I   <- I + ( %s )\n", term);
    H("   9   FIN PARA\n");
    H("   10  DEVOLVER I\n");
    if (mostrar) {
        H("\nDETALLE DE LA SUMATORIA:\n");
        H("    i |      x_i      |    x_(i+1)    |     f(x_i)     |   f(x_(i+1))   |     termino\n");
        H("  ----+---------------+---------------+----------------+----------------+----------------\n");
    }
    for (int i = 0; i < n; i++) {
        double hi = xs[i + 1] - xs[i];
        var_set("xi", xs[i]); var_set("xi1", xs[i + 1]); var_set("h", hi);
        var_set("fi", fs[i]); var_set("fi1", fs[i + 1]);
        var_set("i", (double)i); var_set("n", (double)n);
        if (MODO == 0) var_set("fm", f(0.5 * (xs[i] + xs[i + 1])));
        double t = evalf(term, xs[i]);
        I += t;
        if (mostrar) H("  %3d | %13.8f | %13.8f | %14.8f | %14.8f | %14.8f\n",
                       i, xs[i], xs[i + 1], fs[i], fs[i + 1], t);
    }
    if (!mostrar) H("\n(n = %d es grande: no se imprime el detalle fila por fila.)\n", n);
    H("\nRESULTADO:  I ~ %.10f\n", I);
    /* comparacion con el trapecio: muchas veces el metodo "nuevo" es equivalente */
    double It = calc_trapecio(n, xs, fs);
    H("\nCOMPARACION con la regla del TRAPECIO (mismo n): I_trapecio = %.10f\n", It);
    if (fabs(I - It) < 1e-9 * (fabs(It) + 1)) {
        H("   Los dos valores COINCIDEN. Eso no es casualidad: si se desarrolla la suma\n");
        H("   propuesta y se cancelan los terminos (suma telescopica), se llega exactamente\n");
        H("   a la formula del trapecio compuesto. Conviene DECIRLO en la hoja: el metodo\n");
        H("   propuesto ES el trapecio escrito de otra manera, por eso da lo mismo.\n");
    } else {
        H("   Diferencia con el trapecio: %.3e\n", fabs(I - It));
    }
    bloque_errores(I, "el metodo propuesto");
    hoja_que_escribir("El pseudocodigo del metodo (esta arriba, listo para copiar)",
                      "La formula del termino general y que significa cada variable",
                      "El valor de h y la tabla del detalle (si n es chico)",
                      "El resultado para cada n que pidan",
                      "El error absoluto exacto de cada caso",
                      "La comparacion con el trapecio y la conclusion",
                      (const char *)0);
    P("\nRESULTADO:  I ~ %.10f    (trapecio con el mismo n: %.10f)\n", I, It);
    strcpy(ps, term);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void op_evaluar(void) {
    if (!F[0]) { P("No hay funcion cargada.\n"); return; }
    double x = leer_num("x  (punto donde evaluar f) = ");
    char sub[520];
    sustituir(F, x, sub);
    P("f(%.10g) = %s = %.10f\n", x, sub, f(x));
}

/* ------------------------------------------------------------------ */
int main(void) {
    abrir_log("salida_integracion.txt");
    P("\n############  INTEGRACION NUMERICA   (Guias 7 y 8)  ############\n");
    P("\nTodo lo que calcula queda tambien en el archivo salida_integracion.txt\n");
    cargar();
    for (;;) {
        P("\n======================== MENU ========================\n");
        if (MODO == 0) P("  f(x) = %s     a = %.10g , b = %.10g\n", F, A, B);
        else P("  TABLA de %d puntos     a = %.10g , b = %.10g\n", NP, A, B);
        P("\n  OPCION                          USALA SI EL ENUNCIADO DICE...\n");
        P("  1) Cargar otros datos\n");
        P("  2) Trapecio                     'regla del trapecio', 'trapecio simple/compuesto/multiple'\n");
        P("  3) Simpson 1/3                  'Simpson', 'Simpson 1/3', 'regla de la parabola'\n");
        P("  4) Simpson 3/8                  'Simpson 3/8', n multiplo de 3\n");
        P("  5) Simpson 1/3 + 3/8            n IMPAR, 'combine las reglas de Simpson'\n");
        P("  6) Cuadratura de Gauss          'Gauss', 'Gauss-Legendre', 'cuadratura de 2/3 puntos'\n");
        P("  7) Comparar TODOS los metodos   'compare', 'cual es mas preciso', 'discuta'\n");
        P("  8) Cuantos n para un error      'cuantos subintervalos', 'para que el error sea menor que'\n");
        P("  9) METODO A MEDIDA              'se propone un metodo', 'implementelo', 'pseudocodigo'\n");
        P(" 10) Evaluar f en un punto        para controlar una cuenta a mano\n");
        P("  0) Salir\n");
        int op = leer_int("Opcion: ");
        if (op == 0) break;
        switch (op) {
            case 1: cargar(); break;
            case 2: op_trapecio(); break;
            case 3: op_simpson13(); break;
            case 4: op_simpson38(); break;
            case 5: op_combinado(); break;
            case 6: op_gauss(); break;
            case 7: op_comparar(); break;
            case 8: op_cuantos_n(); break;
            case 9: op_a_medida(); break;
            case 10: op_evaluar(); break;
            default: P("Opcion invalida.\n");
        }
        if (op >= 1 && op <= 10) pausa();
    }
    if (LOG_MN) fclose(LOG_MN);
    return 0;
}
