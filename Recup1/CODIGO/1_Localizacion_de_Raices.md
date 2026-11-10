# Localización de Raíces

---

## 1. Bisección (Método Cerrado)

### Condición inicial
Necesitás un intervalo $[a, b]$ tal que:

$$f(a) \cdot f(b) < 0$$

(la función cambia de signo → el Teorema de Bolzano garantiza al menos una raíz en $(a,b)$).

### Algoritmo (a mano)

1. Calculá el punto medio:
$$c = \frac{a+b}{2}$$

2. Evaluá $f(c)$.

3. Decidí el nuevo subintervalo:
   - Si $f(c) \cdot f(b) > 0$ → la raíz está en $[a, c]$ → actualizás $b = c$
   - Si $f(c) \cdot f(b) < 0$ → la raíz está en $[c, b]$ → actualizás $a = c$
   - Si $f(c) = 0$ → $c$ es la raíz exacta, termina

4. Repetir hasta cumplir el criterio de corte.

### Criterios de corte (cualquiera de estos)

- Por error estimado: $ e \approx |c_{nuevo} - c_{viejo}|$
- Por error porcentual: $ \varepsilon = \frac{|c_{nuevo} - c_{viejo}|}{|c_{nuevo}|} \cdot 100$
- Por número máximo de iteraciones

---

## 2. Regula Falsi / Falsa Posición (Método Cerrado)

### Condición inicial
Igual que Bisección: $f(a)\cdot f(b) < 0$.

### Idea
En vez de partir el intervalo por la mitad, se traza la recta secante entre $(a,f(a))$ y $(b,f(b))$, y se toma como aproximación el punto donde esa recta corta al eje $x$.

### Deducción de la fórmula

Ecuación de la recta que pasa por $(a,f(a))$ y $(b,f(b))$:

$$\frac{y - f(b)}{x - b} = \frac{f(b)-f(a)}{b-a}$$

Igualamos $y=0$ (corte con el eje x) y despejamos $x=c$:

$$0 - f(b) = \frac{f(b)-f(a)}{b-a}(c-b)$$

$$c = b - f(b)\cdot\frac{b-a}{f(b)-f(a)}$$

Forma equivalente (la más usada):

$$c = \frac{a\cdot f(b) - b\cdot f(a)}{f(b)-f(a)}$$

### Algoritmo

1. Calculá $c$ con la fórmula anterior.
2. Evaluá $f(c)$.
3. Igual que en Bisección:
   - Si $f(c)\cdot f(b) < 0 \Rightarrow a = c$
   - Si $f(c)\cdot f(b) > 0 \Rightarrow b = c$
   - Si $f(c)\cdot f(b) = 0 \Rightarrow c$ es la raíz.
4. Repetir hasta el criterio de corte.

### Criterios de corte y error
- Por error estimado: $ e \approx |c_{nuevo} - c_{viejo}|$
- Por error porcentual: $ \varepsilon = \frac{|c_{nuevo} - c_{viejo}|}{|c_{nuevo}|} \cdot 100$

---

## 3. Punto Fijo (Método Abierto)

### Idea
Transformar $f(x) = 0$ en una forma equivalente $x = g(x)$, la idea sería despejar o sumar $x$ a ambos lados (es mejor probar despejar). Si $x^*$ es raíz de $f$, entonces $x^*$ es punto fijo de $g$ (es decir $g(x^*) = x^*$).

### Iteración

$$x_{i+1} = g(x_i)$$

Requiere inicializar un $x_0$.

### Cómo elegir $g(x)$ (paso clave que se hace a mano)

De $f(x) = 0$ hay que despejar $x$ de alguna manera. Por ejemplo, si $f(x) = x^2 - x - 2 = 0$, algunas formas posibles:

- $x = x^2 - 2 \;\Rightarrow\; g_1(x) = x^2-2$
- $x = \sqrt{x+2} \;\Rightarrow\; g_2(x) = \sqrt{x+2}$
- $x = 1 + \dfrac{2}{x} \;\Rightarrow\; g_3(x) = 1+\dfrac{2}{x}$

### Algoritmo

Supongamos $x_0 = 0$.

1. Calculá $x_1 = g(x_0)$ con la fórmula anterior.
2. $x_2 = g(x_1)$
3. $x_3 = g(x_2)$

No todas convergen: hay que verificar la **condición de convergencia**.

### Condición de convergencia (Teorema del punto fijo)

$g$ converge localmente cerca de la raíz si y solo si:

$$|g'(x)| < 1 \quad$$

Esto hay que chequearlo derivando $g(x)$ y evaluando en un valor cercano a la raíz esperada (por ejemplo, usando el intervalo dado o una primera estimación gráfica).

### Criterios de corte
- Por error estimado: $ e \approx |x_{i+1} - x_{i}|$
- Por error porcentual: $ \varepsilon = \frac{|x_{i+1} - x_{i}|}{|x_{i+1}|} \cdot 100$


---

## 4. Newton-Raphson (Método Abierto)

### Deducción (a partir de Taylor)

Se aproxima $f(x)$ cerca de $x_n$ mediante su recta tangente (Taylor de primer orden):

$$f(x) \approx f(x_n) + f'(x_n)(x - x_n)$$

Buscamos el $x$ donde esa recta tangente corta al eje horizontal, es decir donde $f(x)=0$:

$$0 = f(x_n) + f'(x_n)(x_{n+1} - x_n)$$

Despejando $x_{n+1}$:

$$x_{i+1} = x_i - \frac{f(x_i)}{f'(x_i)}$$

### Requisitos
- No converge si $f'(x_n) = 0$
- No converge si $f'$ es muy grande (función empinada) (en realidad converge lento).

### Criterios de corte
- Por error estimado: $ e \approx |x_{i+1} - x_{i}|$
- Por error porcentual: $ \varepsilon = \frac{|x_{i+1} - x_{i}|}{|x_{i+1}|} \cdot 100$

### Ventaja/uso en examen
Convergencia **cuadrática** (mucho más rápida que Bisección o Punto Fijo) cuando converge, pero requiere calcular $f'(x)$ analíticamente antes de iterar — **este despeje de la derivada hay que hacerlo primero en la hoja**.

---

## 5. Método de la Secante (Método Abierto)

### Idea
Es una variante de Newton-Raphson que evita calcular $f'(x)$ analíticamente: se la aproxima numéricamente usando los dos últimos puntos.

### Deducción

Aproximamos la derivada como cociente incremental entre los dos últimos iterados:

$$f'(x_i) \approx \frac{f(x_i) - f(x_{i-1})}{x_i - x_{i-1}}$$

Sustituimos esta aproximación en la fórmula de Newton-Raphson:

$$x_{i+1} = x_i - f(x_i)\cdot\frac{x_{i-1} - x_{i}}{f(x_{i-1})-f(x_i)}$$

### Requisitos
- Se necesitan **dos valores iniciales** $x_0$ y $x_1$ (no hace falta que encierren la raíz, a diferencia de Bisección/Regula Falsi).
- $f(x_i) \neq f(x_{i-1})$ en cada paso.

### Criterios de corte
- Por error estimado: $ e \approx |x_{i+1} - x_{i}|$
- Por error porcentual: $ \varepsilon = \frac{|x_{i+1} - x_{i}|}{|x_{i+1}|} \cdot 100$

### Comparación rápida con Regula Falsi (para no confundir en el examen)
- **Secante**: usa siempre los **dos últimos** puntos calculados (no verifica cambio de signo, es método abierto).
- **Regula Falsi**: usa un punto fijo (el extremo del intervalo que mantiene el cambio de signo) y el nuevo punto (es método cerrado, siempre encierra la raíz).

