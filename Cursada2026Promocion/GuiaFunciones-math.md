# Guía de `math.h` — Cómo escribir funciones matemáticas en C/C++

Referencia para los trabajos de Métodos Numéricos. Todos los ejemplos de esta guía
fueron compilados y ejecutados con **gcc/g++ 13.3.0** en esta máquina, así que los
resultados que figuran son reales, no estimados.

---

## 1. Compilar y ejecutar

Siempre hay que enlazar la librería matemática con **`-lm`** (va **al final** del comando).

```bash
# C++ (archivos .cpp)
g++ Biseccion.cpp -o Biseccion -lm
./Biseccion

# C (archivos .c)
gcc Biseccion.c -o Biseccion -lm
./Biseccion
```

Todo en una línea (compila y, si no hay errores, ejecuta):

```bash
g++ Biseccion.cpp -o Biseccion -lm && ./Biseccion
```

Recomendado mientras desarrollás — activa los avisos del compilador:

```bash
g++ -Wall -Wextra Biseccion.cpp -o Biseccion -lm && ./Biseccion
```

> **Nota:** con `g++` el `-lm` en general no es obligatorio (la libm ya viene enlazada),
> pero con `gcc` sí lo es. Ponelo siempre y te olvidás del tema.

Para no tipear el comando cada vez, un `Makefile` mínimo en la carpeta:

```makefile
%: %.cpp
	g++ -Wall $< -o $@ -lm
```

Y después basta con `make Biseccion && ./Biseccion`.

---

## 2. Lo mínimo indispensable

```c
#include <stdio.h>
#include <math.h>

double funcion(double x) {
    return log(x) + exp(sin(x)) - x;
}
```

Todas las funciones de `math.h` **reciben y devuelven `double`**.

---

## 3. Logaritmos

| Matemática | En C | Ejemplo | Resultado |
|---|---|---|---|
| `ln(x)` (natural, base *e*) | `log(x)` | `log(M_E)` | `1.0` |
| `log₁₀(x)` | `log10(x)` | `log10(1000)` | `3.0` |
| `log₂(x)` | `log2(x)` | `log2(8)` | `3.0` |
| `logₐ(x)` (base cualquiera) | `log(x)/log(a)` | `log(81)/log(3)` | `4.0` |
| `ln(1+x)` preciso cerca de 0 | `log1p(x)` | `log1p(1e-15)` | `1e-15` |

> ⚠️ **El error más común:** en C, `log(x)` es el logaritmo **natural**, no el de base 10.
> Si en el enunciado dice `log(x)` a secas y se refiere a base 10, tenés que escribir `log10(x)`.

**Dominio:** `log(0)` da `-inf` y `log(negativo)` da `NaN`. Ver sección 9.

---

## 4. El número *e* y exponenciales

| Matemática | En C | Notas |
|---|---|---|
| *e* (la constante ≈ 2.718281828) | `M_E` o `exp(1.0)` | Ambas dan `2.7182818285` |
| `e^x` | `exp(x)` | La forma correcta y precisa |
| `e^x` | ~~`pow(M_E, x)`~~ | Funciona pero es más lento y menos preciso — **usá `exp`** |
| `2^x` | `exp2(x)` | |
| `a^x` (base cualquiera) | `pow(a, x)` | |
| `e^x - 1` preciso cerca de 0 | `expm1(x)` | |

```c
return exp(sin(x));        // e^sen(x)
return exp(-x*x);          // e^(-x²)
return exp(2*x + 1);       // e^(2x+1)
return 5 * exp(-0.3 * x);  // 5·e^(-0,3x)
```

`M_E` y `M_PI` están disponibles con solo incluir `math.h` en gcc/g++ (verificado).

---

## 5. Potencias y raíces

| Matemática | En C | Resultado |
|---|---|---|
| `x²` | `x*x` **o** `pow(x, 2)` | `x*x` es más rápido y exacto |
| `x³` | `x*x*x` o `pow(x, 3)` | |
| `x^n` (n cualquiera) | `pow(x, n)` | `pow(2,10)` → `1024.0` |
| `x^(1/2)` = √x | `sqrt(x)` | `sqrt(2)` → `1.414214` |
| `x^(1/3)` = ∛x | `cbrt(x)` | `cbrt(27)` → `3.0` |
| `x^(2/3)` | `pow(x, 2.0/3.0)` | ojo con la división entera |
| `1/x^n` | `pow(x, -n)` | |
| `√(x² + y²)` | `hypot(x, y)` | `hypot(3,4)` → `5.0` — evita overflow |

> ⚠️ **`^` NO es potencia en C.** Es el operador XOR bit a bit.
> Verificado: `2^3` da **1**, no 8. El compilador incluso avisa:
> *"result of '2^3' is 1; did you mean '1 << 3'?"*. Para potencias siempre `pow()`.

> ⚠️ **`pow` con base negativa y exponente fraccionario da `NaN`.**
> Verificado: `pow(-8, 1.0/3)` → `-nan`, pero `cbrt(-8)` → `-2.0`.
> Para raíces cúbicas de negativos usá `cbrt()`.

---

## 6. Trigonometría

Todas trabajan en **radianes**, nunca en grados.

| Matemática | En C |
|---|---|
| `sen(x)`, `cos(x)`, `tan(x)` | `sin(x)`, `cos(x)`, `tan(x)` |
| `arcsen(x)`, `arccos(x)`, `arctan(x)` | `asin(x)`, `acos(x)`, `atan(x)` |
| `arctan(y/x)` con cuadrante correcto | `atan2(y, x)` |
| `sec(x)` | `1.0/cos(x)` |
| `cosec(x)` | `1.0/sin(x)` |
| `cotg(x)` | `1.0/tan(x)` |
| `senh(x)`, `cosh(x)`, `tgh(x)` | `sinh(x)`, `cosh(x)`, `tanh(x)` |
| `arcsenh`, `arccosh`, `arctgh` | `asinh(x)`, `acosh(x)`, `atanh(x)` |

**Conversión de grados a radianes:**

```c
double rad = grados * M_PI / 180.0;
double grados = rad * 180.0 / M_PI;

return sin(x * M_PI / 180.0);   // sen(x) con x en grados
```

**Potencias de funciones trigonométricas** — `sen²(x)` se escribe así:

```c
return sin(x) * sin(x);   // correcto
return pow(sin(x), 2);    // también correcto
// return sin^2(x);       // NO EXISTE, error de compilación
// return sin(x)^2;       // NO, eso es XOR
```

---

## 7. Valor absoluto, redondeo y resto

| Matemática | En C | Notas |
|---|---|---|
| `\|x\|` con `x` real | **`fabs(x)`** | La que tenés que usar siempre |
| `\|x\|` con `x` entero | `abs(x)` | requiere `#include <stdlib.h>` |
| resto de `x/y` reales | `fmod(x, y)` | `fmod(7.5, 2)` → `1.50` |
| redondear al más cercano | `round(x)` | |
| truncar hacia abajo | `floor(x)` | |
| truncar hacia arriba | `ceil(x)` | |
| truncar hacia cero | `trunc(x)` | |

> ⚠️ **`abs()` vs `fabs()` — trampa clásica.** Verificado en esta máquina:
> - En **C** (`gcc`): `abs(-2.7)` devuelve **`2`** — trunca la parte decimal, silenciosamente.
> - En **C++** (`g++`): `abs(-2.7)` devuelve `2.70` porque hay una sobrecarga para `double`.
>
> O sea, el mismo código da resultados distintos según con qué compiles.
> **Usá siempre `fabs()` para reales** y te olvidás del problema.

Esto importa mucho en los criterios de corte:

```c
} while (fabs(error) > tolerancia);          // bien
} while (fabs(funcion(c)) > tolerancia);     // bien
} while (fabs(x1 - x0) > tolerancia);        // bien
```

---

## 8. Trampas de aritmética que dan resultados mal

### División entera

```c
1/2         // → 0    (ambos son enteros, se trunca)
1.0/2       // → 0.5  (verificado)
1/2.0       // → 0.5
(double)1/2 // → 0.5
```

Escribí siempre las constantes con punto decimal: `2.0`, `1.0/3.0`, `0.5`.

```c
return pow(x, 2/3);      // MAL: 2/3 vale 0, esto da x^0 = 1
return pow(x, 2.0/3.0);  // BIEN
```

### Precedencia de operadores

```c
-x*x          // es -(x²), NO (-x)²
a/2*b         // es (a/2)·b, NO a/(2b)
a/(2*b)       // así se escribe a/(2b)
1/2*x         // es 0 (división entera) → usar 0.5*x o x/2.0
```

Ante la duda, **poné paréntesis de más**. No cuestan nada y evitan horas de debug.

### Fracciones largas

Una expresión como esta:

$$c = \frac{a \cdot f(b) - b \cdot f(a)}{f(b) - f(a)}$$

se escribe con paréntesis explícitos en numerador y denominador:

```c
c = (a*funcion(b) - b*funcion(a)) / (funcion(b) - funcion(a));
```

---

## 9. Cuando la función se rompe: `NaN` e `inf`

Valores verificados en esta máquina:

| Expresión | Resultado |
|---|---|
| `log(0)` | `-inf` |
| `log(-1)` | `-nan` |
| `sqrt(-1)` | `-nan` |
| `pow(-8, 1.0/3)` | `-nan` |
| `1.0/0.0` | `inf` |

El problema es que `NaN` **se propaga en silencio**: si tu `funcion(x)` devuelve `NaN`,
todas las comparaciones (`>`, `<`, `==`) dan falso y el método iterativo se comporta
de forma rarísima sin avisar.

Detectarlo:

```c
#include <math.h>

if (isnan(valor)) { printf("La función no está definida en ese punto\n"); }
if (isinf(valor)) { printf("La función diverge en ese punto\n"); }
if (!isfinite(valor)) { /* cubre NaN e inf juntos */ }
```

Esto es especialmente relevante en tus programas: `funcion(x) = log(x) + e^sen(x) - x`
**no está definida para x ≤ 0**. Si el usuario ingresa `a = 0` o `a = -1`, el programa
no va a fallar visiblemente, simplemente va a dar un resultado sin sentido.

Chequeo defensivo para agregarle a Bisección o Regula Falsi:

```c
if (!isfinite(funcion(a)) || !isfinite(funcion(b))) {
    printf("\nLa funcion no esta definida en algun extremo del intervalo\n");
    return;
}
```

---

## 10. Diccionario rápido: notación matemática → C

| Se escribe así en el enunciado | Se escribe así en C |
|---|---|
| `ln(x) + e^sen(x) − x` | `log(x) + exp(sin(x)) - x` |
| `x³ − 2x − 5` | `x*x*x - 2*x - 5` |
| `e^(−x) − x` | `exp(-x) - x` |
| `x² − e^(−2x)` | `x*x - exp(-2*x)` |
| `cos(x) − x` | `cos(x) - x` |
| `x·sen(x) − 1` | `x*sin(x) - 1` |
| `√(x+1) − x²` | `sqrt(x + 1) - x*x` |
| `log₁₀(x) − 1/x` | `log10(x) - 1.0/x` |
| `2^x − 3x` | `pow(2, x) - 3*x` |
| `(x−1)/(x+2)` | `(x - 1) / (x + 2)` |
| `sen²(x) + cos(2x)` | `sin(x)*sin(x) + cos(2*x)` |
| `3x⁴ − 2x³ + x − 7` | `3*pow(x,4) - 2*pow(x,3) + x - 7` |
| `\|x − 2\| − 1` | `fabs(x - 2) - 1` |
| `x^(3/2)` | `pow(x, 1.5)` o `pow(x, 3.0/2.0)` |
| `1/(1+x²)` | `1.0 / (1.0 + x*x)` |
| `e^(x²)/x` | `exp(x*x) / x` |

**Polinomios — forma de Horner** (más eficiente y numéricamente más estable):

```c
// 3x⁴ − 2x³ + x − 7
return ((((3*x - 2)*x + 0)*x + 1)*x - 7);
```

---

## 11. Derivadas, para los métodos abiertos

Newton-Raphson necesita `f'(x)`. Dos opciones:

**a) Derivada analítica** (preferible, más exacta):

```c
double funcion(double x) {
    return log(x) + exp(sin(x)) - x;
}

double derivada(double x) {
    return 1.0/x + cos(x) * exp(sin(x)) - 1.0;
}
```

Tabla de derivadas frecuentes ya traducidas a C:

| `f(x)` | `f(x)` en C | `f'(x)` en C |
|---|---|---|
| `ln(x)` | `log(x)` | `1.0/x` |
| `e^x` | `exp(x)` | `exp(x)` |
| `e^u(x)` | `exp(u(x))` | `u'(x) * exp(u(x))` |
| `sen(x)` | `sin(x)` | `cos(x)` |
| `cos(x)` | `cos(x)` | `-sin(x)` |
| `tan(x)` | `tan(x)` | `1.0/(cos(x)*cos(x))` |
| `xⁿ` | `pow(x,n)` | `n * pow(x, n-1)` |
| `√x` | `sqrt(x)` | `1.0/(2.0*sqrt(x))` |
| `log₁₀(x)` | `log10(x)` | `1.0/(x*log(10.0))` |

**b) Derivada numérica** (si la analítica es muy engorrosa):

```c
double derivada(double x) {
    double h = 1e-7;
    return (funcion(x + h) - funcion(x - h)) / (2.0 * h);   // diferencia centrada
}
```

`h = 1e-7` es un compromiso razonable: más chico y el error de redondeo domina.

---

## 12. Plantilla lista para copiar

```c
#include <stdio.h>
#include <math.h>

double funcion(double x);

int main() {
    double x;

    printf("Ingrese x: ");
    scanf("%lf", &x);

    if (!isfinite(funcion(x))) {
        printf("La funcion no esta definida en x = %.4lf\n", x);
        return 1;
    }

    printf("f(%.6lf) = %.10lf\n", x, funcion(x));
    return 0;
}

double funcion(double x) {
    return log(x) + exp(sin(x)) - x;    // <-- cambiar acá nomás
}
```

Compilar y ejecutar:

```bash
g++ -Wall plantilla.cpp -o plantilla -lm && ./plantilla
```

---

## 13. Formato de impresión

| Especificador | Uso |
|---|---|
| `%lf` | leer un `double` con `scanf` — **obligatorio el `l`** |
| `%f` o `%lf` | imprimir un `double` con `printf` |
| `%.10lf` | 10 decimales fijos |
| `%.6e` | notación científica: `2.614639e+00` |
| `%g` | elige automáticamente entre fija y científica |
| `%d` | enteros (contador de iteraciones) |

> ⚠️ En `scanf` un `double` **siempre** lleva `%lf`. Con `%f` se lee mal y el
> programa da valores basura sin avisar.
