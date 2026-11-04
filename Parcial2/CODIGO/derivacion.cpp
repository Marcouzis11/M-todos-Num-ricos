/* derivacion.cpp -- DIFERENCIACION NUMERICA: DIFERENCIAS FINITAS   (Guia 8)
 *
 *  Derivadas primera, segunda, tercera y cuarta por diferencias hacia adelante,
 *  hacia atras y centradas, en version O(h) y de ALTA EXACTITUD O(h^2) / O(h^4).
 *  Extrapolacion de Richardson, derivadas de una TABLA de datos, comparacion con
 *  la derivada exacta (el programa la calcula solo) y el efecto del tamano de h
 *  (incluido el error de redondeo cuando h es demasiado chico).
 *
 *  Todo queda en salida_derivacion.txt
 */
#include "mn_comun.h"

static char F[256] = "";
static int MODO = 0;              /* 0 = funcion , 1 = tabla */
static int NP = 0;
static double X[MAXPUNTOS], Y[MAXPUNTOS];
static char DER[5][600];          /* derivadas simbolicas 1..4 (si se pueden) */
static int NDER = 0;

static double f(double x) { return evalf(F, x); }

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
        printf("   Escribi los numeros separados por ESPACIOS.\n");
    }
}

static void glosario(void) {
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   h = paso: la distancia entre los puntos que se usan para la cuenta\n");
    P("   HACIA ADELANTE: usa x, x+h, x+2h...  (sirve en el PRIMER punto de una tabla)\n");
    P("   HACIA ATRAS:    usa x, x-h, x-2h...  (sirve en el ULTIMO punto de una tabla)\n");
    P("   CENTRADA:       usa x-h y x+h        (la mas precisa; necesita puntos a los dos lados)\n");
    P("   O(h) , O(h^2) , O(h^4) = ORDEN del error: con O(h^2), al partir h al medio\n");
    P("                            el error se divide por 4; con O(h^4), por 16\n");
    P("   Richardson = combinar dos resultados con h y h/2 para ganar precision gratis\n");
}

static void cargar(void) {
    P("\n=== CARGAR LOS DATOS ===\n");
    P("  1) Tengo la FUNCION f(x)\n");
    P("  2) Tengo una TABLA de datos (mediciones, sin formula)\n");
    int op = leer_int("Opcion (1-2): ");
    NDER = 0;
    if (op == 2) {
        MODO = 1;
        do { NP = leer_lista("Valores de x  (ORDENADOS, separados por espacio): ", X, MAXPUNTOS); } while (NP < 2);
        for (;;) {
            int c = leer_lista("Valores de y = f(x)  (en el mismo orden): ", Y, MAXPUNTOS);
            if (c == NP) break;
            printf("   Cargaste %d valores de x pero %d de y.\n", NP, c);
        }
        F[0] = 0;
        leer_linea("Funcion EXACTA de donde salen los datos, si la da el enunciado (ENTER = no): ", F, 256);
        if (F[0] && !expr_valida(F)) { printf("   No entendi esa funcion: se ignora.\n"); F[0] = 0; }
    } else {
        MODO = 0;
        leer_funcion("\nf(x) =  (la funcion a derivar, ej: -0.1*x^4 - 0.15*x^3 - 0.5*x^2 - 0.25*x + 1.2): ", F);
    }
    if (F[0]) {
        strcpy(DER[0], F);
        for (int i = 1; i <= 4; i++) {
            if (!derivar_texto(DER[i - 1], DER[i])) break;
            NDER = i;
        }
        if (NDER) {
            P("\nDERIVADAS EXACTAS CALCULADAS POR EL PROGRAMA (para comparar):\n");
            for (int i = 1; i <= NDER; i++) P("   f%.*s(x) = %s\n", i, "''''", DER[i]);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Las 6 formulas de derivada primera y las 6 de segunda               */
/* ------------------------------------------------------------------ */
/* id: 0..5 primera ; 6..11 segunda */
static const char *NOMF[12] = {
    "Primera, HACIA ADELANTE, O(h)",
    "Primera, HACIA ATRAS, O(h)",
    "Primera, CENTRADA, O(h^2)",
    "Primera, HACIA ADELANTE alta exactitud, O(h^2)",
    "Primera, HACIA ATRAS alta exactitud, O(h^2)",
    "Primera, CENTRADA alta exactitud, O(h^4)",
    "Segunda, HACIA ADELANTE, O(h)",
    "Segunda, HACIA ATRAS, O(h)",
    "Segunda, CENTRADA, O(h^2)",
    "Segunda, HACIA ADELANTE alta exactitud, O(h^2)",
    "Segunda, HACIA ATRAS alta exactitud, O(h^2)",
    "Segunda, CENTRADA alta exactitud, O(h^4)"
};
static const char *FORMF[12] = {
    "f'(x) ~ [ f(x+h) - f(x) ] / h",
    "f'(x) ~ [ f(x) - f(x-h) ] / h",
    "f'(x) ~ [ f(x+h) - f(x-h) ] / (2h)",
    "f'(x) ~ [ -f(x+2h) + 4*f(x+h) - 3*f(x) ] / (2h)",
    "f'(x) ~ [ 3*f(x) - 4*f(x-h) + f(x-2h) ] / (2h)",
    "f'(x) ~ [ -f(x+2h) + 8*f(x+h) - 8*f(x-h) + f(x-2h) ] / (12h)",
    "f''(x) ~ [ f(x+2h) - 2*f(x+h) + f(x) ] / h^2",
    "f''(x) ~ [ f(x) - 2*f(x-h) + f(x-2h) ] / h^2",
    "f''(x) ~ [ f(x+h) - 2*f(x) + f(x-h) ] / h^2",
    "f''(x) ~ [ -f(x+3h) + 4*f(x+2h) - 5*f(x+h) + 2*f(x) ] / h^2",
    "f''(x) ~ [ 2*f(x) - 5*f(x-h) + 4*f(x-2h) - f(x-3h) ] / h^2",
    "f''(x) ~ [ -f(x+2h) + 16*f(x+h) - 30*f(x) + 16*f(x-h) - f(x-2h) ] / (12h^2)"
};
static const int ORD[12] = {1, 1, 2, 2, 2, 4, 1, 1, 2, 2, 2, 4};

static double dif(int id, double x, double h) {
    switch (id) {
        case 0:  return (f(x + h) - f(x)) / h;
        case 1:  return (f(x) - f(x - h)) / h;
        case 2:  return (f(x + h) - f(x - h)) / (2 * h);
        case 3:  return (-f(x + 2 * h) + 4 * f(x + h) - 3 * f(x)) / (2 * h);
        case 4:  return (3 * f(x) - 4 * f(x - h) + f(x - 2 * h)) / (2 * h);
        case 5:  return (-f(x + 2 * h) + 8 * f(x + h) - 8 * f(x - h) + f(x - 2 * h)) / (12 * h);
        case 6:  return (f(x + 2 * h) - 2 * f(x + h) + f(x)) / (h * h);
        case 7:  return (f(x) - 2 * f(x - h) + f(x - 2 * h)) / (h * h);
        case 8:  return (f(x + h) - 2 * f(x) + f(x - h)) / (h * h);
        case 9:  return (-f(x + 3 * h) + 4 * f(x + 2 * h) - 5 * f(x + h) + 2 * f(x)) / (h * h);
        case 10: return (2 * f(x) - 5 * f(x - h) + 4 * f(x - 2 * h) - f(x - 3 * h)) / (h * h);
        default: return (-f(x + 2 * h) + 16 * f(x + h) - 30 * f(x) + 16 * f(x - h) - f(x - 2 * h)) / (12 * h * h);
    }
}

/* escribe los f(x +- k h) que usa la formula */
static void detalle_valores(int id, double x, double h) {
    static const int OFS[12][5] = {
        {0, 1, 9, 9, 9}, {-1, 0, 9, 9, 9}, {-1, 1, 9, 9, 9},
        {0, 1, 2, 9, 9}, {-2, -1, 0, 9, 9}, {-2, -1, 1, 2, 9},
        {0, 1, 2, 9, 9}, {-2, -1, 0, 9, 9}, {-1, 0, 1, 9, 9},
        {0, 1, 2, 3, 9}, {-3, -2, -1, 0, 9}, {-2, -1, 0, 1, 2}
    };
    char sub[520];
    H("\n   Valores que hacen falta:\n");
    for (int k = 0; k < 5; k++) {
        int o = OFS[id][k];
        if (o == 9) continue;
        double xx = x + o * h;
        if (MODO == 0) { sustituir(F, xx, sub); H("      f(%.10g) = %s = %.10f\n", xx, sub, f(xx)); }
        else H("      f(%.10g) = %.10f\n", xx, f(xx));
    }
}

/* ------------------------------------------------------------------ */
static void op_derivar(int segunda) {
    double x, h;
    P("\n=== DERIVADA %s POR DIFERENCIAS FINITAS ===\n", segunda ? "SEGUNDA" : "PRIMERA");
    glosario();
    if (MODO == 1 && !F[0]) {
        P("\nCon una TABLA usa la opcion 4 (derivar la tabla en sus nodos).\n");
        return;
    }
    int base = segunda ? 6 : 0;
    P("\nFORMULAS DISPONIBLES:\n");
    for (int i = 0; i < 6; i++) P("  %d) %-48s %s\n", i + 1, NOMF[base + i], FORMF[base + i]);
    P("  7) TODAS (tabla comparativa)   <- lo mas util si el enunciado dice 'compare'\n");
    int op = leer_int("Opcion (1-7): ");
    if (op < 1 || op > 7) return;
    x = leer_num("x  (punto donde derivar) = ");
    h = leer_num("h  (paso, ej: 0.25) = ");
    if (h == 0) { P("h no puede ser 0.\n"); return; }
    double exacta = 0; int hayex = 0;
    if (NDER >= (segunda ? 2 : 1)) { exacta = evalf(DER[segunda ? 2 : 1], x); hayex = 1; }

    hoja_reset();
    if (op == 7) {
        H("DERIVADA %s POR DIFERENCIAS FINITAS - COMPARACION DE FORMULAS\n", segunda ? "SEGUNDA" : "PRIMERA");
        H("f(x) = %s        x = %.10g        h = %.10g\n", F, x, h);
        if (hayex) H("Derivada exacta: f%s(x) = %s   ->   f%s(%.10g) = %.10f\n",
                     segunda ? "''" : "'", DER[segunda ? 2 : 1], segunda ? "''" : "'", x, exacta);
        H("\n  FORMULA                                          | orden |      valor      ");
        if (hayex) H("|   error abs. |  error %%");
        H("\n  -------------------------------------------------+-------+-----------------");
        if (hayex) H("+--------------+----------");
        H("\n");
        for (int i = 0; i < 6; i++) {
            double v = dif(base + i, x, h);
            H("  %-48s | O(h^%d) | %15.10f ", NOMF[base + i], ORD[base + i], v);
            if (hayex) {
                double ea = fabs(v - exacta);
                H("| %12.3e | %8.4f ", ea, exacta != 0 ? 100 * ea / fabs(exacta) : 0.0);
            }
            H("\n");
        }
        H("\nCONCLUSION TIPICA: la CENTRADA es mas precisa que las de un solo lado con el\n");
        H("mismo h (su error es O(h^2) contra O(h)), y las de alta exactitud mejoran\n");
        H("todavia mas porque usan mas puntos. Las de un lado se usan cuando no hay\n");
        H("datos del otro lado (los extremos de una tabla).\n");
        hoja_que_escribir("La tabla comparativa con las formulas y sus valores",
                          "El orden del error de cada una",
                          "Cual es mas precisa y POR QUE",
                          "Cuando hay que usar las de un solo lado (extremos de la tabla)",
                          (const char *)0);
    } else {
        int id = base + op - 1;
        double v = dif(id, x, h);
        H("DERIVADA %s:  %s\n", segunda ? "SEGUNDA" : "PRIMERA", NOMF[id]);
        H("f(x) = %s        x = %.10g        h = %.10g\n", F, x, h);
        H("\nFormula:   %s\n", FORMF[id]);
        detalle_valores(id, x, h);
        H("\n   Reemplazando:\n");
        H("      %s\n", FORMF[id]);
        H("      = %.10f\n", v);
        H("\nRESULTADO:  f%s(%.10g) ~ %.10f\n", segunda ? "''" : "'", x, v);
        if (hayex) {
            double ea = fabs(v - exacta);
            H("\nCONTROL CON LA DERIVADA EXACTA:\n");
            H("   f%s(x) = %s          (derivada calculada por el programa)\n", segunda ? "''" : "'", DER[segunda ? 2 : 1]);
            H("   Valor exacto:  f%s(%.10g) = %.10f\n", segunda ? "''" : "'", x, exacta);
            H("   Error absoluto = |%.10f - %.10f| = %.3e\n", v, exacta, ea);
            if (exacta != 0) H("   Error relativo porcentual = %.6f %%\n", 100 * ea / fabs(exacta));
        }
        H("\nORDEN DEL ERROR: O(h^%d). Si se parte h al medio, el error deberia dividirse por %d.\n",
          ORD[id], ORD[id] == 1 ? 2 : (ORD[id] == 2 ? 4 : 16));
        hoja_que_escribir("La formula usada, escrita completa",
                          "Los valores de f en cada punto que hace falta",
                          "El reemplazo con numeros y el resultado",
                          hayex ? "La derivada exacta y el error absoluto y relativo" : "El orden del error de la formula",
                          (const char *)0);
        P("\nRESULTADO:  f%s(%.10g) ~ %.10f\n", segunda ? "''" : "'", x, v);
        if (hayex) P("Exacta = %.10f   error = %.3e\n", exacta, fabs(v - exacta));
    }
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* ORDEN SUPERIOR (tercera y cuarta, centradas)                        */
/* ------------------------------------------------------------------ */
static void op_orden_superior(void) {
    P("\n=== DERIVADAS TERCERA Y CUARTA (centradas) ===\n");
    if (!F[0]) { P("Hace falta la funcion.\n"); return; }
    double x = leer_num("x  (punto donde derivar) = ");
    double h = leer_num("h  (paso) = ");
    if (h == 0) return;
    double d3 = (f(x + 2 * h) - 2 * f(x + h) + 2 * f(x - h) - f(x - 2 * h)) / (2 * pow(h, 3));
    double d4 = (f(x + 2 * h) - 4 * f(x + h) + 6 * f(x) - 4 * f(x - h) + f(x - 2 * h)) / pow(h, 4);
    hoja_reset();
    H("DERIVADAS DE ORDEN SUPERIOR POR DIFERENCIAS CENTRADAS\n");
    H("f(x) = %s     x = %.10g     h = %.10g\n", F, x, h);
    H("\nTERCERA DERIVADA, centrada, O(h^2):\n");
    H("   f'''(x) ~ [ f(x+2h) - 2*f(x+h) + 2*f(x-h) - f(x-2h) ] / (2*h^3)\n");
    H("   = [ %.10f - 2*(%.10f) + 2*(%.10f) - %.10f ] / (2*%.10g)\n",
      f(x + 2 * h), f(x + h), f(x - h), f(x - 2 * h), pow(h, 3));
    H("   = %.10f\n", d3);
    if (NDER >= 3) H("   Exacta: f'''(x) = %s  ->  %.10f    error = %.3e\n",
                     DER[3], evalf(DER[3], x), fabs(d3 - evalf(DER[3], x)));
    H("\nCUARTA DERIVADA, centrada, O(h^2):\n");
    H("   f''''(x) ~ [ f(x+2h) - 4*f(x+h) + 6*f(x) - 4*f(x-h) + f(x-2h) ] / h^4\n");
    H("   = %.10f\n", d4);
    if (NDER >= 4) H("   Exacta: f''''(x) = %s  ->  %.10f    error = %.3e\n",
                     DER[4], evalf(DER[4], x), fabs(d4 - evalf(DER[4], x)));
    H("\nNOTA: cada orden que se sube divide por otra potencia de h, asi que estas\n");
    H("formulas son MUY sensibles al error de redondeo si h es chico.\n");
    hoja_que_escribir("La formula centrada de la derivada pedida",
                      "Los valores de f en los 5 puntos",
                      "La cuenta y el resultado",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* RICHARDSON                                                          */
/* ------------------------------------------------------------------ */
static void op_richardson(void) {
    P("\n=== EXTRAPOLACION DE RICHARDSON ===\n");
    if (!F[0]) { P("Hace falta la funcion.\n"); return; }
    P("\nIdea: se calcula la derivada con h y con h/2, y se COMBINAN para cancelar\n");
    P("el termino principal del error. Se gana un orden sin evaluar formulas nuevas.\n");
    P("\n  1) Sobre la derivada primera CENTRADA O(h^2)   ->  queda O(h^4)\n");
    P("  2) Sobre la derivada segunda CENTRADA O(h^2)   ->  queda O(h^4)\n");
    int cual = leer_int("Opcion (1-2): ");
    if (cual < 1 || cual > 2) return;
    int id = (cual == 1) ? 2 : 8;
    double x = leer_num("x  (punto donde derivar) = ");
    double h = leer_num("h  (paso grande; se usara tambien h/2) = ");
    if (h == 0) return;
    double D1 = dif(id, x, h), D2 = dif(id, x, h / 2);
    double R = (4 * D2 - D1) / 3;
    double exacta = 0; int hayex = 0;
    if (NDER >= (cual == 1 ? 1 : 2)) { exacta = evalf(DER[cual == 1 ? 1 : 2], x); hayex = 1; }
    hoja_reset();
    H("EXTRAPOLACION DE RICHARDSON sobre la derivada %s centrada\n", cual == 1 ? "PRIMERA" : "SEGUNDA");
    H("f(x) = %s     x = %.10g\n", F, x);
    H("\nFormula de Richardson para un metodo de orden O(h^2):\n");
    H("   D = ( 4*D(h/2) - D(h) ) / 3          (tambien se escribe D = D(h/2) + [D(h/2) - D(h)]/3)\n");
    H("\n1) Con h = %.10g:\n", h);
    H("   %s\n", FORMF[id]);
    H("   D(h) = %.10f\n", D1);
    H("\n2) Con h/2 = %.10g:\n", h / 2);
    H("   D(h/2) = %.10f\n", D2);
    H("\n3) Se combinan:\n");
    H("   D = ( 4*(%.10f) - (%.10f) ) / 3 = %.10f\n", D2, D1, R);
    H("\nRESULTADO:  derivada ~ %.10f\n", R);
    if (hayex) {
        H("\nCONTROL:\n");
        H("   Valor exacto                 = %.10f\n", exacta);
        H("   Error con h                  = %.3e\n", fabs(D1 - exacta));
        H("   Error con h/2                = %.3e\n", fabs(D2 - exacta));
        H("   Error con Richardson         = %.3e   <- el mejor\n", fabs(R - exacta));
        H("\n   Se ve que Richardson mejora varios ORDENES de magnitud respecto de los dos\n");
        H("   valores que combina, sin necesidad de una formula nueva.\n");
    }
    hoja_que_escribir("Los dos valores D(h) y D(h/2) con sus cuentas",
                      "La formula de Richardson",
                      "La combinacion con numeros y el resultado",
                      hayex ? "La comparacion de los tres errores" : "Que el orden pasa de O(h^2) a O(h^4)",
                      (const char *)0);
    P("\nRichardson: %.10f\n", R);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* DERIVAR UNA TABLA DE DATOS                                          */
/* ------------------------------------------------------------------ */
static void op_tabla(void) {
    if (MODO != 1) {
        /* modo funcion: se genera primero la tabla equiespaciada que pide el enunciado */
        P("\n=== GENERAR LA TABLA Y DERIVARLA ===\n");
        P("\nTenes cargada la funcion f(x) = %s\n", F);
        P("Varios parciales piden: 'genere una tabla con n sub-intervalos y despues\n");
        P("calcule df/dx con diferencias centradas en el interior y hacia adelante o\n");
        P("hacia atras en los extremos'. Eso es exactamente lo que hace esta opcion.\n");
        double a = leer_num("\na  (inicio del intervalo) = ");
        double b = leer_num("b  (fin del intervalo) = ");
        int n = leer_int("n  (cantidad de SUB-INTERVALOS, ej: 6) = ");
        if (n < 2 || n > MAXPUNTOS - 1) { P("n tiene que estar entre 2 y %d.\n", MAXPUNTOS - 1); return; }
        NP = n + 1;
        double hh = (b - a) / n;
        for (int i = 0; i < NP; i++) { X[i] = a + i * hh; Y[i] = f(X[i]); }
        P("\nTabla generada: %d puntos, h = %.10g\n", NP, hh);
    }
    P("\n=== DERIVADA EN LOS NODOS DE LA TABLA ===\n");
    P("\nEn cada punto se usa la mejor formula disponible:\n");
    P("   primer punto  -> hacia ADELANTE (no hay nada a la izquierda)\n");
    P("   ultimo punto  -> hacia ATRAS    (no hay nada a la derecha)\n");
    P("   los del medio -> CENTRADA       (la mas precisa)\n");
    int cte = 1;
    double h0 = X[1] - X[0];
    for (int i = 1; i < NP - 1; i++)
        if (fabs((X[i + 1] - X[i]) - h0) > 1e-9 * (fabs(h0) + 1)) cte = 0;
    hoja_reset();
    H("DERIVADA NUMERICA DE UNA TABLA DE DATOS\n");
    if (!cte) {
        H("\nAVISO: la tabla NO tiene paso constante. Se usan las formulas de puntos\n");
        H("desigualmente espaciados (derivada del polinomio de Lagrange de 3 puntos).\n");
    } else {
        H("\nPaso constante h = %.10g\n", h0);
    }
    H("\n    i |     x_i      |     y_i      |    formula usada      |     y'(x_i)\n");
    H("  ----+--------------+--------------+-----------------------+----------------\n");
    for (int i = 0; i < NP; i++) {
        double d; const char *nm;
        if (cte) {
            if (i == 0) {
                if (NP >= 3) { d = (-Y[2] + 4 * Y[1] - 3 * Y[0]) / (2 * h0); nm = "adelante O(h^2)"; }
                else { d = (Y[1] - Y[0]) / h0; nm = "adelante O(h)"; }
            } else if (i == NP - 1) {
                if (NP >= 3) { d = (3 * Y[i] - 4 * Y[i - 1] + Y[i - 2]) / (2 * h0); nm = "atras O(h^2)"; }
                else { d = (Y[i] - Y[i - 1]) / h0; nm = "atras O(h)"; }
            } else { d = (Y[i + 1] - Y[i - 1]) / (2 * h0); nm = "centrada O(h^2)"; }
        } else {
            /* derivada del polinomio de Lagrange de 3 puntos (i-1, i, i+1) ajustados al borde */
            int a = i - 1, b = i, c = i + 1;
            if (i == 0) { a = 0; b = 1; c = 2; }
            if (i == NP - 1) { a = NP - 3; b = NP - 2; c = NP - 1; }
            if (NP == 2) { d = (Y[1] - Y[0]) / (X[1] - X[0]); nm = "2 puntos"; }
            else {
                double xi = X[i];
                d = Y[a] * (2 * xi - X[b] - X[c]) / ((X[a] - X[b]) * (X[a] - X[c]))
                  + Y[b] * (2 * xi - X[a] - X[c]) / ((X[b] - X[a]) * (X[b] - X[c]))
                  + Y[c] * (2 * xi - X[a] - X[b]) / ((X[c] - X[a]) * (X[c] - X[b]));
                nm = "Lagrange 3 puntos";
            }
        }
        H("  %3d | %12.6g | %12.6f | %-21s | %14.8f", i, X[i], Y[i], nm, d);
        if (NDER >= 1) H("   (exacta %.8f , error %.2e)", evalf(DER[1], X[i]), fabs(d - evalf(DER[1], X[i])));
        H("\n");
    }
    if (cte && NP >= 3) {
        H("\nSEGUNDA DERIVADA en los nodos interiores (centrada, O(h^2)):\n");
        H("   y''(x_i) ~ [ y_(i+1) - 2*y_i + y_(i-1) ] / h^2\n");
        for (int i = 1; i < NP - 1; i++) {
            double d2 = (Y[i + 1] - 2 * Y[i] + Y[i - 1]) / (h0 * h0);
            H("   x = %10.6g :  [ %.6f - 2*(%.6f) + %.6f ] / %.6g = %14.8f", X[i], Y[i + 1], Y[i], Y[i - 1], h0 * h0, d2);
            if (NDER >= 2) H("   (exacta %.8f)", evalf(DER[2], X[i]));
            H("\n");
        }
    }
    /* export listo para encadenar con integracion.exe */
    H("\n--- PARA PEGAR EN integracion.exe (si hay que INTEGRAR la derivada) ---\n");
    H("Valores de x:\n");
    for (int i = 0; i < NP; i++) H("%.10g%s", X[i], i == NP - 1 ? "\n" : " ");
    H("Valores de y = f'(x):\n");
    for (int i = 0; i < NP; i++) {
        double d;
        if (cte) {
            if (i == 0 && NP >= 3)        d = (-Y[2] + 4 * Y[1] - 3 * Y[0]) / (2 * h0);
            else if (i == NP - 1 && NP >= 3) d = (3 * Y[i] - 4 * Y[i - 1] + Y[i - 2]) / (2 * h0);
            else if (i == 0)              d = (Y[1] - Y[0]) / h0;
            else if (i == NP - 1)         d = (Y[i] - Y[i - 1]) / h0;
            else                          d = (Y[i + 1] - Y[i - 1]) / (2 * h0);
        } else {
            int a = i - 1, b = i, c = i + 1;
            if (i == 0) { a = 0; b = 1; c = 2; }
            if (i == NP - 1) { a = NP - 3; b = NP - 2; c = NP - 1; }
            if (NP == 2) d = (Y[1] - Y[0]) / (X[1] - X[0]);
            else {
                double xi = X[i];
                d = Y[a] * (2 * xi - X[b] - X[c]) / ((X[a] - X[b]) * (X[a] - X[c]))
                  + Y[b] * (2 * xi - X[a] - X[c]) / ((X[b] - X[a]) * (X[b] - X[c]))
                  + Y[c] * (2 * xi - X[a] - X[b]) / ((X[c] - X[a]) * (X[c] - X[b]));
            }
        }
        H("%.10f%s", d, i == NP - 1 ? "\n" : " ");
    }
    H("\n(En integracion.exe elegi 'tengo una TABLA' y pega esas dos lineas:\n");
    H(" asi se integra la derivada, que es justo lo que piden varios parciales.)\n");
    if (NDER >= 1)
        H("\nSi hace falta la derivada como FUNCION (por ejemplo para Gauss, que evalua\n"
          "en puntos que no estan en la tabla), usar:   f'(x) = %s\n", DER[1]);
    hoja_que_escribir("Que formula se usa en cada punto y POR QUE (bordes vs interior)",
                      "El valor de h (o el aviso de que no es constante)",
                      "La tabla con las derivadas y las cuentas de la que pidan",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* EFECTO DEL TAMANO DE h                                              */
/* ------------------------------------------------------------------ */
static void op_efecto_h(void) {
    P("\n=== EFECTO DEL TAMANO DE h  (truncamiento contra redondeo) ===\n");
    if (!F[0] || NDER < 1) { P("Hace falta la funcion (y poder derivarla) para comparar.\n"); return; }
    P("\nAl bajar h, el error de TRUNCAMIENTO baja... pero el de REDONDEO sube,\n");
    P("porque se restan numeros casi iguales. Hay un h OPTIMO.\n");
    int id = 2;
    P("\n  1) Primera derivada, hacia adelante O(h)\n");
    P("  2) Primera derivada, centrada O(h^2)\n");
    P("  3) Segunda derivada, centrada O(h^2)\n");
    int op = leer_int("Opcion (1-3): ");
    if (op == 1) id = 0; else if (op == 2) id = 2; else if (op == 3) id = 8; else return;
    double x = leer_num("x  (punto donde derivar) = ");
    int orden_der = (id == 8) ? 2 : 1;
    if (NDER < orden_der) { P("No pude obtener la derivada exacta de ese orden.\n"); return; }
    double exacta = evalf(DER[orden_der], x);
    hoja_reset();
    H("EFECTO DEL TAMANO DE h   -   %s\n", NOMF[id]);
    H("f(x) = %s     x = %.10g\n", F, x);
    H("Valor exacto: %.12f\n", exacta);
    H("\n        h        |      valor aproximado    |   error absoluto  |  error(h)/error(h/10)\n");
    H("  ---------------+--------------------------+-------------------+----------------------\n");
    double eant = 0, mejor = 1e300, hmejor = 0;
    for (int k = 0; k <= 12; k++) {
        double h = pow(10.0, -(double)k);
        double v = dif(id, x, h), ea = fabs(v - exacta);
        if (ea < mejor) { mejor = ea; hmejor = h; }
        H("   %13.1e | %24.14f | %17.3e |", h, v, ea);
        if (k > 0 && ea > 0) H(" %20.2f", eant / ea);
        H("\n");
        eant = ea;
    }
    H("\nEl mejor resultado se logro con h = %.1e  (error %.3e).\n", hmejor, mejor);
    H("\nQUE PASA:\n");
    H("   - Al principio, al bajar h el error BAJA: manda el error de truncamiento,\n");
    H("     que es O(h^%d).\n", ORD[id]);
    H("   - Pasado el h optimo, el error VUELVE A SUBIR: manda el error de REDONDEO,\n");
    H("     porque f(x+h) y f(x-h) son casi iguales y al restarlos se pierden cifras\n");
    H("     significativas, y despues se divide por un h chiquito (se amplifica).\n");
    H("   - El epsilon de maquina es 2.22e-16: con h = 1e-12 practicamente no queda\n");
    H("     ninguna cifra correcta en la resta.\n");
    H("   CONCLUSION: h muy grande es malo (truncamiento) y h muy chico tambien es malo\n");
    H("   (redondeo). Esto es lo contrario de lo que pasa en integracion, donde bajar h\n");
    H("   casi siempre mejora.\n");
    hoja_que_escribir("La tabla de h contra error",
                      "Donde esta el h optimo",
                      "La explicacion de los DOS errores: truncamiento y redondeo",
                      "Por que al restar numeros parecidos se pierden cifras significativas",
                      (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
int main(void) {
    abrir_log("salida_derivacion.txt");
    P("\n####  DIFERENCIACION NUMERICA: DIFERENCIAS FINITAS   (Guia 8)  ####\n");
    P("\nTodo lo que calcula queda tambien en el archivo salida_derivacion.txt\n");
    cargar();
    for (;;) {
        P("\n======================== MENU ========================\n");
        if (MODO == 0) P("  f(x) = %s\n", F);
        else P("  TABLA de %d puntos%s\n", NP, F[0] ? " (con funcion exacta cargada)" : "");
        P("\n  OPCION                          USALA SI EL ENUNCIADO DICE...\n");
        P("  1) Cargar otros datos\n");
        P("  2) Derivada PRIMERA             'f'(x)', 'la derivada en x = ...', 'diferencias finitas'\n");
        P("  3) Derivada SEGUNDA             'f''(x)', 'la segunda derivada'\n");
        P("  4) Derivar una TABLA            'calcule la derivada en cada punto de la tabla'\n");
        P("  5) Richardson                   'Richardson', 'mejore la estimacion', 'extrapolacion'\n");
        P("  6) Tercera y cuarta derivada    'tercera derivada', 'cuarta derivada'\n");
        P("  7) Efecto del tamano de h       'que pasa si h es muy chico', 'h optimo', 'redondeo'\n");
        P("  0) Salir\n");
        int op = leer_int("Opcion: ");
        if (op == 0) break;
        switch (op) {
            case 1: cargar(); break;
            case 2: op_derivar(0); break;
            case 3: op_derivar(1); break;
            case 4: op_tabla(); break;
            case 5: op_richardson(); break;
            case 6: op_orden_superior(); break;
            case 7: op_efecto_h(); break;
            default: P("Opcion invalida.\n");
        }
        if (op >= 1 && op <= 7) pausa();
    }
    if (LOG_MN) fclose(LOG_MN);
    return 0;
}
