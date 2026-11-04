/* raices.cpp -- RAICES DE ECUACIONES f(x) = 0   (Guias 1 y 2)
 *
 *  Biseccion, Regula Falsi, Punto Fijo, Newton-Raphson, Newton con derivada
 *  aproximada, Secante y busqueda de cambios de signo.
 *
 *  Ademas de la tabla, cada metodo imprime un bloque
 *      "TEXTO PARA COPIAR EN LA HOJA"
 *  con la justificacion, la formula, la primera iteracion con numeros,
 *  el resultado y el error, listo para transcribir en el parcial.
 *  Todo queda guardado en salida_raices.txt
 */
#include "mn_comun.h"

static char F[256] = "";
static double XEX = 0;        /* valor exacto de la raiz, si el enunciado lo da */
static int HAYXEX = 0;
static int CRIT;        /* 1 IUA%, 2 absoluto, 3 |f(x)|, 4 cantidad fija */
static double TOL;
static int NITER;

/* ------------------------------------------------------------------ */
/* Bloque de texto para el parcial                                     */
/* ------------------------------------------------------------------ */
static void hoja_final(double x, int k, double dx, double e_pct, int corto) {
    double fv = evalf(F, x);
    H("\nRESULTADO tras %d iteracion%s:\n", k, k == 1 ? "" : "es");
    H("   raiz aproximada   x = %.10f\n\n", x);
    H("   ERRORES (cada uno mide una cosa distinta):\n");
    H("   1) Error absoluto ESTIMADO = |x_k - x_(k-1)| = %.6e\n", dx);
      H("      Es la diferencia con el valor de la iteracion anterior. Es una ESTIMACION:\n");
      H("      no se conoce la raiz verdadera, se supone que el metodo ya esta cerca.\n");
    H("   2) Error relativo porcentual (formula # del parcial) = %.6f %%\n", e_pct);
      H("      Es el error estimado dividido por el promedio de los dos valores, en %%.\n");
    H("   3) Residuo |f(x)| = %.3e\n", fabs(fv));
      H("      NO es un error de x: mide cuanto le falta a la ecuacion para cumplirse.\n");
    if (HAYXEX) {
        double ea = fabs(x - XEX);
        H("   4) Error absoluto EXACTO (o verdadero) = |x - x_exacta| = |%.10f - %.10f| = %.6e\n", x, XEX, ea);
        H("      Solo se puede calcular porque el enunciado da la raiz exacta x = %.10f.\n", XEX);
        if (XEX != 0) H("   5) Error relativo EXACTO = 100*|x - x_exacta|/|x_exacta| = %.6f %%\n", 100 * ea / fabs(XEX));
        H("      Comparacion: el error estimado (%.3e) %s el error exacto (%.3e), como corresponde.\n",
          dx, dx >= ea ? "es mayor que" : "es menor que", ea);
    } else {
        H("   4) Error absoluto EXACTO: no se puede calcular, porque no se conoce la raiz verdadera.\n");
        H("      (Si el enunciado la da, se calcula como |x_calculado - x_exacto|.)\n");
    }
    if (!corto) H("   ATENCION: se corto por maximo de iteraciones, sin alcanzar la tolerancia.\n");
}

/* ------------------------------------------------------------------ */
static void pedir_criterio(void) {
    P("\nCriterio de corte:\n");
    P("  1) Error IUA en %%   e = 100 |x_nuevo - x_viejo| / ( 1/2 |x_nuevo + x_viejo| )\n");
    P("  2) Error absoluto   |x_nuevo - x_viejo|\n");
    P("  3) Residuo          |f(x)|\n");
    P("  4) Hacer una cantidad FIJA de iteraciones (ej: \"calcular hasta la iteracion 4\")\n");
    do { CRIT = leer_int("Opcion (1-4): "); } while (CRIT < 1 || CRIT > 4);
    if (CRIT == 4) {
        NITER = leer_int("Cantidad de iteraciones a realizar  (ej: 10): ");
        TOL = -1;
    } else {
        if (CRIT == 1) TOL = leer_num("Tolerancia en %  (error maximo admitido, ej: 0.01): ");
        else TOL = leer_num("Tolerancia  (error maximo admitido, ej: 1e-4): ");
        NITER = 500;
    }
    XEX = leer_num_opcional("Valor EXACTO de la raiz, si el enunciado lo da (ENTER = no se conoce): ", &HAYXEX);
}

static void hoja_criterio(void) {
    if (CRIT == 1) H("Criterio de corte: error porcentual (formula #) menor que %g %%.\n\n", TOL);
    else if (CRIT == 2) H("Criterio de corte: error absoluto |x_nuevo - x_viejo| menor que %g.\n\n", TOL);
    else if (CRIT == 3) H("Criterio de corte: residuo |f(x)| menor que %g.\n\n", TOL);
    else H("Criterio de corte: realizar %d iteraciones.\n\n", NITER);
}

/* hay_err = 0 si en esta iteracion no hay valor viejo para comparar */
static int cortar(int k, int hay_err, double e_pct, double dx, double fx) {
    if (CRIT == 4) return k >= NITER;
    if (fx == 0.0) return 1;
    if (CRIT == 3) return fabs(fx) < TOL;
    if (!hay_err) return 0;
    if (CRIT == 1) return e_pct < TOL;
    return dx < TOL;
}

static void resumen(double x, int k, int hay, double e_pct, double dx, int corto) {
    linea();
    if (!corto) P("ATENCION: se llego al maximo de %d iteraciones SIN cumplir la tolerancia.\n", k);
    P("RAIZ APROXIMADA  x = %.10f\n", x);
    P("f(x)             = %.6e\n", evalf(F, x));
    P("Iteraciones      = %d\n", k);
    if (hay) {
        P("Error IUA (%%)    = %.6e %%\n", e_pct);
        P("Error |dx|       = %.6e\n", dx);
    }
    linea();
}

/* ------------------------------------------------------------------ */
static void cerrados(int regula) {
    const char *nombre = regula ? "REGULA FALSI (falsa posicion)" : "BISECCION";
    P("\n=== %s ===   f(x) = %s\n", nombre, F);
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   xl = extremo IZQUIERDO del intervalo   (x inferior)\n");
    P("   xu = extremo DERECHO del intervalo     (x superior)\n");
    P("   xr = RAIZ APROXIMADA de esa iteracion  (es la respuesta del metodo)\n");
    P("   f(xr) = cuanto vale la funcion en xr   (si diera 0, xr seria la raiz exacta)\n");
    P("   |dx| = error absoluto: cuanto cambio xr respecto del valor anterior\n");
    P("   eps %% = ese mismo error en porcentaje (formula # del parcial)\n");
    double xl = leer_num("xl  (x inferior = extremo IZQUIERDO del intervalo): ");
    double xu = leer_num("xu  (x superior = extremo DERECHO del intervalo): ");
    if (xl > xu) { double t = xl; xl = xu; xu = t; }
    double xl0 = xl, xu0 = xu;
    double fl = evalf(F, xl), fu = evalf(F, xu);
    P("f(xl) = f(%.10f) = %.10f\n", xl, fl);
    P("f(xu) = f(%.10f) = %.10f\n", xu, fu);
    if (fl * fu > 0) {
        P("ATENCION: f(xl)*f(xu) > 0  ->  NO hay cambio de signo. El metodo no se puede aplicar en este intervalo.\n");
        P("Usa la opcion 7 del menu para buscar un intervalo con cambio de signo.\n");
        return;
    }
    P("f(xl)*f(xu) < 0  ->  hay al menos una raiz en [xl, xu] (Teorema de Bolzano).\n");
    int hay;
    double xv = leer_num_opcional("Valor 'viejo' para el error de la 1ra iteracion (si el enunciado lo da; ENTER = usar xu): ", &hay);
    if (!hay) { xv = xu0; hay = 1; }   /* convencion de la catedra: x_viejo = xu */
    pedir_criterio();

    hoja_reset();
    H("METODO DE %s      f(x) = %s\n\n", nombre, F);
    H("NOTACION: xl = extremo izquierdo del intervalo, xu = extremo derecho, xr = raiz\n");
    H("aproximada de la iteracion, f(xr) = valor de la funcion en xr, |dx| = error absoluto.\n\n");
    H("Datos:  xl = %.10g      xu = %.10g\n", xl0, xu0);
    {
        char sb[512];
        sustituir(F, xl0, sb);
        H("f(xl) = f(%.10g) = %s = %.10f   (%s)\n", xl0, sb, fl, fl < 0 ? "negativo" : "positivo");
        sustituir(F, xu0, sb);
        H("f(xu) = f(%.10g) = %s = %.10f   (%s)\n\n", xu0, sb, fu, fu < 0 ? "negativo" : "positivo");
    }
    H("Como f(xl)*f(xu) < 0 y f es continua, por el teorema de Bolzano existe al menos\n");
    H("una raiz en el intervalo (%.10g , %.10g).\n\n", xl0, xu0);
    if (regula) H("Formula:  xr = xu - f(xu)*(xl - xu) / ( f(xl) - f(xu) )\n");
    else        H("Formula:  xr = (xl + xu) / 2\n");
    H("Regla:    si f(xl)*f(xr) < 0  ->  xu = xr ;  si no  ->  xl = xr\n");
    hoja_criterio();

    P("\n  k |        xl        |        xu        |        xr        |      f(xr)      |   error IUA %%  |     |dx|\n");
    linea();
    double xr = 0, e = 0, dx = 0;
    int k, corto = 0;
    for (k = 1; k <= NITER; k++) {
        if (regula) xr = xu - fu * (xl - xu) / (fl - fu);
        else xr = 0.5 * (xl + xu);
        double fr = evalf(F, xr);
        e = eps_iua(xr, xv);
        dx = fabs(xr - xv);
        if (k == 1) {
            if (regula)
                P("  (a mano) xr = xu - f(xu)(xl-xu)/(f(xl)-f(xu)) = %.10f - (%.10f)(%.10f)/(%.10f) = %.10f\n",
                  xu, fu, xl - xu, fl - fu, xr);
            else
                P("  (a mano) xr = (xl + xu)/2 = (%.10f + %.10f)/2 = %.10f\n", xl, xu, xr);
            P("           f(xr) = %.10f ;  f(xl)*f(xr) %s 0  ->  %s\n", fr,
              fl * fr < 0 ? "<" : ">=", fl * fr < 0 ? "la raiz esta en [xl, xr]: xu = xr" : "la raiz esta en [xr, xu]: xl = xr");
            H("Iteracion 1:\n");
            if (regula)
                H("   xr = %.10g - (%.10f)*(%.10g - %.10g) / (%.10f - %.10f) = %.10f\n",
                  xu, fu, xl, xu, fl, fu, xr);
            else
                H("   xr = (%.10g + %.10g)/2 = %.10f\n", xl, xu, xr);
            {
                char sb[512];
                sustituir(F, xr, sb);
                H("   f(xr) = f(%.10f) = %s = %.10f   (%s)\n", xr, sb, fr, fr < 0 ? "negativo" : "positivo");
            }
            H("   f(xl)*f(xr) %s 0  =>  la raiz esta en [%.10g , %.10g]  =>  %s = xr\n",
              fl * fr < 0 ? "<" : ">", fl * fr < 0 ? xl : xr, fl * fr < 0 ? xr : xu,
              fl * fr < 0 ? "xu" : "xl");
            H("   Raiz obtenida en la 1ra iteracion: xr = %.10f\n", xr);
            H("   Error absoluto estimado: |xr - x_viejo| = |%.10f - %.10g| = %.10f\n", xr, xv, dx);
            H("      (x_viejo es %s)\n", hay ? "el valor de referencia elegido" : "el extremo xu");
            H("   Error porcentual: 100*|%.10f - %.10g| / (0.5*|%.10f + %.10g|) = %.6f %%\n\n", xr, xv, xr, xv, e);
            H("   Nota: en la 1ra iteracion no hay un xr anterior, por eso se toma como valor\n");
            H("   viejo el extremo %s = %.10g. Las dos referencias posibles son:\n", (fabs(xv - xu0) < 1e-12) ? "xu" : ((fabs(xv - xl0) < 1e-12) ? "xl" : "elegido"), xv);
            H("      |xr - xl| = |%.10f - %.10g| = %.10f   (cuanto se movio xr desde el extremo izquierdo)\n", xr, xl0, fabs(xr - xl0));
            H("      |xr - xu| = |%.10f - %.10g| = %.10f   (cuanto se movio xr desde el extremo derecho)\n", xr, xu0, fabs(xr - xu0));
            H("   El intervalo que queda despues de esta iteracion es [%.10f , %.10f], de ancho %.10f:\n", (fl * fr < 0) ? xl : xr, (fl * fr < 0) ? xr : xu, fabs(((fl * fr < 0) ? xr : xu) - ((fl * fr < 0) ? xl : xr)));
            H("   ese ancho es la COTA del error (la raiz esta seguro adentro). En biseccion coincide\n");
            H("   con |xr - xu|; en regula falsi no tiene por que coincidir.\n");
        }
        P("%3d | %16.10f | %16.10f | %16.10f | %15.6e | %14.6e | %12.4e\n", k, xl, xu, xr, fr, e, dx);
        if (k == 1) {
            H("\nTabla de iteraciones:\n");
            if (HAYXEX)
                H("   k |        xl        |        xu        |        xr        |    f(xr)    |    |dx|     |   eps %%    | err exacto\n");
            else
                H("   k |        xl        |        xu        |        xr        |    f(xr)    |    |dx|     |   eps %%\n");
        }
        if (HAYXEX)
            H("%4d | %16.10f | %16.10f | %16.10f | %12.4e | %11.4e | %10.5f | %11.4e\n", k, xl, xu, xr, fr, dx, e, fabs(xr - XEX));
        else
            H("%4d | %16.10f | %16.10f | %16.10f | %12.4e | %11.4e | %10.5f\n", k, xl, xu, xr, fr, dx, e);
        int fin = cortar(k, hay, e, dx, fr);
        if (fl * fr < 0) { xu = xr; fu = fr; } else { xl = xr; fl = fr; }
        xv = xr;
        if (fin) { corto = 1; break; }
    }
    if (k > NITER) k = NITER;
    resumen(xr, k, hay, e, dx, corto);
    H("Intervalo final: [%.10f , %.10f]\n", xl, xu);
    hoja_final(xr, k, dx, e, corto);
    hoja_que_escribir("La justificacion del cambio de signo: f(xl) y f(xu) con sus signos (teorema de Bolzano)", "La formula del metodo y la regla para elegir el nuevo intervalo", "La 1ra iteracion con los numeros: xr, f(xr), signo de f(xl)*f(xr) y el nuevo intervalo", "La raiz obtenida y el error absoluto estimado, aclarando que valor viejo se uso", "La tabla de iteraciones (o la captura de pantalla si la piden)", "El resultado final con 10 decimales, la cantidad de iteraciones y el error", "Si el enunciado da la raiz exacta: el error exacto", (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void punto_fijo(void) {
    char G[256];
    P("\n=== PUNTO FIJO ===\n");
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   g(x) = la ecuacion despejada como x = g(x)\n");
    P("   x_viejo = valor de la iteracion anterior ;  x_nuevo = g(x_viejo)\n");
    P("   g'(x) = pendiente de g: si |g'| < 1 cerca de la raiz, el metodo converge\n");
    P("Hay que despejar f(x)=0 como x = g(x).  Converge si |g'(x)| < 1 cerca de la raiz.\n");
    P("Si no se te ocurre un despeje, uno que casi siempre anda es  g(x) = x - f(x)/c  con c ~ f'(raiz).\n");
    leer_funcion("g(x) =  (el despeje, o sea la ecuacion escrita como x = g(x)): ", G);
    double xv = leer_num("x0  (valor inicial; conviene elegirlo cerca de la raiz): ");
    pedir_criterio();
    double x0 = xv;
    char GDER[600];
    int hay_gder = derivar_texto(G, GDER);
    if (hay_gder) P("DERIVADA CALCULADA POR EL PROGRAMA:  g'(x) = %s\n", GDER);
    double gp0 = (evalf(G, xv + 1e-6) - evalf(G, xv - 1e-6)) / 2e-6;
    P("g'(x0) ~ %.6f  (ojo: lo que decide la convergencia es |g'| cerca de la RAIZ, mira la ultima columna)\n", gp0);

    hoja_reset();
    H("METODO DE PUNTO FIJO      f(x) = %s\n\n", F);
    H("Se reescribe f(x) = 0 como x = g(x), con  g(x) = %s\n", G);
    H("Formula de iteracion:  x_(k+1) = g(x_k)\n");
    if (hay_gder) H("Derivada de g (calculada por el programa):  g'(x) = %s\n", GDER);
    H("Condicion de convergencia: |g'(x)| < 1 en un entorno de la raiz.\n\n");
    hoja_criterio();

    P("\n  k |     x_viejo      |  x_nuevo = g(x)  |   error IUA %%  |     |dx|     |   g'(x_nuevo)\n");
    linea();
    double xn = xv, e = 0, dx = 0, gp = 0;
    int k, corto = 0;
    for (k = 1; k <= NITER; k++) {
        xn = evalf(G, xv);
        e = eps_iua(xn, xv);
        dx = fabs(xn - xv);
        gp = (evalf(G, xn + 1e-6) - evalf(G, xn - 1e-6)) / 2e-6;
        if (k == 1) {
            P("  (a mano) x1 = g(x0) = g(%.10f) = %.10f\n", xv, xn);
            H("Iteracion 1 (x0 = %.10g):\n", x0);
            {
                char sb[512];
                sustituir(G, x0, sb);
                H("   x1 = g(x0) = g(%.10g) = %s = %.10f\n", x0, sb, xn);
            }
            H("   Error absoluto estimado: |x1 - x0| = %.10f\n", dx);
            H("   Error porcentual: 100*|%.10f - %.10g| / (0.5*|%.10f + %.10g|) = %.6f %%\n\n", xn, x0, xn, x0, e);
        }
        P("%3d | %16.10f | %16.10f | %14.6e | %12.4e | %12.6f\n", k, xv, xn, e, dx, gp);
        if (k == 1) {
            H("\nTabla de iteraciones:\n");
            H("   k |     x_viejo      |   x_nuevo = g(x)  |    |dx|     |   eps %%     |  g'(x_nuevo)\n");
        }
        H("%4d | %16.10f | %17.10f | %11.4e | %10.5f | %11.6f\n", k, xv, xn, dx, e, gp);
        if (!(fabs(xn) < 1e12)) { P("DIVERGE: los valores se van a infinito o se salen del dominio.\n"); break; }
        if (cortar(k, 1, e, dx, evalf(F, xn))) { corto = 1; break; }
        xv = xn;
    }
    if (k > NITER) k = NITER;
    linea();
    if (!corto) P("ATENCION: no se cumplio la tolerancia (diverge u oscila).\n");
    P("PUNTO FIJO  x = %.10f    iteraciones = %d    error IUA = %.6e %%    |dx| = %.6e\n", xn, k, e, dx);
    P("|g'(x)| = %.6f  %s\n", fabs(gp), fabs(gp) < 1 ? "< 1  (condicion de convergencia OK)" : ">= 1  (NO cumple la condicion de convergencia)");
    linea();
    H("Verificacion de la convergencia: g'(x) evaluada cerca del ultimo valor da %.6f,\n", gp);
    if (fabs(gp) < 1)
        H("y |g'| = %.6f < 1, por lo tanto el metodo CONVERGE con este despeje.\n", fabs(gp));
    else
        H("y |g'| = %.6f >= 1, por lo tanto este despeje NO sirve: hay que elegir otro g(x).\n", fabs(gp));
    hoja_final(xn, k, dx, e, corto);
    hoja_que_escribir("El despeje x = g(x) que se uso", "g'(x) y la condicion de convergencia |g'(x)| < 1 cerca de la raiz", "La 1ra iteracion con numeros: x1 = g(x0)", "La tabla y el resultado con 10 decimales", "La conclusion: converge o no, y por que", (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
/* tipo_d: 0 = derivada escrita, 1 = centrada, 2 = atras 3 ptos, 3 = adelante 3 ptos */
static double derivada(const char *FP, int tipo_d, double h, double x) {
    if (tipo_d == 0) return evalf(FP, x);
    if (tipo_d == 1) return (evalf(F, x + h) - evalf(F, x - h)) / (2 * h);
    if (tipo_d == 2) return (3 * evalf(F, x) - 4 * evalf(F, x - h) + evalf(F, x - 2 * h)) / (2 * h);
    return (-3 * evalf(F, x) + 4 * evalf(F, x + h) - evalf(F, x + 2 * h)) / (2 * h);
}

static void newton(int aproximada) {
    char FP[256] = "";
    int tipo_d = 0;
    double h = 1e-6;
    P("\n=== NEWTON-RAPHSON%s ===   f(x) = %s\n", aproximada ? " con DERIVADA APROXIMADA" : "", F);
    P("x_(k+1) = x_k - f(x_k) / f'(x_k)\n");
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   x0 = valor inicial ;  x_k = valor de la iteracion k ;  x_(k+1) = el siguiente\n");
    P("   f'(x) = derivada de f (pendiente de la recta tangente)\n");
    P("   |dx| = |x_(k+1) - x_k| = error absoluto estimado\n");
    if (!aproximada) {
        leer_linea("f'(x) = (ENTER para que la calcule el programa, o escribila vos) ", FP, 256);
        if (!FP[0]) {
            char DER[600];
            if (derivar_texto(F, DER)) {
                strncpy(FP, DER, 255); FP[255] = 0;
                P("DERIVADA CALCULADA POR EL PROGRAMA:  f'(x) = %s\n", FP);
                P("(se aplican las reglas: (u+v)' = u'+v' ;  (u*v)' = u'v+uv' ;  (u/v)' = (u'v-uv')/v^2 ;\n");
                P(" cadena: [f(u)]' = f'(u)*u' ;  [ln u]' = u'/u ;  [sin u]' = cos(u)*u' ;  [u^k]' = k*u^(k-1)*u')\n");
                tipo_d = 0;
            } else {
                tipo_d = 1;
                P("No pude derivar simbolicamente esa expresion: uso derivada numerica centrada (h = 1e-6).\n");
            }
        }
        else {
            while (!expr_valida(FP)) leer_funcion("   No entendi. f'(x) = ", FP);
        }
    } else {
        P("Formula para f'(x):\n");
        P("  1) Centrada O(h^2):        (f(x+h) - f(x-h)) / (2h)\n");
        P("  2) Atras 3 puntos O(h^2):  (3f(x) - 4f(x-h) + f(x-2h)) / (2h)\n");
        P("  3) Adelante 3 ptos O(h^2): (-3f(x) + 4f(x+h) - f(x+2h)) / (2h)\n");
        do { tipo_d = leer_int("Opcion (1-3): "); } while (tipo_d < 1 || tipo_d > 3);
        h = leer_num("h  (paso para la derivada aproximada, ej: 0.01): ");
    }
    double xv = leer_num("x0  (valor inicial; conviene elegirlo cerca de la raiz): ");
    double x0 = xv;
    if (tipo_d == 0) {
        double num = (evalf(F, xv + 1e-6) - evalf(F, xv - 1e-6)) / 2e-6, esc = evalf(FP, xv);
        if (fabs(num - esc) > 1e-4 * (1 + fabs(num)))
            P("ATENCION: tu f'(x0) = %.8f pero la derivada numerica da %.8f. REVISA LA DERIVADA.\n", esc, num);
        else P("Chequeo: la derivada escrita coincide con la numerica en x0. OK\n");
    }
    pedir_criterio();

    hoja_reset();
    H("METODO DE NEWTON-RAPHSON      f(x) = %s\n\n", F);
    if (tipo_d == 0) {
        H("Derivada:  f'(x) = %s\n", FP);
        H("(obtenida aplicando las reglas de derivacion: suma, producto, cociente y regla de la cadena)\n");
    }
    else if (tipo_d == 1) H("Derivada aproximada (centrada, O(h^2), h = %g):  f'(x) ~ (f(x+h) - f(x-h))/(2h)\n", h);
    else if (tipo_d == 2) H("Derivada aproximada (atras 3 puntos, O(h^2), h = %g):  f'(x) ~ (3f(x) - 4f(x-h) + f(x-2h))/(2h)\n", h);
    else H("Derivada aproximada (adelante 3 puntos, O(h^2), h = %g):  f'(x) ~ (-3f(x) + 4f(x+h) - f(x+2h))/(2h)\n", h);
    H("Formula:  x_(k+1) = x_k - f(x_k) / f'(x_k)\n");
    H("Valor inicial: x0 = %.10g\n\n", x0);
    hoja_criterio();

    P("\n  k |      x_k-1       |    f(x_k-1)     |    f'(x_k-1)    |       x_k        |   error IUA %%  |     |dx|\n");
    linea();
    double xn = xv, e = 0, dx = 0;
    int k, corto = 0;
    for (k = 1; k <= NITER; k++) {
        double fx = evalf(F, xv), d = derivada(FP, tipo_d, h, xv);
        if (d == 0) { P("f'(x) = 0: division por cero. Elegi otro x0.\n"); break; }
        xn = xv - fx / d;
        e = eps_iua(xn, xv);
        dx = fabs(xn - xv);
        if (k == 1) {
            P("  (a mano) x1 = x0 - f(x0)/f'(x0) = %.10f - (%.10f)/(%.10f) = %.10f\n", xv, fx, d, xn);
            H("Iteracion 1:\n");
            {
                char sb[512];
                sustituir(F, x0, sb);
                H("   f(x0)  = f(%.10g) = %s = %.10f\n", x0, sb, fx);
                if (tipo_d == 0) { sustituir(FP, x0, sb); H("   f'(x0) = %s = %.10f\n", sb, d); }
                else H("   f'(x0) = %.10f   (con la formula de diferencias finitas, h = %g)\n", d, h);
            }
            H("   x1 = x0 - f(x0)/f'(x0) = %.10g - (%.10f)/(%.10f) = %.10f\n", x0, fx, d, xn);
            H("   Raiz obtenida en la 1ra iteracion: x1 = %.10f\n", xn);
            H("   Error absoluto estimado: |x1 - x0| = %.10f\n", dx);
            H("   Error porcentual: 100*|%.10f - %.10g| / (0.5*|%.10f + %.10g|) = %.6f %%\n\n", xn, x0, xn, x0, e);
        }
        P("%3d | %16.10f | %15.6e | %15.6e | %16.10f | %14.6e | %12.4e\n", k, xv, fx, d, xn, e, dx);
        if (k == 1) {
            H("\nTabla de iteraciones:\n");
            if (HAYXEX)
                H("   k |      x_(k-1)     |   f(x_(k-1))  |  f'(x_(k-1)) |        x_k       |    |dx|     |   eps %%    | err exacto\n");
            else
                H("   k |      x_(k-1)     |   f(x_(k-1))  |  f'(x_(k-1)) |        x_k       |    |dx|     |   eps %%\n");
        }
        if (HAYXEX)
            H("%4d | %16.10f | %13.5e | %12.5e | %16.10f | %11.4e | %10.5f | %11.4e\n", k, xv, fx, d, xn, dx, e, fabs(xn - XEX));
        else
            H("%4d | %16.10f | %13.5e | %12.5e | %16.10f | %11.4e | %10.5f\n", k, xv, fx, d, xn, dx, e);
        if (!(fabs(xn) < 1e12)) { P("DIVERGE.\n"); break; }
        if (cortar(k, 1, e, dx, evalf(F, xn))) { corto = 1; break; }
        xv = xn;
    }
    if (k > NITER) k = NITER;
    resumen(xn, k, 1, e, dx, corto);
    hoja_final(xn, k, dx, e, corto);
    hoja_que_escribir("La derivada f'(x) (la calcula el programa) o la formula de la derivada aproximada", "La 1ra iteracion con numeros: f(x0), f'(x0) y x1 = x0 - f(x0)/f'(x0)", "El error estimado |x1 - x0| (y el exacto si se conoce la raiz)", "La tabla hasta la iteracion pedida y el resultado con 10 decimales", "Un comentario: Newton converge cuadraticamente (duplica las cifras correctas)", (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void secante(void) {
    P("\n=== SECANTE ===   f(x) = %s\n", F);
    P("x_(k+1) = x_k - f(x_k) (x_k - x_(k-1)) / (f(x_k) - f(x_(k-1)))\n");
    P("\nQUE SIGNIFICA CADA SIMBOLO:\n");
    P("   x0 y x1 = los dos valores iniciales ;  x_(k+1) = el nuevo valor calculado\n");
    P("   no hace falta la derivada: se usa la recta que une los dos ultimos puntos\n");
    double x0 = leer_num("x0  (valor inicial; conviene elegirlo cerca de la raiz): ");
    double x1 = leer_num("x1  (segundo valor inicial, distinto de x0): ");
    double xa = x0, xb = x1;
    pedir_criterio();

    hoja_reset();
    H("METODO DE LA SECANTE      f(x) = %s\n\n", F);
    H("Formula:  x_(k+1) = x_k - f(x_k)*(x_k - x_(k-1)) / ( f(x_k) - f(x_(k-1)) )\n");
    H("No necesita derivada. Valores iniciales: x0 = %.10g , x1 = %.10g\n\n", xa, xb);
    hoja_criterio();

    P("\n  k |      x_k-1       |       x_k        |     f(x_k-1)    |      f(x_k)     |      x_k+1       |  error IUA %%  |    |dx|\n");
    linea();
    double x2 = x1, e = 0, dx = 0;
    int k, corto = 0;
    for (k = 1; k <= NITER; k++) {
        double f0 = evalf(F, x0), f1 = evalf(F, x1);
        if (f1 == f0) { P("f(x_k) = f(x_k-1): division por cero.\n"); break; }
        x2 = x1 - f1 * (x1 - x0) / (f1 - f0);
        e = eps_iua(x2, x1);
        dx = fabs(x2 - x1);
        if (k == 1) {
            P("  (a mano) x2 = x1 - f(x1)(x1-x0)/(f(x1)-f(x0)) = %.10f - (%.10f)(%.10f)/(%.10f) = %.10f\n",
              x1, f1, x1 - x0, f1 - f0, x2);
            H("Iteracion 1:\n");
            {
                char sb[512];
                sustituir(F, x0, sb); H("   f(x0) = %s = %.10f\n", sb, f0);
                sustituir(F, x1, sb); H("   f(x1) = %s = %.10f\n", sb, f1);
            }
            H("   x2 = %.10g - (%.10f)*(%.10g - %.10g) / (%.10f - %.10f) = %.10f\n", x1, f1, x1, x0, f1, f0, x2);
            H("   Error absoluto estimado: |x2 - x1| = %.10f\n", dx);
            H("   Error porcentual: %.6f %%\n\n", e);
        }
        P("%3d | %16.10f | %16.10f | %15.6e | %15.6e | %16.10f | %13.6e | %11.4e\n", k, x0, x1, f0, f1, x2, e, dx);
        if (k == 1) {
            H("\nTabla de iteraciones:\n");
            H("   k |     x_(k-1)      |       x_k        |      x_(k+1)     |    |dx|     |   eps %%\n");
        }
        H("%4d | %16.10f | %16.10f | %16.10f | %11.4e | %10.5f\n", k, x0, x1, x2, dx, e);
        if (cortar(k, 1, e, dx, evalf(F, x2))) { corto = 1; break; }
        x0 = x1;
        x1 = x2;
    }
    if (k > NITER) k = NITER;
    resumen(x2, k, 1, e, dx, corto);
    hoja_final(x2, k, dx, e, corto);
    hoja_que_escribir("Los dos valores iniciales x0 y x1", "La formula y la 1ra iteracion con numeros", "La tabla y el resultado con 10 decimales", "Un comentario: no necesita la derivada", (const char *)0);
    hoja_mostrar();
}

/* ------------------------------------------------------------------ */
static void escanear(void) {
    P("\n=== TABLA DE f(x) Y CAMBIOS DE SIGNO ===   f(x) = %s\n", F);
    double a = leer_num("Desde a  (inicio del rango a explorar) = ");
    double b = leer_num("Hasta b  (fin del rango) = ");
    int n = leer_int("Cantidad de pasos  (en cuantos pedazos dividir el rango, ej: 20): ");
    if (n < 1) n = 20;
    double h = (b - a) / n, xa = a, fa = evalf(F, a);
    P("\n        x         |       f(x)\n");
    P("%16.10f  | %15.6e\n", xa, fa);
    int cambios = 0;
    for (int i = 1; i <= n; i++) {
        double xb = a + i * h, fb = evalf(F, xb);
        int cambio = (fa * fb < 0);
        P("%16.10f  | %15.6e %s\n", xb, fb, cambio ? "   <-- CAMBIO DE SIGNO: hay raiz entre este x y el anterior" : "");
        if (cambio) cambios++;
        xa = xb;
        fa = fb;
    }
    P("Cambios de signo encontrados: %d\n", cambios);
    P("(Si f no es continua, un cambio de signo puede ser una asintota y no una raiz.)\n");
}

static void evaluar(void) {
    double x = leer_num("x  (punto donde evaluar f) = ");
    P("f(%.10f) = %.10f\n", x, evalf(F, x));
}

/* ------------------------------------------------------------------ */
/* JUSTIFICACIONES listas para copiar en la hoja                       */
/* ------------------------------------------------------------------ */
static double dnum(const char *g, double x) { return (evalf(g, x + 1e-5) - evalf(g, x - 1e-5)) / 2e-5; }
static double d2num(const char *g, double x) { return (evalf(g, x + 1e-4) - 2 * evalf(g, x) + evalf(g, x - 1e-4)) / 1e-8; }

static void just_bolzano(void) {
    double a = leer_num("a  (extremo izquierdo del intervalo) = ");
    double b = leer_num("b  (extremo derecho del intervalo) = ");
    if (a > b) { double t = a; a = b; b = t; }
    int N = 2000, malos = 0, cambios = 0, fp_cambia = 0, signo_fp = 0;
    double fa = evalf(F, a), fb = evalf(F, b), fant = fa;
    for (int i = 0; i <= N; i++) {
        double x = a + (b - a) * i / N, y = evalf(F, x);
        int def = (y == y) && (fabs(y) < 1e300);
        if (!def) malos++;
        if (i > 0 && def && fabs(fant) < 1e300 && fant * y < 0) cambios++;
        if (def) {
            double d = dnum(F, x);
            int s = (d > 1e-12) - (d < -1e-12);
            if (!signo_fp) signo_fp = s;
            else if (s && s != signo_fp) fp_cambia = 1;
        }
        fant = y;
    }
    hoja_reset();
    H("JUSTIFICACION DE LA EXISTENCIA DE RAIZ EN EL INTERVALO [%.10g , %.10g]\n\n", a, b);
    H("f(x) = %s\n\n", F);
    if (malos == 0)
        H("1) CONTINUIDAD: f es continua en [%.10g , %.10g]. Se evaluo en %d puntos del intervalo\n"
          "   y en todos quedo definida (no hay division por cero, ni raiz de numero negativo,\n"
          "   ni logaritmo de numero no positivo).\n\n", a, b, N + 1);
    else
        H("1) ATENCION: f NO esta definida en %d de los %d puntos evaluados del intervalo.\n"
          "   Revisa el dominio: los metodos necesitan que f sea continua en [a , b].\n\n", malos, N + 1);
    H("2) VALORES EN LOS EXTREMOS:\n");
    H("   f(%.10g) = %.10f   (%s)\n", a, fa, fa < 0 ? "negativo" : "positivo");
    H("   f(%.10g) = %.10f   (%s)\n\n", b, fb, fb < 0 ? "negativo" : "positivo");
    if (fa * fb < 0) {
        H("3) CONCLUSION: como f(a)*f(b) = %.6e es MENOR QUE CERO y f es continua en [a , b],\n", fa * fb);
        H("   por el TEOREMA DE BOLZANO (o del valor intermedio) existe al menos un valor c en el\n");
        H("   intervalo (%.10g , %.10g) tal que f(c) = 0. Por lo tanto SI hay raiz, y se pueden\n", a, b);
        H("   aplicar los metodos cerrados (biseccion y regula falsi).\n\n");
        H("4) UNICIDAD: dentro del intervalo se detecto %d cambio%s de signo de f, y la derivada\n", cambios, cambios == 1 ? "" : "s");
        H("   %s de signo, de modo que la raiz %s.\n",
          fp_cambia ? "SI cambia" : "NO cambia (f es monotona en el intervalo)",
          (!fp_cambia && cambios == 1) ? "es UNICA en el intervalo" : "podria no ser unica: conviene achicar el intervalo");
    } else {
        H("3) CONCLUSION: como f(a)*f(b) = %.6e es MAYOR QUE CERO, no se cumple la hipotesis de\n", fa * fb);
        H("   cambio de signo: el teorema de Bolzano no garantiza la existencia de raiz y NO se\n");
        H("   pueden aplicar los metodos cerrados en ese intervalo (puede no haber raices, o\n");
        H("   haber una cantidad par de ellas).\n");
        if (cambios) H("   Se detectaron %d cambios de signo internos: usa la opcion 7 para localizarlos.\n", cambios);
    }
    hoja_que_escribir("Este texto completo: continuidad, valores en los extremos con sus signos y la conclusion por Bolzano", "Si piden grafico: el PNG de la graficadora con el cruce marcado", (const char *)0);
    hoja_mostrar();
}

static void just_punto_fijo(void) {
    char G[256];
    P("\nSe verifican las dos condiciones del teorema del punto fijo en un intervalo [a , b].\n");
    leer_funcion("g(x) =  (el despeje, o sea la ecuacion escrita como x = g(x)): ", G);
    double a = leer_num("a  (extremo izquierdo del intervalo) = ");
    double b = leer_num("b  (extremo derecho del intervalo) = ");
    if (a > b) { double t = a; a = b; b = t; }
    int N = 1000;
    double gmin = 1e300, gmax = -1e300, gpmax = 0, xgpmax = a;
    for (int i = 0; i <= N; i++) {
        double x = a + (b - a) * i / N, gv = evalf(G, x), d = fabs(dnum(G, x));
        if (gv == gv && fabs(gv) < 1e300) { if (gv < gmin) gmin = gv; if (gv > gmax) gmax = gv; }
        if (d == d && d > gpmax) { gpmax = d; xgpmax = x; }
    }
    double x = 0.5 * (a + b);
    int diverge = 0;
    for (int i = 0; i < 300; i++) {
        double xn = evalf(G, x);
        if (!(xn == xn) || !(fabs(xn) < 1e12)) { diverge = 1; break; }
        if (fabs(xn - x) < 1e-13) { x = xn; break; }
        x = xn;
    }
    int dentro = (x >= a - 1e-9 && x <= b + 1e-9);
    double gp_raiz = dnum(G, x);
    hoja_reset();
    H("JUSTIFICACION DE LA APLICABILIDAD DEL METODO DE PUNTO FIJO\n\n");
    H("Se reescribe f(x) = 0 como x = g(x), con  g(x) = %s\n", G);
    H("Intervalo analizado: [%.10g , %.10g]\n\n", a, b);
    H("CONDICION 1: g debe llevar el intervalo dentro de si mismo.\n");
    H("   Evaluando g en %d puntos de [%.10g , %.10g] resulta g([a,b]) contenido en [%.8f , %.8f].\n", N + 1, a, b, gmin, gmax);
    H("   %s\n\n", (gmin >= a - 1e-9 && gmax <= b + 1e-9)
      ? "Ese rango esta dentro de [a , b], asi que la condicion SE CUMPLE."
      : "Ese rango NO esta contenido en [a , b]: la condicion no se cumple en todo el intervalo\n   (se puede achicar el intervalo alrededor de la raiz).");
    H("CONDICION 2: |g'(x)| < 1 en el intervalo.\n");
    H("   El maximo de |g'(x)| en [a , b] vale %.6f y se alcanza en x = %.6f.\n", gpmax, xgpmax);
    H("   %s\n\n", gpmax < 1
      ? "Como ese maximo es menor que 1, la condicion SE CUMPLE en todo el intervalo."
      : "Como ese maximo es mayor o igual que 1, la condicion NO se cumple en todo el intervalo.");
    if (diverge) {
        H("Iterando x_(k+1) = g(x_k) desde el punto medio del intervalo, los valores se van a\n");
        H("infinito o se salen del dominio de g.\n\n");
        H("CONCLUSION: con este despeje el metodo NO se puede aplicar (diverge). Hay que elegir otro\n"
          "g(x); por ejemplo g(x) = x - f(x)/c con c proximo a f'(raiz), que hace |g'| cercano a cero.\n");
    } else if (!dentro) {
        H("Iterando x_(k+1) = g(x_k) desde el punto medio del intervalo, la sucesion SE ESCAPA de\n");
        H("[%.10g , %.10g] y termina en x = %.10f, que esta FUERA del intervalo pedido.\n\n", a, b, x);
        H("CONCLUSION: este despeje NO sirve para la raiz de [%.10g , %.10g] (converge a otro punto\n"
          "fijo). Hay que elegir otro g(x); por ejemplo g(x) = x - f(x)/c con c proximo a f'(raiz).\n", a, b);
    } else if (fabs(gp_raiz) < 1) {
        H("En el punto fijo (x = %.10f) resulta g'(x) = %.6f, o sea |g'| = %.6f.\n\n", x, gp_raiz, fabs(gp_raiz));
        H("CONCLUSION: SI se puede aplicar punto fijo con este despeje. La iteracion x_(k+1) = g(x_k)\n"
          "converge al punto fijo x = %.10f, con convergencia lineal de razon aproximada %.4f\n"
          "(el error se multiplica por ese factor en cada iteracion).\n", x, fabs(gp_raiz));
        if (gpmax >= 1)
            H("Observacion: la condicion |g'| < 1 no se cumple en TODO el intervalo (el maximo es %.4f),\n"
              "asi que la convergencia esta garantizada solo si se arranca suficientemente cerca de la raiz.\n", gpmax);
    } else {
        H("En el punto fijo (x = %.10f) resulta g'(x) = %.6f, o sea |g'| = %.6f.\n\n", x, gp_raiz, fabs(gp_raiz));
        H("CONCLUSION: con este despeje el metodo NO converge, porque |g'| = %.4f es mayor o igual\n"
          "que 1 cerca de la raiz. Hay que elegir otro despeje; por ejemplo g(x) = x - f(x)/c con c\n"
          "proximo a f'(raiz), que hace |g'| cercano a cero.\n", fabs(gp_raiz));
    }
    hoja_mostrar();
}

static void just_newton(void) {
    double a = leer_num("a  (extremo izquierdo del intervalo) = ");
    double b = leer_num("b  (extremo derecho del intervalo) = ");
    if (a > b) { double t = a; a = b; b = t; }
    double x0 = leer_num("x0  (valor inicial que vas a usar) = ");
    int N = 1000, sfpp = 0, fpp_cambia = 0;
    double minfp = 1e300, xminfp = a;
    for (int i = 0; i <= N; i++) {
        double x = a + (b - a) * i / N, d = fabs(dnum(F, x)), dd = d2num(F, x);
        if (d == d && d < minfp) { minfp = d; xminfp = x; }
        int s = (dd > 1e-6) - (dd < -1e-6);
        if (!sfpp) sfpp = s;
        else if (s && s != sfpp) fpp_cambia = 1;
    }
    double f0 = evalf(F, x0), fp0 = dnum(F, x0), fpp0 = d2num(F, x0);
    hoja_reset();
    H("JUSTIFICACION DE LA APLICABILIDAD DEL METODO DE NEWTON-RAPHSON\n\n");
    H("f(x) = %s        intervalo [%.10g , %.10g]        x0 = %.10g\n\n", F, a, b, x0);
    H("1) La formula x_(k+1) = x_k - f(x_k)/f'(x_k) exige que f sea derivable y que f'(x) no se\n");
    H("   anule. El minimo de |f'(x)| en el intervalo vale %.6f (en x = %.6f): %s\n\n",
      minfp, xminfp, minfp > 1e-3 ? "no se anula, la condicion se cumple." : "es casi cero, hay riesgo de que el metodo salte lejos de la raiz.");
    H("2) En el intervalo, f''(x) %s de signo.\n", fpp_cambia ? "CAMBIA" : "NO cambia");
    H("3) En el valor inicial:  f(x0) = %.10f ,  f'(x0) = %.10f ,  f''(x0) = %.6f\n", f0, fp0, fpp0);
    H("   El criterio de Fourier pide f(x0)*f''(x0) > 0 : da %.6e, o sea que %s\n\n",
      f0 * fpp0, f0 * fpp0 > 0 ? "SE CUMPLE (la convergencia esta asegurada desde ese x0)." : "NO se cumple (igual suele converger, pero no esta garantizado).");
    H("CONCLUSION: %s\n",
      (minfp > 1e-3 && !fpp_cambia)
      ? "se puede aplicar Newton-Raphson en este intervalo, con convergencia cuadratica\n(la cantidad de cifras correctas aproximadamente se duplica en cada iteracion)."
      : "se puede aplicar Newton, pero conviene controlar la convergencia iteracion a iteracion\n(o arrancar con biseccion para acercarse a la raiz y refinar despues con Newton).");
    hoja_que_escribir("Este texto completo: f'(x) no se anula, signo de f'' y el criterio de Fourier en x0", (const char *)0);
    hoja_mostrar();
}


/* Propone un g(x), verifica las condiciones y deja el inciso completo escrito */
static void just_punto_fijo_auto(void) {
    double a = leer_num("a  (extremo izquierdo del intervalo donde esta la raiz) = ");
    double b = leer_num("b  (extremo derecho) = ");
    if (a > b) { double t = a; a = b; b = t; }
    double fa = evalf(F, a), fb = evalf(F, b);
    if (fa * fb > 0) {
        P("En [%g , %g] no hay cambio de signo: primero localiza la raiz con la opcion 7 del menu.\n", a, b);
        return;
    }
    /* raiz aproximada por biseccion, para evaluar g' cerca de ella */
    double lo = a, hi = b, flo = fa;
    for (int i = 0; i < 200; i++) {
        double m = 0.5 * (lo + hi), fm = evalf(F, m);
        if (flo * fm <= 0) hi = m; else { lo = m; flo = fm; }
    }
    double raiz = 0.5 * (lo + hi);
    double fpr = dnum(F, raiz);
    double c = fpr;
    if (fabs(c) < 1e-8) c = 1.0;

    /* candidatos de despeje */
    char g[3][700];
    double gpr[3], gmin[3], gmax[3];
    int sirve[3];
    sprintf(g[0], "x - (%s)/(%.6g)", F, c);
    sprintf(g[1], "x - (%s)", F);
    sprintf(g[2], "x + (%s)", F);
    for (int t = 0; t < 3; t++) {
        gpr[t] = dnum(g[t], raiz);
        gmin[t] = 1e300; gmax[t] = -1e300;
        for (int i = 0; i <= 500; i++) {
            double x = a + (b - a) * i / 500.0, v = evalf(g[t], x);
            if (v == v && fabs(v) < 1e300) { if (v < gmin[t]) gmin[t] = v; if (v > gmax[t]) gmax[t] = v; }
        }
        sirve[t] = (fabs(gpr[t]) < 1);
    }
    int elegido = 0;
    for (int t = 0; t < 3; t++) if (sirve[t]) { elegido = t; break; }

    hoja_reset();
    H("METODO DE PUNTO FIJO: ES APLICABLE?      f(x) = %s\n\n", F);
    H("El metodo consiste en reescribir f(x) = 0 en la forma x = g(x) y luego iterar\n");
    H("x_(k+1) = g(x_k). El despeje NO es unico, y de el depende que el metodo converja.\n\n");
    H("Un despeje valido para cualquier funcion es:\n");
    H("     f(x) = 0   <=>   x = x - f(x)/c        (con c distinto de cero)\n");
    H("porque restarle a x un multiplo de f(x) no cambia el conjunto de soluciones.\n");
    H("Conviene tomar c proximo a f'(raiz), porque asi g'(raiz) queda cerca de cero.\n\n");
    H("Aca la raiz esta cerca de x = %.10f y f'(x) vale alli aproximadamente %.6f,\n", raiz, fpr);
    H("de modo que se propone:\n\n     g(x) = %s\n\n", g[elegido]);
    H("VERIFICACION DE LAS DOS CONDICIONES DEL TEOREMA DEL PUNTO FIJO en [%.10g , %.10g]:\n\n", a, b);
    H("1) g debe llevar el intervalo dentro de si mismo:\n");
    H("   evaluando g en 501 puntos de [%.10g , %.10g] resulta g([a,b]) dentro de [%.8f , %.8f]\n", a, b, gmin[elegido], gmax[elegido]);
    H("   %s\n\n", (gmin[elegido] >= a - 1e-9 && gmax[elegido] <= b + 1e-9)
      ? "que esta contenido en [a , b]: LA CONDICION SE CUMPLE."
      : "que NO esta del todo contenido en [a , b]: alcanza con achicar el intervalo alrededor de la raiz.");
    H("2) |g'(x)| < 1 cerca de la raiz:\n");
    H("   g'(%.10f) = %.6f   =>   |g'| = %.6f %s 1\n\n", raiz, gpr[elegido], fabs(gpr[elegido]), fabs(gpr[elegido]) < 1 ? "<" : ">=");
    if (fabs(gpr[elegido]) < 1) {
        H("CONCLUSION: SI se puede aplicar el metodo de punto fijo con ese despeje, y converge\n");
        H("a la raiz para cualquier valor inicial tomado en el intervalo.\n\n");
    } else {
        H("CONCLUSION: con los despejes probados no se cumple |g'| < 1. Hay que proponer otro\n");
        H("despeje (o usar otro metodo).\n\n");
    }
    /* iteraciones con el g elegido */
    double x0 = 0.5 * (a + b), x = x0, xv;
    H("Iteraciones partiendo de x0 = %.10g:\n", x0);
    H("   k |      x_viejo      |    x_nuevo = g(x)   |     |dx|      |   eps %%\n");
    for (int k = 1; k <= 25; k++) {
        xv = x;
        x = evalf(g[elegido], xv);
        if (!(x == x) || !(fabs(x) < 1e12)) { H("   la sucesion se escapa: este despeje no sirve.\n"); break; }
        H("%4d | %17.10f | %19.10f | %13.4e | %10.6f\n", k, xv, x, fabs(x - xv), eps_iua(x, xv));
        if (fabs(x - xv) < 1e-10) break;
    }
    H("\nRaiz obtenida: x = %.10f     (control: f(x) = %.3e)\n", x, evalf(F, x));
    H("\nOtros despejes probados (para mostrar que la eleccion importa):\n");
    for (int t = 0; t < 3; t++)
        H("   g(x) = %-42s  |g'(raiz)| = %8.4f  ->  %s\n", g[t], fabs(gpr[t]), sirve[t] ? "converge" : "NO converge");
    P("\nListo: el texto completo del inciso quedo abajo.\n");
    hoja_que_escribir("El despeje propuesto g(x) y por que es valido (restar un multiplo de f no cambia la solucion)", "Las dos condiciones con sus numeros: g lleva el intervalo en si mismo y |g'| < 1", "La conclusion: SI / NO se puede aplicar", "La tablita de otros despejes: muestra que la respuesta depende del despeje elegido", (const char *)0);
    hoja_mostrar();
}

static void justificaciones(void) {
    P("\n=== JUSTIFICACIONES PARA LA HOJA ===\n");
    P("  1) Existencia (y unicidad) de raiz en un intervalo -> teorema de Bolzano\n");
    P("  2) Aplicabilidad del metodo de PUNTO FIJO con un g(x)\n");
    P("  3) Aplicabilidad del metodo de NEWTON-RAPHSON\n");
    P("  4) PUNTO FIJO COMPLETO: el programa propone el g(x), lo justifica e itera\n");
    int op = leer_int("Opcion: ");
    if (op == 1) just_bolzano();
    else if (op == 2) just_punto_fijo();
    else if (op == 3) just_newton();
    else if (op == 4) just_punto_fijo_auto();
    else P("Opcion invalida.\n");
}

int main(void) {
    abrir_log("salida_raices.txt");
    P("\n############  RAICES DE ECUACIONES  f(x) = 0   (Guias 1 y 2)  ############\n");
    P("Escribi la funcion como en la calculadora:  (x+1)/(x+4) - 0.25*x     ln(x^2+1) - sin(x)\n");
    P("Funciones: sin cos tan exp ln log(=ln) log10 sqrt abs  |  pi  e  |  potencia ^  |  ANGULOS EN RADIANES\n");
    P("Cada metodo imprime al final el TEXTO PARA COPIAR EN LA HOJA.\n");
    leer_funcion("\nf(x) =  (la funcion del enunciado, igualada a cero): ", F);
    for (;;) {
        P("\n================ MENU  (f(x) = %s) ================\n", F);
        P("  OPCION                          USALA SI EL ENUNCIADO DICE...\n");
        P("  1) Biseccion                    'biseccion', 'intervalo inicial [a,b]', 'metodo cerrado'\n");
        P("  2) Regula Falsi                 'regula falsi', 'falsa posicion'\n");
        P("  3) Punto fijo (iterar g)        'punto fijo' y te dan o elegis un g(x)\n");
        P("  4) Newton-Raphson               'Newton', 'tangente', 'valor inicial x0' (ENTER = deriva solo)\n");
        P("  5) Newton con derivada aprox.   'derivada aproximada', 'diferencias finitas', 'h = ...'\n");
        P("  6) Secante                      'secante', 'sin usar la derivada', 'x0 y x1'\n");
        P("  7) Tabla de f(x)                'localice las raices', 'cambios de signo', 'grafique'\n");
        P("  8) Evaluar f en un punto        calcular f(1.6), f(2) o un valor exacto de la funcion\n");
        P(" 10) JUSTIFICACIONES              'justifique': 1 Bolzano | 2 y 4 punto fijo | 3 Newton\n");
        P("  9) Cambiar f(x)                 0) Salir\n");
        int op = leer_int("Opcion: ");
        if (op == 0) break;
        switch (op) {
            case 1: cerrados(0); break;
            case 2: cerrados(1); break;
            case 3: punto_fijo(); break;
            case 4: newton(0); break;
            case 5: newton(1); break;
            case 6: secante(); break;
            case 7: escanear(); break;
            case 8: evaluar(); break;
            case 9: leer_funcion("f(x) =  (la nueva funcion): ", F); break;
            case 10: justificaciones(); break;
            default: P("Opcion invalida.\n");
        }
        if ((op >= 1 && op <= 8) || op == 10) pausa();
    }
    if (LOG_MN) fclose(LOG_MN);
    return 0;
}
