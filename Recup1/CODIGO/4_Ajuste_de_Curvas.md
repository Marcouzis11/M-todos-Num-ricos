# Ajuste de Curvas

Recordatorio: acá la curva **no pasa exactamente** por los puntos, se busca la que minimiza el error total (mínimos cuadrados), dado un conjunto de $n$ puntos $(x_i, y_i)$, $i=1,\dots,n$.

---

## 1. Regresión Lineal

### Modelo propuesto

$$y = a_0 + a_1 x$$

### Planteo de mínimos cuadrados

Se busca minimizar la suma de los cuadrados de los residuos (diferencia entre el valor real $y_i$ y el valor predicho por la recta):

$$S(a_0,a_1) = \sum_{i=1}^{n} \left(y_i - a_0 - a_1 x_i\right)^2$$

### Deducción de las ecuaciones normales

Para minimizar $S$, se deriva parcialmente respecto de cada parámetro y se iguala a cero:

**Derivada respecto de $a_0$:**

$$\frac{\partial S}{\partial a_0} = -2\sum_{i=1}^n \left(y_i - a_0 - a_1 x_i\right) = 0$$

$$\Rightarrow \sum y_i - n\,a_0 - a_1 \sum x_i = 0$$

$$\Rightarrow \boxed{n\,a_0 + a_1 \sum x_i = \sum y_i} \quad \text{(Ecuación 1)}$$

**Derivada respecto de $a_1$:**

$$\frac{\partial S}{\partial a_1} = -2\sum_{i=1}^n x_i\left(y_i - a_0 - a_1 x_i\right) = 0$$

$$\Rightarrow \sum x_i y_i - a_0\sum x_i - a_1 \sum x_i^2 = 0$$

$$\Rightarrow \boxed{a_0\sum x_i + a_1\sum x_i^2 = \sum x_i y_i} \quad \text{(Ecuación 2)}$$

### Sistema de ecuaciones normales (2×2)

$$\begin{cases} n\,a_0 + a_1 \sum x_i = \sum y_i \\[4pt] a_0\sum x_i + a_1\sum x_i^2 = \sum x_i y_i \end{cases}$$

### Resolución (despeje explícito, por ejemplo por sustitución o Cramer)

De la Ecuación 1: $a_0 = \dfrac{\sum y_i - a_1\sum x_i}{n}$. Sustituyendo en la Ecuación 2 y despejando $a_1$, se llega a la fórmula cerrada:

$$a_1 = \frac{n\sum x_i y_i - \sum x_i \sum y_i}{n\sum x_i^2 - \left(\sum x_i\right)^2}$$

$$a_0 = \frac{\sum y_i - a_1\sum x_i}{n} = \bar{y} - a_1\,\bar{x}$$

donde $\bar x = \dfrac{\sum x_i}{n}$ y $\bar y = \dfrac{\sum y_i}{n}$.

### Tabla de trabajo sugerida (para hacer a mano)

| $i$ | $x_i$ | $y_i$ | $x_i^2$ | $x_i y_i$ |
|---|---|---|---|---|
| 1 | | | | |
| ... | | | | |
| **Σ** | $\sum x_i$ | $\sum y_i$ | $\sum x_i^2$ | $\sum x_i y_i$ |

Con las 4 sumatorias (y $n$) armás directamente el sistema 2×2 y lo resolvés.

### (Opcional) Bondad del ajuste
Si el enunciado pide el coeficiente de correlación $r$:

$$r = \frac{n\sum x_iy_i - \sum x_i\sum y_i}{\sqrt{\left[n\sum x_i^2-\left(\sum x_i\right)^2\right]\left[n\sum y_i^2-\left(\sum y_i\right)^2\right]}}$$

(requiere agregar una columna extra $y_i^2$ en la tabla).

---

## 2. Regresión Polinómica (caso cuadrático)

### Modelo propuesto

$$y = a_0 + a_1 x + a_2 x^2$$

### Planteo de mínimos cuadrados

$$S(a_0,a_1,a_2) = \sum_{i=1}^n \left(y_i - a_0 - a_1 x_i - a_2 x_i^2\right)^2$$

### Deducción de las ecuaciones normales

Se deriva parcialmente respecto de cada uno de los 3 parámetros e se iguala a cero.

**Respecto de $a_0$:**

$$\frac{\partial S}{\partial a_0} = -2\sum\left(y_i - a_0 - a_1x_i - a_2x_i^2\right) = 0$$

$$\Rightarrow \boxed{n\,a_0 + a_1\sum x_i + a_2\sum x_i^2 = \sum y_i}$$

**Respecto de $a_1$:**

$$\frac{\partial S}{\partial a_1} = -2\sum x_i\left(y_i - a_0 - a_1x_i - a_2x_i^2\right) = 0$$

$$\Rightarrow \boxed{a_0\sum x_i + a_1\sum x_i^2 + a_2\sum x_i^3 = \sum x_i y_i}$$

**Respecto de $a_2$:**

$$\frac{\partial S}{\partial a_2} = -2\sum x_i^2\left(y_i - a_0 - a_1x_i - a_2x_i^2\right) = 0$$

$$\Rightarrow \boxed{a_0\sum x_i^2 + a_1\sum x_i^3 + a_2\sum x_i^4 = \sum x_i^2 y_i}$$

### Sistema de ecuaciones normales (3×3)

$$\begin{cases}
n\,a_0 + a_1\sum x_i + a_2\sum x_i^2 = \sum y_i \\[4pt]
a_0\sum x_i + a_1\sum x_i^2 + a_2\sum x_i^3 = \sum x_i y_i \\[4pt]
a_0\sum x_i^2 + a_1\sum x_i^3 + a_2\sum x_i^4 = \sum x_i^2 y_i
\end{cases}$$

En forma matricial:

$$\begin{bmatrix}
n & \sum x_i & \sum x_i^2 \\
\sum x_i & \sum x_i^2 & \sum x_i^3 \\
\sum x_i^2 & \sum x_i^3 & \sum x_i^4
\end{bmatrix}
\begin{bmatrix} a_0 \\ a_1 \\ a_2 \end{bmatrix}
=
\begin{bmatrix} \sum y_i \\ \sum x_iy_i \\ \sum x_i^2y_i \end{bmatrix}$$

### Resolución
Este sistema 3×3 se resuelve con **Eliminación Gaussiana + retrosustitución** (ver el .md de Sistemas de Ecuaciones Lineales) — no tiene fórmula cerrada simple como en el caso lineal, así que en el examen conviene resolverlo por triangulación directa con los números concretos.

### Tabla de trabajo sugerida (para hacer a mano)

| $i$ | $x_i$ | $y_i$ | $x_i^2$ | $x_i^3$ | $x_i^4$ | $x_i y_i$ | $x_i^2 y_i$ |
|---|---|---|---|---|---|---|---|
| 1 | | | | | | | |
| ... | | | | | | | |
| **Σ** | | | | | | | |

Con las 7 sumatorias armás directamente la matriz 3×3 y el vector del lado derecho, y resolvés por Gauss.

### Generalización (por si el examen pide grado $m$ mayor)
El patrón se repite: la matriz de coeficientes es de $(m+1)\times(m+1)$, con elemento genérico $\sum x_i^{\,p+q}$ en la fila $p$, columna $q$ (contando desde 0), y el lado derecho tiene elemento genérico $\sum x_i^{\,p} y_i$.
