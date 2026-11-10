# Sistemas de Ecuaciones Lineales

Sistema general: $Ax = b$, con $A$ de $n\times n$, $x$ y $b$ vectores columna de $n$ elementos.

---

## 1. Eliminación Gaussiana (Triangulación + Determinante + Retrosustitución)

### Paso 1 — Armar la matriz aumentada

$$[A \mid b] = \left[\begin{array}{ccc|c}
a_{11} & a_{12} & a_{13} & b_1 \\
a_{21} & a_{22} & a_{23} & b_2 \\
a_{31} & a_{32} & a_{33} & b_3
\end{array}\right]$$

### Paso 2 — Triangulación (eliminación hacia adelante)

Para cada columna pivote $k = 1, \dots, n-1$, y para cada fila $i > k$, se calcula el **factor multiplicador**:

$$m_{ik} = \frac{a_{ik}}{a_{kk}}$$

y se actualiza toda la fila $i$:

$$\text{Fila}_i \;\leftarrow\; \text{Fila}_i - m_{ik}\cdot \text{Fila}_k$$

(esto se aplica tanto a los coeficientes de $A$ como al término independiente $b_i$).

**Ejemplo con matriz $3\times 3$ (para practicar el orden de los pasos):**

- Paso $k=1$: eliminar $a_{21}$ y $a_{31}$ usando la fila 1 como pivote.
  - $m_{21} = a_{21}/a_{11}$, $m_{31}=a_{31}/a_{11}$
- Paso $k=2$: eliminar $a_{32}$ (nuevo valor, tras el paso anterior) usando la fila 2 como pivote.
  - $m_{32} = a_{32}^{(1)}/a_{22}^{(1)}$

Al terminar, la matriz queda triangular superior:

$$U = \left[\begin{array}{ccc}
a_{11} & a_{12} & a_{13} \\
0 & a_{22}^{(1)} & a_{23}^{(1)} \\
0 & 0 & a_{33}^{(2)}
\end{array}\right]$$

⚠️ **Pivoteo:** si en algún paso $a_{kk} = 0$ (o muy chico), hay que intercambiar filas antes de continuar (pivoteo parcial). Cada intercambio de filas cambia el signo del determinante.

### Paso 3 — Cálculo del determinante

Una vez triangulada la matriz, el determinante es el producto de los elementos de la diagonal:

$$\det(A) = (-1)^{p}\cdot a_{11}\cdot a_{22}^{(1)} \cdot a_{33}^{(2)} \cdots a_{nn}^{(n-1)}$$

donde $p$ es la cantidad de intercambios de filas realizados (si no hubo pivoteo, $p=0$ y el signo es $+$).

### Paso 4 — Retrosustitución

Con el sistema ya triangular superior $Ux = b'$, se resuelve de abajo hacia arriba:

$$x_n = \frac{b'_n}{a_{nn}}$$

Y para $i = n-1, n-2, \dots, 1$:

$$x_i = \frac{b'_i - \displaystyle\sum_{j=i+1}^{n} a_{ij}\,x_j}{a_{ii}}$$

**Ejemplo explícito para $n=3$:**

$$x_3 = \frac{b_3'}{a_{33}}, \qquad x_2 = \frac{b_2' - a_{23}x_3}{a_{22}}, \qquad x_1 = \frac{b_1' - a_{12}x_2 - a_{13}x_3}{a_{11}}$$

---

## 2. Método de Jacobi (Iterativo)

### Condición de convergencia (chequear siempre primero)

El método converge si $A$ es **diagonalmente dominante**:

$$|a_{ii}| > \sum_{j\neq i} |a_{ij}| \qquad \text{para todo } i$$

(Si no se cumple, a veces se puede reordenar filas/columnas para lograrlo.)

### Despeje de la fórmula

De la ecuación $i$ del sistema:

$$a_{i1}x_1 + a_{i2}x_2 + \dots + a_{ii}x_i + \dots + a_{in}x_n = b_i$$

se despeja $x_i$:

$$x_i = \frac{b_i - \displaystyle\sum_{j\neq i} a_{ij}x_j}{a_{ii}}$$

### Iteración de Jacobi

$$x_i^{(k+1)} = \frac{b_i - \displaystyle\sum_{j\neq i} a_{ij}\,x_j^{(k)}}{a_{ii}} \qquad i=1,\dots,n$$

**Clave del método:** para calcular **todos** los $x_i^{(k+1)}$ de la iteración $k+1$, se usan **únicamente los valores de la iteración anterior** $x_j^{(k)}$ (aunque ya tengas calculado, por ejemplo, $x_1^{(k+1)}$, no lo usás todavía para calcular $x_2^{(k+1)}$).

### Vector inicial
Se parte de una aproximación inicial, típicamente $x^{(0)} = (0,0,\dots,0)$ salvo que el enunciado indique otro.

### Criterio de corte

$$\max_i \left| x_i^{(k+1)} - x_i^{(k)} \right| < \varepsilon$$

(o el error relativo $\dfrac{|x_i^{(k+1)}-x_i^{(k)}|}{|x_i^{(k+1)}|} < \varepsilon$)

### Tabla de trabajo sugerida (sistema de 3 incógnitas)

| k | $x_1^{(k)}$ | $x_2^{(k)}$ | $x_3^{(k)}$ | error |
|---|---|---|---|---|

---

## 3. Método de Gauss-Seidel (Iterativo)

### Diferencia clave con Jacobi

Usa los valores **ya actualizados** en la misma iteración tan pronto como están disponibles, en lugar de esperar a la iteración siguiente. Esto generalmente **acelera la convergencia**.

### Fórmula

$$x_i^{(k+1)} = \frac{b_i - \displaystyle\sum_{j<i} a_{ij}\,x_j^{(k+1)} - \displaystyle\sum_{j>i} a_{ij}\,x_j^{(k)}}{a_{ii}}$$

Notar la diferencia con Jacobi: para $j<i$ (índices ya calculados en esta misma iteración $k+1$) se usa el valor **nuevo**; para $j>i$ (todavía no calculados) se usa el valor **viejo**.

**Ejemplo explícito para $n=3$:**

$$x_1^{(k+1)} = \frac{b_1 - a_{12}x_2^{(k)} - a_{13}x_3^{(k)}}{a_{11}}$$

$$x_2^{(k+1)} = \frac{b_2 - a_{21}x_1^{(k+1)} - a_{23}x_3^{(k)}}{a_{22}}$$

$$x_3^{(k+1)} = \frac{b_3 - a_{31}x_1^{(k+1)} - a_{32}x_2^{(k+1)}}{a_{33}}$$

(fijate que $x_1^{(k+1)}$ ya se usa para calcular $x_2^{(k+1)}$, y ambos para $x_3^{(k+1)}$).

### Condición de convergencia y criterio de corte
Igual que Jacobi: diagonal dominante estricta garantiza convergencia; corte por $\max_i|x_i^{(k+1)}-x_i^{(k)}| < \varepsilon$.

---

## 4. Método de Gauss-Seidel con Relajación (SOR)

### Idea
Se introduce un **factor de relajación** $\omega$ que combina el valor viejo con el valor que daría Gauss-Seidel, para acelerar (o estabilizar) la convergencia.

### Deducción de la fórmula

Primero se calcula el valor "candidato" de Gauss-Seidel normal, llamémoslo $\tilde{x}_i^{(k+1)}$:

$$\tilde{x}_i^{(k+1)} = \frac{b_i - \displaystyle\sum_{j<i} a_{ij}\,x_j^{(k+1)} - \displaystyle\sum_{j>i} a_{ij}\,x_j^{(k)}}{a_{ii}}$$

Y el valor final se toma como una combinación (ponderada por $\omega$) entre el valor anterior $x_i^{(k)}$ y este candidato:

$$x_i^{(k+1)} = (1-\omega)\,x_i^{(k)} + \omega\,\tilde{x}_i^{(k+1)}$$

Es decir, la fórmula completa queda:

$$x_i^{(k+1)} = (1-\omega)\,x_i^{(k)} + \omega\cdot\frac{b_i - \displaystyle\sum_{j<i} a_{ij}\,x_j^{(k+1)} - \displaystyle\sum_{j>i} a_{ij}\,x_j^{(k)}}{a_{ii}}$$

### Interpretación de $\omega$

- $\omega = 1$: es exactamente Gauss-Seidel normal.
- $0 < \omega < 1$: **subrelajación** (under-relaxation), útil cuando Gauss-Seidel oscila o diverge, sacrifica velocidad por estabilidad.
- $1 < \omega < 2$: **sobrerrelajación** (over-relaxation, SOR), acelera la convergencia si el sistema ya converge bien.
- Se requiere $0 < \omega < 2$ como condición necesaria para la convergencia.

### Criterio de corte
Igual que los anteriores: $\max_i |x_i^{(k+1)}-x_i^{(k)}| < \varepsilon$.

### Tabla de trabajo sugerida

| k | $x_1^{(k)}$ | $\tilde x_1$ | $x_1^{(k+1)}$ | $x_2^{(k)}$ | $\tilde x_2$ | $x_2^{(k+1)}$ | ... | error |
|---|---|---|---|---|---|---|---|---|
