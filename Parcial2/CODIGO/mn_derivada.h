/* mn_derivada.h -- Derivacion SIMBOLICA de la funcion escrita por teclado.
 *
 *  derivar_texto("ln(x^2+1) - sin(x)", salida)  ->  "2*x/(x^2+1) - cos(x)"
 *
 *  Sirve para que el programa calcule f'(x) sin que haya que derivar a mano.
 *  Construye el arbol de la expresion, aplica las reglas de derivacion y
 *  simplifica el resultado.
 */
#ifndef MN_DERIVADA_H
#define MN_DERIVADA_H

#define MAXNODOS 6000

enum { T_NUM, T_VAR, T_SUM, T_RES, T_MUL, T_DIV, T_POT, T_FUN, T_NEG };

typedef struct {
    int tipo;
    double val;       /* si es numero */
    int a, b;         /* hijos (-1 si no tiene) */
    char fn[10];      /* nombre de la funcion */
} NodoMN;

static NodoMN ND[MAXNODOS];
static int NDn = 0;
static const char *dp;      /* puntero de lectura del parser simbolico */
static int derr = 0;

static int nd_nuevo(int tipo, double val, int a, int b, const char *fn) {
    if (NDn >= MAXNODOS - 1) { derr = 1; return 0; }
    int i = NDn++;
    ND[i].tipo = tipo; ND[i].val = val; ND[i].a = a; ND[i].b = b;
    ND[i].fn[0] = 0;
    if (fn) { strncpy(ND[i].fn, fn, 9); ND[i].fn[9] = 0; }
    return i;
}
static int nd_num(double v) { return nd_nuevo(T_NUM, v, -1, -1, 0); }
static int es_num(int n, double v) { return ND[n].tipo == T_NUM && fabs(ND[n].val - v) < 1e-15; }

/* ---------------- constructores con simplificacion ---------------- */
static int s_sum(int a, int b) {
    if (es_num(a, 0)) return b;
    if (es_num(b, 0)) return a;
    if (ND[a].tipo == T_NUM && ND[b].tipo == T_NUM) return nd_num(ND[a].val + ND[b].val);
    return nd_nuevo(T_SUM, 0, a, b, 0);
}
static int s_neg(int a);
static int s_res(int a, int b) {
    if (es_num(b, 0)) return a;
    if (ND[a].tipo == T_NUM && ND[b].tipo == T_NUM) return nd_num(ND[a].val - ND[b].val);
    if (es_num(a, 0)) return s_neg(b);
    return nd_nuevo(T_RES, 0, a, b, 0);
}
static int s_mul(int a, int b) {
    if (es_num(a, 0) || es_num(b, 0)) return nd_num(0);
    if (es_num(a, 1)) return b;
    if (es_num(b, 1)) return a;
    if (ND[a].tipo == T_NUM && ND[b].tipo == T_NUM) return nd_num(ND[a].val * ND[b].val);
    if (es_num(a, -1)) return s_neg(b);
    if (es_num(b, -1)) return s_neg(a);
    /* el numero adelante:  cos(3x)*3  ->  3*cos(3x) */
    if (ND[b].tipo == T_NUM && ND[a].tipo != T_NUM) { int t = a; a = b; b = t; }
    /* juntar numeros:  3*(2*x)  ->  6*x */
    if (ND[a].tipo == T_NUM && ND[b].tipo == T_MUL && ND[ND[b].a].tipo == T_NUM)
        return s_mul(nd_num(ND[a].val * ND[ND[b].a].val), ND[b].b);
    return nd_nuevo(T_MUL, 0, a, b, 0);
}
static int s_div(int a, int b) {
    if (es_num(a, 0)) return nd_num(0);
    if (es_num(b, 1)) return a;
    if (ND[a].tipo == T_NUM && ND[b].tipo == T_NUM && ND[b].val != 0) return nd_num(ND[a].val / ND[b].val);
    return nd_nuevo(T_DIV, 0, a, b, 0);
}
static int s_pot(int a, int b) {
    if (es_num(b, 1)) return a;
    if (es_num(b, 0)) return nd_num(1);
    return nd_nuevo(T_POT, 0, a, b, 0);
}
static int s_neg(int a) {
    if (ND[a].tipo == T_NUM) return nd_num(-ND[a].val);
    return nd_nuevo(T_NEG, 0, a, -1, 0);
}
static int s_fun(const char *f, int a) { return nd_nuevo(T_FUN, 0, a, -1, f); }

/* ---------------- parser que arma el arbol ---------------- */
static int d_expr(void);

static void d_sp(void) { while (*dp == ' ' || *dp == '\t') dp++; }

static int d_prim(void) {
    d_sp();
    if (*dp == '(') {
        dp++;
        int e = d_expr();
        d_sp();
        if (*dp == ')') dp++; else derr = 1;
        return e;
    }
    if (isdigit((unsigned char)*dp) || *dp == '.') {
        char *fin;
        double v = strtod(dp, &fin);
        if (fin == dp) { derr = 1; return nd_num(0); }
        dp = fin;
        return nd_num(v);
    }
    if (isalpha((unsigned char)*dp)) {
        char nom[16];
        int n = 0;
        while (isalnum((unsigned char)*dp) && n < 15) nom[n++] = (char)tolower((unsigned char)*dp++);
        nom[n] = 0;
        if (!strcmp(nom, "x")) return nd_nuevo(T_VAR, 0, -1, -1, 0);
        if (!strcmp(nom, "pi")) return nd_num(PI_MN);
        if (!strcmp(nom, "e")) return nd_num(exp(1.0));
        d_sp();
        if (*dp != '(') { derr = 1; return nd_num(0); }
        dp++;
        int arg = d_expr();
        d_sp();
        if (*dp == ')') dp++; else derr = 1;
        if (!strcmp(nom, "sen")) strcpy(nom, "sin");
        if (!strcmp(nom, "tg")) strcpy(nom, "tan");
        if (!strcmp(nom, "log")) strcpy(nom, "ln");
        if (!strcmp(nom, "raiz")) strcpy(nom, "sqrt");
        return s_fun(nom, arg);
    }
    derr = 1;
    return nd_num(0);
}

static int d_unario(void);

static int d_pot(void) {
    int base = d_prim();
    d_sp();
    if (*dp == '^') { dp++; return s_pot(base, d_unario()); }
    return base;
}

static int d_unario(void) {
    d_sp();
    if (*dp == '-') { dp++; return s_neg(d_unario()); }
    if (*dp == '+') { dp++; return d_unario(); }
    return d_pot();
}

static int d_term(void) {
    int v = d_unario();
    for (;;) {
        d_sp();
        if (*dp == '*') { dp++; v = s_mul(v, d_unario()); }
        else if (*dp == '/') { dp++; v = s_div(v, d_unario()); }
        else if (*dp == '(' || isalpha((unsigned char)*dp)) { v = s_mul(v, d_unario()); }
        else return v;
    }
}

static int d_expr(void) {
    int v = d_term();
    for (;;) {
        d_sp();
        if (*dp == '+') { dp++; v = s_sum(v, d_term()); }
        else if (*dp == '-') { dp++; v = s_res(v, d_term()); }
        else return v;
    }
}

/* ---------------- la derivada ---------------- */
static int derivar(int n) {
    int u, du;
    switch (ND[n].tipo) {
        case T_NUM: return nd_num(0);
        case T_VAR: return nd_num(1);
        case T_SUM: return s_sum(derivar(ND[n].a), derivar(ND[n].b));
        case T_RES: return s_res(derivar(ND[n].a), derivar(ND[n].b));
        case T_NEG: return s_neg(derivar(ND[n].a));
        case T_MUL:   /* (u*v)' = u'v + uv' */
            return s_sum(s_mul(derivar(ND[n].a), ND[n].b), s_mul(ND[n].a, derivar(ND[n].b)));
        case T_DIV:   /* (u/v)' = (u'v - uv')/v^2 */
            return s_div(s_res(s_mul(derivar(ND[n].a), ND[n].b), s_mul(ND[n].a, derivar(ND[n].b))),
                         s_pot(ND[n].b, nd_num(2)));
        case T_POT:
            u = ND[n].a; du = derivar(u);
            if (ND[ND[n].b].tipo == T_NUM) {       /* u^k  ->  k*u^(k-1)*u' */
                double k = ND[ND[n].b].val;
                return s_mul(s_mul(nd_num(k), s_pot(u, nd_num(k - 1))), du);
            }
            /* u^v general:  u^v * ( v'*ln(u) + v*u'/u ) */
            return s_mul(n, s_sum(s_mul(derivar(ND[n].b), s_fun("ln", u)),
                                  s_div(s_mul(ND[n].b, du), u)));
        case T_FUN: {
            u = ND[n].a; du = derivar(u);
            const char *f = ND[n].fn;
            if (!strcmp(f, "sin"))  return s_mul(s_fun("cos", u), du);
            if (!strcmp(f, "cos"))  return s_neg(s_mul(s_fun("sin", u), du));
            if (!strcmp(f, "tan"))  return s_div(du, s_pot(s_fun("cos", u), nd_num(2)));
            if (!strcmp(f, "exp"))  return s_mul(n, du);
            if (!strcmp(f, "ln"))   return s_div(du, u);
            if (!strcmp(f, "log10")) return s_div(du, s_mul(u, nd_num(log(10.0))));
            if (!strcmp(f, "sqrt")) return s_div(du, s_mul(nd_num(2), n));
            if (!strcmp(f, "asin")) return s_div(du, s_fun("sqrt", s_res(nd_num(1), s_pot(u, nd_num(2)))));
            if (!strcmp(f, "acos")) return s_neg(s_div(du, s_fun("sqrt", s_res(nd_num(1), s_pot(u, nd_num(2))))));
            if (!strcmp(f, "atan")) return s_div(du, s_sum(nd_num(1), s_pot(u, nd_num(2))));
            if (!strcmp(f, "sinh")) return s_mul(s_fun("cosh", u), du);
            if (!strcmp(f, "cosh")) return s_mul(s_fun("sinh", u), du);
            if (!strcmp(f, "tanh")) return s_div(du, s_pot(s_fun("cosh", u), nd_num(2)));
            derr = 1;                      /* abs u otra: no derivable simbolicamente */
            return nd_num(0);
        }
    }
    derr = 1;
    return nd_num(0);
}

/* ---------------- pasar el arbol a texto ---------------- */
static void nd_texto(int n, char *out, int *o, int max);

static void nd_hijo(int n, char *out, int *o, int max, int parentesis) {
    if (parentesis) { if (*o < max - 1) out[(*o)++] = '('; }
    nd_texto(n, out, o, max);
    if (parentesis) { if (*o < max - 1) out[(*o)++] = ')'; }
}

static void nd_texto(int n, char *out, int *o, int max) {
    char tmp[64];
    int t = ND[n].tipo;
    if (t == T_NUM) {
        sprintf(tmp, "%.10g", ND[n].val);
        for (int i = 0; tmp[i] && *o < max - 1; i++) out[(*o)++] = tmp[i];
        return;
    }
    if (t == T_VAR) { if (*o < max - 1) out[(*o)++] = 'x'; return; }
    if (t == T_FUN) {
        for (int i = 0; ND[n].fn[i] && *o < max - 1; i++) out[(*o)++] = ND[n].fn[i];
        nd_hijo(ND[n].a, out, o, max, 1);
        return;
    }
    if (t == T_NEG) {
        if (*o < max - 1) out[(*o)++] = '-';
        nd_hijo(ND[n].a, out, o, max, ND[ND[n].a].tipo == T_SUM || ND[ND[n].a].tipo == T_RES);
        return;
    }
    int ta = ND[ND[n].a].tipo, tb = ND[n].b >= 0 ? ND[ND[n].b].tipo : T_NUM;
    int sumres_a = (ta == T_SUM || ta == T_RES), sumres_b = (tb == T_SUM || tb == T_RES);
    int prod_b = (tb == T_MUL || tb == T_DIV);
    if (t == T_SUM || t == T_RES) {
        nd_hijo(ND[n].a, out, o, max, 0);
        if (*o < max - 2) { out[(*o)++] = ' '; out[(*o)++] = (t == T_SUM ? '+' : '-'); out[(*o)++] = ' '; }
        nd_hijo(ND[n].b, out, o, max, sumres_b);
        return;
    }
    if (t == T_MUL) {
        nd_hijo(ND[n].a, out, o, max, sumres_a);
        if (*o < max - 1) out[(*o)++] = '*';
        nd_hijo(ND[n].b, out, o, max, sumres_b);
        return;
    }
    if (t == T_DIV) {
        nd_hijo(ND[n].a, out, o, max, sumres_a);
        if (*o < max - 1) out[(*o)++] = '/';
        nd_hijo(ND[n].b, out, o, max, sumres_b || prod_b);
        return;
    }
    if (t == T_POT) {
        nd_hijo(ND[n].a, out, o, max, !(ta == T_NUM || ta == T_VAR || ta == T_FUN));
        if (*o < max - 1) out[(*o)++] = '^';
        nd_hijo(ND[n].b, out, o, max, !(tb == T_NUM || tb == T_VAR || tb == T_FUN));
        return;
    }
}

/* Devuelve 1 si pudo derivar; deja el resultado en 'salida'. */
static int derivar_texto(const char *expr, char *salida) {
    NDn = 0; derr = 0;
    dp = expr;
    int raiz = d_expr();
    d_sp();
    if (derr || *dp) return 0;
    int d = derivar(raiz);
    if (derr) return 0;
    int o = 0;
    salida[0] = 0;
    nd_texto(d, salida, &o, 500);
    salida[o] = 0;
    return salida[0] != 0;
}

#endif
