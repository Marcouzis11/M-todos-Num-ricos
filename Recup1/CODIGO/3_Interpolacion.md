# Interpolación

## Método de Lagrange

### Diferencia con ajuste de curvas (recordatorio)
En interpolación, el polinomio resultante **pasa exactamente** por todos los puntos dados $(x_0,y_0), (x_1,y_1), \dots, (x_n,y_n)$.

### Planteo general

Con $n+1$ puntos, el polinomio interpolante de Lagrange tiene grado $\leq n$ y se construye como:

$$P(x) = \sum_{i=0}^{n} y_i \cdot L_i(x)$$

donde cada $L_i(x)$ es el **polinomio base de Lagrange**, definido como:

$$L_i(x) = \prod_{\substack{j=0 \\ j\neq i}}^{n} \frac{x - x_j}{x_i - x_j}$$

### Propiedad clave de $L_i(x)$ (por qué funciona)

$$L_i(x_k) = \begin{cases} 1 & \text{si } k = i \\ 0 & \text{si } k \neq i \end{cases}$$

Esto garantiza que al evaluar $P(x_k)$, todos los términos se anulan salvo el que corresponde a $i=k$, y queda $P(x_k) = y_k$ — es decir, el polinomio efectivamente pasa por cada punto dado.

### Desarrollo paso a paso con 3 puntos (caso típico de examen)

Dados $(x_0,y_0)$, $(x_1,y_1)$, $(x_2,y_2)$:

**Paso 1 — Armar cada $L_i(x)$:**

$$L_0(x) = \frac{(x-x_1)(x-x_2)}{(x_0-x_1)(x_0-x_2)}$$

$$L_1(x) = \frac{(x-x_0)(x-x_2)}{(x_1-x_0)(x_1-x_2)}$$

$$L_2(x) = \frac{(x-x_0)(x-x_1)}{(x_2-x_0)(x_2-x_1)}$$

**Paso 2 — Armar el polinomio:**

$$P(x) = y_0\,L_0(x) + y_1\,L_1(x) + y_2\,L_2(x)$$

**Paso 3 — Si te piden evaluar en un punto $\bar x$:**

Simplemente reemplazá $x=\bar x$ en cada $L_i(x)$ (con números, no hace falta expandir el polinomio simbólicamente), y sumá:

$$P(\bar x) = y_0\,L_0(\bar x) + y_1\,L_1(\bar x) + y_2\,L_2(\bar x)$$

**Paso 4 — Si te piden el polinomio expandido (forma $a_0+a_1x+a_2x^2$):**

Ahí sí hay que expandir cada $L_i(x)$ algebraicamente (multiplicar los binomios), agrupar términos por potencia de $x$, y sumar los tres. Es más largo pero es pura álgebra de polinomios.

### Tabla de trabajo sugerida

| $i$ | $x_i$ | $y_i$ | $L_i(\bar x)$ (numerador) | $L_i(\bar x)$ (denominador) | $L_i(\bar x)$ | $y_i \cdot L_i(\bar x)$ |
|---|---|---|---|---|---|---|

Sumando la última columna se obtiene $P(\bar x)$.

### Errores comunes a evitar (para el examen)
- No usar el mismo índice $i$ en el numerador que en el denominador (en $L_i$, el factor $(x-x_i)$ **nunca** aparece).
- Con $n+1$ puntos el grado del polinomio es **como máximo** $n$ (puede ser menor si los datos son colineales, cuadráticos exactos, etc.).
- Si agregás un punto nuevo, hay que rehacer todos los $L_i(x)$ desde cero (a diferencia de Newton con diferencias divididas, que reutiliza cálculos — por si el enunciado pide comparar métodos).
