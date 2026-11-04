/* Euler explicito para sistemas de dos o tres EDO de primer orden. */
#ifndef MN_EULER_SISTEMAS_H
#define MN_EULER_SISTEMAS_H

#include "mn_comun.h"

#define MAXPASOS_SISTEMA 20000
static const char *NOMBRE_SISTEMA[3] = {"y", "z", "w"};

static void sistema_variables(int n, double x, const double *u) {
    vars_reset();
    var_set("t", x);
    for (int j = 0; j < n; j++) {
        char alias[8];
        sprintf(alias, "y%d", j + 1);
        var_set(alias, u[j]);
        if (j > 0) var_set(NOMBRE_SISTEMA[j], u[j]);
    }
}

/* Todas las pendientes se evaluan con el MISMO vector anterior. */
static int sistema_paso_euler(int n, const char funciones[][256], double x,
                              const double *u, double h, double *nuevo,
                              double *pendientes) {
    sistema_variables(n, x, u);
    for (int j = 0; j < n; j++) {
        pendientes[j] = evalf2(funciones[j], x, u[0]);
        if (eerr || !isfinite(pendientes[j])) return 0;
    }
    for (int j = 0; j < n; j++) {
        nuevo[j] = u[j] + h * pendientes[j];
        if (!isfinite(nuevo[j])) return 0;
    }
    return 1;
}

static double sistema_leer_finito(const char *msg) {
    for (;;) {
        double v = leer_num(msg);
        if (isfinite(v)) return v;
        P("El valor debe ser un numero finito.\n");
    }
}

static void sistema_fila(int i, double x, const double *u, int n) {
    P("%5d | %14.8f", i, x);
    for (int j = 0; j < n; j++) P(" | %16.10f", u[j]);
    P("\n");
}

static void sistema_resolver_euler(int n) {
    char funciones[3][256], msg[160];
    double u[3] = {1, 1, 1};
    P("\n=== EULER PARA UN SISTEMA DE %d ECUACIONES ===\n", n);
    P("Variable independiente: x (tambien podes escribir t).\n");
    P("Incognitas: y, z%s. Tambien podes usar y1, y2%s.\n",
      n == 3 ? ", w" : "", n == 3 ? ", y3" : "");
    P("Escribi cada derivada despejada; por ejemplo dy/dx = z.\n");
    sistema_variables(n, 1, u);
    for (int j = 0; j < n; j++) {
        sprintf(msg, "d%s/dx = ", NOMBRE_SISTEMA[j]);
        leer_funcion(msg, funciones[j]);
    }
    double x0 = sistema_leer_finito("x0 = ");
    for (int j = 0; j < n; j++) {
        sprintf(msg, "%s(x0) = ", NOMBRE_SISTEMA[j]);
        u[j] = sistema_leer_finito(msg);
    }
    double h;
    do {
        h = sistema_leer_finito("h (paso, distinto de cero) = ");
        if (h == 0) P("h no puede ser cero.\n");
    } while (h == 0);
    double xf = sistema_leer_finito("x final = ");
    double distancia = xf - x0;
    if (!isfinite(distancia) ||
        (distancia != 0 && ((distancia > 0) != (h > 0)))) {
        P("El signo de h debe avanzar desde x0 hacia x final. Revisa los datos.\n");
        return;
    }
    double cantidad = distancia / h;
    if (!isfinite(cantidad) || cantidad > MAXPASOS_SISTEMA) {
        P("Se necesitan mas de %d pasos. Usa un paso mayor.\n", MAXPASOS_SISTEMA);
        return;
    }
    int pasos = (int)ceil(cantidad - 1e-12 * fmax(1.0, cantidad));
    if (distancia != 0 && pasos < 1) pasos = 1;

    int cada = 1;
    if (pasos > 25) {
        do {
            cada = leer_int("Mostrar una fila cada cuantos pasos (1 = todas): ");
        } while (cada < 1);
    }
    P("\nDESARROLLO PARA COPIAR EN LA HOJA\n");
    P("EULER EXPLICITO: SISTEMA DE %d EDO DE PRIMER ORDEN\n", n);
    for (int j = 0; j < n; j++) {
        P("   d%s/dx = %s;  %s(%.10g) = %.10g\n",
          NOMBRE_SISTEMA[j], funciones[j], NOMBRE_SISTEMA[j], x0, u[j]);
    }
    P("Intervalo: [%.10g, %.10g]; h = %.10g; %d pasos. Orden global: 1.\n", x0, xf, h, pasos);
    P("Si h no divide el intervalo, el ultimo paso se acorta para llegar a x final.\n");
    for (int j = 0; j < n; j++) {
        P("   %s_(i+1) = %s_i + h_i*f%d(x_i, y_i, z_i%s)\n",
          NOMBRE_SISTEMA[j], NOMBRE_SISTEMA[j], j + 1, n == 3 ? ", w_i" : "");
    }
    P("Se calculan TODAS las pendientes con los valores del paso anterior.\n");
    P("\nTABLA DE RESULTADOS:\n    i |            x_i");
    for (int j = 0; j < n; j++) P(" | %14s_i", NOMBRE_SISTEMA[j]);
    P("\n");
    double x = x0;
    sistema_fila(0, x, u, n);
    for (int i = 1; i <= pasos; i++) {
        double destino = i == pasos ? xf : x0 + i * h;
        double hi = destino - x, nuevo[3], pendientes[3];
        if (!sistema_paso_euler(n, funciones, x, u, hi, nuevo, pendientes)) {
            P("\nNo se pudo completar el paso %d en x = %.10g: una derivada\n", i, x);
            P("o un resultado no es finito. Revisa el dominio y el paso h.\n");
            return;
        }
        if (i == 1) {
            P("\nPRIMERA ITERACION PASO A PASO (h_0 = %.10g):\n", hi);
            for (int j = 0; j < n; j++) {
                P("   f%d(x0, y0, z0%s) = %.10f\n", j + 1, n == 3 ? ", w0" : "", pendientes[j]);
                P("   %s1 = %.10g + (%.10g)*(%.10f) = %.10f\n",
                  NOMBRE_SISTEMA[j], u[j], hi, pendientes[j], nuevo[j]);
            }
            P("\n");
        }
        for (int j = 0; j < n; j++) u[j] = nuevo[j];
        x = destino;
        if (i % cada == 0 || i == pasos) sistema_fila(i, x, u, n);
    }
    P("\nRESULTADO:\n");
    for (int j = 0; j < n; j++) P("   %s(%.10g) ~ %.10f\n", NOMBRE_SISTEMA[j], x, u[j]);
    P("\nError global O(h), para soluciones suficientemente regulares.\n");
}

/* Conserva las variables del evaluador del programa que abre esta opcion. */
static void op_sistema_euler(int n) {
    char nombres[MAXVARS][16];
    double valores[MAXVARS];
    int cantidad = NVARS;
    memcpy(nombres, VNOM, sizeof(nombres));
    memcpy(valores, VVAL, sizeof(valores));
    sistema_resolver_euler(n);
    memcpy(VNOM, nombres, sizeof(nombres));
    memcpy(VVAL, valores, sizeof(valores));
    NVARS = cantidad;
}

static int programa_sistema_euler(int n) {
    abrir_log(n == 2 ? "salida_edo_sistema2.txt" : "salida_edo_sistema3.txt");
    P("\n#### EULER: SISTEMAS DE %d ECUACIONES DIFERENCIALES ####\n", n);
    for (;;) {
        op_sistema_euler(n);
        if (leer_int("\n1) Resolver otro sistema   0) Salir: ") != 1) break;
    }
    if (LOG_MN) fclose(LOG_MN);
    return 0;
}

#endif
