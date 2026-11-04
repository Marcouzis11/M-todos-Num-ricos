/* mn_comun.h -- Herramientas comunes para los programas del Parcial 1.
 *
 *  - Lee numeros y funciones escritas por teclado: "x^3 - 2*x + exp(-x)".
 *  - Todo lo que se imprime tambien se guarda en un archivo salida_*.txt
 *    (en la misma carpeta del programa) para copiarlo despues.
 *
 *  Funciones que entiende: sin sen cos tan asin acos atan sinh cosh tanh
 *  exp ln log (=ln, logaritmo natural) log10 sqrt abs.  Constantes: pi, e.
 *  Operadores: + - * / ^ y parentesis.  2x equivale a 2*x.  Acepta 0,5 o 0.5.
 *
 *  Compila con cualquier compilador C++ (Code::Blocks, Dev-C++, g++).
 */
#ifndef MN_COMUN_H
#define MN_COMUN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>

#define PI_MN 3.14159265358979323846
#define MAXN 20          /* tamano maximo de sistemas / cantidad de puntos */
#define MAXPUNTOS 60

/* ------------------------------------------------------------------ */
/* Salida por pantalla + archivo                                       */
/* ------------------------------------------------------------------ */
static FILE *LOG_MN = NULL;

static void abrir_log(const char *nombre) {
    LOG_MN = fopen(nombre, "a");
}

static void P(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    if (LOG_MN) {
        va_start(ap, fmt);
        vfprintf(LOG_MN, fmt, ap);
        va_end(ap);
        fflush(LOG_MN);
    }
    fflush(stdout);
}

/* ------------------------------------------------------------------ */
/* Bloque "TEXTO PARA COPIAR EN LA HOJA"                               */
/* ------------------------------------------------------------------ */
static char HOJA[60000];
static int HN = 0;

static void hoja_reset(void) { HN = 0; HOJA[0] = 0; }

static void H(const char *fmt, ...) {
    char tmp[1500];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(tmp, fmt, ap);
    va_end(ap);
    int n = (int)strlen(tmp);
    if (HN + n < (int)sizeof(HOJA) - 1) { strcpy(HOJA + HN, tmp); HN += n; }
}

static void linea(void);

static void hoja_mostrar(void) {
    P("\n=========================== TEXTO PARA COPIAR EN LA HOJA ===========================\n\n");
    P("%s", HOJA);
    P("\n====================================================================================\n");
    P("(Tambien quedo guardado en el archivo salida_*.txt de esta carpeta.)\n");
}

/* Lista de lo que hay que escribir en la hoja (termina con NULL) */
static void hoja_que_escribir(const char *primero, ...) {
    va_list ap;
    const char *s;
    H("\n------------------------- QUE ESCRIBIR EN LA HOJA -------------------------\n");
    va_start(ap, primero);
    for (s = primero; s; s = va_arg(ap, const char *)) H("  [ ] %s\n", s);
    va_end(ap);
}

static void linea(void) {
    P("--------------------------------------------------------------------------------------------------------\n");
}

/* ------------------------------------------------------------------ */
/* Evaluador de expresiones                                            */
/* ------------------------------------------------------------------ */
static const char *ep;   /* posicion actual en el texto */

/* ------------------------------------------------------------------ */
/* Variables con nombre, para metodos definidos por el usuario         */
/*   var_set("fi", 3.5)  y despues se puede escribir  2*fi - fim1      */
/* ------------------------------------------------------------------ */
#define MAXVARS 16
static char VNOM[MAXVARS][16];
static double VVAL[MAXVARS];
static int NVARS = 0;

static void vars_reset(void) { NVARS = 0; }

static void var_set(const char *n, double v) {
    int i;
    for (i = 0; i < NVARS; i++)
        if (!strcmp(VNOM[i], n)) { VVAL[i] = v; return; }
    if (NVARS < MAXVARS) { strncpy(VNOM[NVARS], n, 15); VNOM[NVARS][15] = 0; VVAL[NVARS] = v; NVARS++; }
}

static int var_get(const char *n, double *v) {
    int i;
    for (i = 0; i < NVARS; i++)
        if (!strcmp(VNOM[i], n)) { *v = VVAL[i]; return 1; }
    return 0;
}

static double ex;        /* valor de x */
static double ey;        /* valor de y (para f(x,y) en las EDO) */
static int eerr;         /* 1 si hubo error de sintaxis */

static double e_expr(void);

static void e_sp(void) {
    while (*ep == ' ' || *ep == '\t') ep++;
}

static double e_prim(void) {
    e_sp();
    if (*ep == '(') {
        ep++;
        double v = e_expr();
        e_sp();
        if (*ep == ')') ep++; else eerr = 1;
        return v;
    }
    if (isdigit((unsigned char)*ep) || *ep == '.') {
        char *fin;
        double v = strtod(ep, &fin);
        if (fin == ep) { eerr = 1; return 0; }
        ep = fin;
        return v;
    }
    if (isalpha((unsigned char)*ep)) {
        char nom[16];
        int n = 0;
        while (isalnum((unsigned char)*ep) && n < 15) nom[n++] = (char)tolower((unsigned char)*ep++);
        nom[n] = 0;
        if (!strcmp(nom, "x")) return ex;
        if (!strcmp(nom, "y")) return ey;
        { double vv; if (var_get(nom, &vv)) return vv; }
        if (!strcmp(nom, "pi")) return PI_MN;
        if (!strcmp(nom, "e")) return exp(1.0);
        e_sp();
        if (*ep != '(') { eerr = 1; return 0; }
        ep++;
        double a = e_expr();
        e_sp();
        if (*ep == ')') ep++; else eerr = 1;
        if (!strcmp(nom, "sin") || !strcmp(nom, "sen")) return sin(a);
        if (!strcmp(nom, "cos")) return cos(a);
        if (!strcmp(nom, "tan") || !strcmp(nom, "tg")) return tan(a);
        if (!strcmp(nom, "asin") || !strcmp(nom, "arcsin")) return asin(a);
        if (!strcmp(nom, "acos") || !strcmp(nom, "arccos")) return acos(a);
        if (!strcmp(nom, "atan") || !strcmp(nom, "arctan")) return atan(a);
        if (!strcmp(nom, "sinh")) return sinh(a);
        if (!strcmp(nom, "cosh")) return cosh(a);
        if (!strcmp(nom, "tanh")) return tanh(a);
        if (!strcmp(nom, "exp")) return exp(a);
        if (!strcmp(nom, "ln") || !strcmp(nom, "log")) return log(a);
        if (!strcmp(nom, "log10")) return log10(a);
        if (!strcmp(nom, "sqrt") || !strcmp(nom, "raiz")) return sqrt(a);
        if (!strcmp(nom, "abs")) return fabs(a);
        eerr = 1;
        return 0;
    }
    eerr = 1;
    return 0;
}

static double e_unario(void);

static double e_pot(void) {           /* a ^ b (asociativo a derecha) */
    double base = e_prim();
    e_sp();
    if (*ep == '^') {
        ep++;
        double expo = e_unario();
        return pow(base, expo);
    }
    return base;
}

static double e_unario(void) {
    e_sp();
    if (*ep == '-') { ep++; return -e_unario(); }
    if (*ep == '+') { ep++; return e_unario(); }
    return e_pot();
}

static double e_term(void) {
    double v = e_unario();
    for (;;) {
        e_sp();
        if (*ep == '*') { ep++; v *= e_unario(); }
        else if (*ep == '/') { ep++; v /= e_unario(); }
        else if (*ep == '(' || isalpha((unsigned char)*ep)) { v *= e_unario(); } /* 2x, 3(x+1) */
        else return v;
    }
}

static double e_expr(void) {
    double v = e_term();
    for (;;) {
        e_sp();
        if (*ep == '+') { ep++; v += e_term(); }
        else if (*ep == '-') { ep++; v -= e_term(); }
        else return v;
    }
}

/* Evalua el texto s con x = valor. */
static double evalf(const char *s, double x) {
    ep = s; ex = x; eerr = 0;
    return e_expr();
}

/* Evalua una expresion con DOS variables: f(x, y)  (para las EDO) */
static double evalf2(const char *s, double x, double y) {
    ep = s; ex = x; ey = y; eerr = 0;
    return e_expr();
}

static int expr_valida(const char *s) {
    if (!s[0]) return 0;
    ep = s; ex = 1.2345; ey = 0.6789; eerr = 0;
    e_expr();
    e_sp();
    return !eerr && *ep == 0;
}

#include "mn_derivada.h"

/* ------------------------------------------------------------------ */
/* Lectura por teclado                                                 */
/* ------------------------------------------------------------------ */
static void leer_linea(const char *msg, char *buf, int n) {
    printf("%s", msg);
    fflush(stdout);
    if (!fgets(buf, n, stdin)) exit(0);     /* se cerro la entrada: salir */
    buf[strcspn(buf, "\r\n")] = 0;
    for (char *p = buf; *p; p++) if (*p == ',') *p = '.';
    if (LOG_MN) fprintf(LOG_MN, "%s%s\n", msg, buf);
}

/* Lee un numero. Acepta cuentas: 1e-4, pi/2, 10^-6, 3.2 */
static double leer_num(const char *msg) {
    char b[256];
    for (;;) {
        leer_linea(msg, b, 256);
        if (b[0]) {
            ep = b; ex = 0; eerr = 0;
            double v = e_expr();
            e_sp();
            if (!eerr && *ep == 0) return v;
        }
        printf("   No entendi el numero. Ejemplos: 3.2   1e-4   pi/2\n");
    }
}

/* Igual que leer_num pero si se aprieta ENTER sin escribir devuelve 0 en *hay. */
static double leer_num_opcional(const char *msg, int *hay) {
    char b[256];
    for (;;) {
        leer_linea(msg, b, 256);
        if (!b[0]) { *hay = 0; return 0; }
        ep = b; ex = 0; eerr = 0;
        double v = e_expr();
        e_sp();
        if (!eerr && *ep == 0) { *hay = 1; return v; }
        printf("   No entendi el numero.\n");
    }
}

static int leer_int(const char *msg) {
    return (int)floor(leer_num(msg) + 0.5);
}

static void leer_funcion(const char *msg, char *dst) {
    for (;;) {
        leer_linea(msg, dst, 256);
        if (expr_valida(dst)) return;
        printf("   No entendi la funcion. Ejemplo:  x^3 - 2*x + exp(-x)   (usa * para multiplicar)\n");
    }
}

static void pausa(void) {
    char b[64];
    printf("\nPresiona ENTER para volver al menu...");
    fflush(stdout);
    if (!fgets(b, 64, stdin)) return;
}

/* Reemplaza la variable x por un numero, para mostrar la cuenta como se hace a mano.
   Ej: "ln(x^2+1) - sin(x)" con 1.5  ->  "ln((1.5)^2+1) - sin((1.5))"  */
static void sustituir(const char *expr, double val, char *out) {
    char num[64];
    sprintf(num, "(%.6g)", val);
    int o = 0, i = 0;
    while (expr[i] && o < 460) {
        if (isalpha((unsigned char)expr[i])) {
            int j = i;
            while (isalnum((unsigned char)expr[j])) j++;
            if (j - i == 1 && (expr[i] == 'x' || expr[i] == 'X')) {
                for (int k = 0; num[k]; k++) out[o++] = num[k];
            } else {
                for (int k = i; k < j; k++) out[o++] = expr[k];
            }
            i = j;
        } else out[o++] = expr[i++];
    }
    out[o] = 0;
}

/* ------------------------------------------------------------------ */
/* Detalle de las funciones internas, para el desarrollo "a mano".      */
/* Para  exp(x)*sin(x)  con x = 0.5  arma el texto:                     */
/*      exp(0.5) = 1.6487212707     sin(0.5) = 0.4794255386            */
/* Devuelve cuantas funciones distintas encontro.                       */
/* ------------------------------------------------------------------ */
static int detalle_funciones(const char *expr, double x, char *out) {
    static const char *FN[] = {"asin", "acos", "atan", "sinh", "cosh", "tanh",
                               "log10", "sqrt", "raiz", "sin", "sen", "cos",
                               "tan", "exp", "ln", "log", "abs", (const char *)0};
    char vistos[12][140];
    int nv = 0, o = 0;
    out[0] = 0;
    for (int i = 0; expr[i]; i++) {
        if (!isalpha((unsigned char)expr[i])) continue;
        if (i > 0 && isalnum((unsigned char)expr[i - 1])) continue;
        int L = 0;
        while (isalnum((unsigned char)expr[i + L])) L++;
        int k = -1;
        for (int j = 0; FN[j]; j++)
            if ((int)strlen(FN[j]) == L && !strncmp(expr + i, FN[j], L)) { k = j; break; }
        if (k < 0) { i += L - 1; continue; }
        int p = i + L;
        while (expr[p] == ' ') p++;
        if (expr[p] != '(') { i += L - 1; continue; }
        int d = 0, q = p;
        while (expr[q]) {                      /* parentesis que cierra */
            if (expr[q] == '(') d++;
            else if (expr[q] == ')') { d--; if (!d) break; }
            q++;
        }
        if (!expr[q]) break;
        int na = q - p - 1, nl = q - i + 1;
        if (na > 150 || nl > 150 || na < 1) { i = q; continue; }
        char arg[160], llam[180], linea[140];
        strncpy(arg, expr + p + 1, na); arg[na] = 0;
        strncpy(llam, expr + i, nl);    llam[nl] = 0;
        double va = evalf(arg, x);
        double vf = evalf(llam, x);
        sprintf(linea, "%.*s(%.10g) = %.10f", L, expr + i, va, vf);
        int rep = 0;
        for (int j = 0; j < nv; j++) if (!strcmp(vistos[j], linea)) rep = 1;
        if (!rep && nv < 12 && o < 880) {
            strcpy(vistos[nv++], linea);
            o += sprintf(out + o, "%s%s", nv > 1 ? "     " : "", linea);
        }
        i = q;
    }
    return nv;
}

/* ------------------------------------------------------------------ */
/* Errores                                                             */
/* ------------------------------------------------------------------ */
/* Criterio (#) del IUA:  100 * |x_nuevo - x_viejo| / ( 1/2 |x_nuevo + x_viejo| ) */
static double eps_iua(double xn, double xv) {
    double den = 0.5 * fabs(xn + xv);
    if (den == 0.0) return (xn == xv) ? 0.0 : 1e300;
    return 100.0 * fabs(xn - xv) / den;
}

#endif
