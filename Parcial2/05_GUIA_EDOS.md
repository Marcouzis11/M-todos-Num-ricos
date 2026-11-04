# Guía para resolver EDOs de primer, segundo y tercer orden

Esta guía cubre cómo reconocer una EDO, resolver a mano los casos habituales y aplicar Euler o RK4 cuando se pide una aproximación. Incluye el cambio a sistemas que necesitan los códigos de `PARA EDITAR/EDOs`.

Para las integrales que aparezcan durante la resolución, consultá [la guía de integración](06_GUIA_INTEGRALES.md).

## 1. Antes de empezar: qué te están pidiendo

Una ecuación diferencial ordinaria (EDO) relaciona una función desconocida, por ejemplo $y(x)$, con sus derivadas respecto de una sola variable independiente.

| Concepto | Cómo reconocerlo | Ejemplo |
|---|---|---|
| Primer orden | La derivada más alta es $y'$ | $y'+2y=x$ |
| Segundo orden | La derivada más alta es $y''$ | $y''+3y'+2y=0$ |
| Tercer orden | La derivada más alta es $y'''$ | $y'''+y'=x$ |
| Lineal | $y$ y sus derivadas aparecen a la primera potencia, sin multiplicarse entre sí; sus coeficientes dependen solo de $x$ | $y''+xy'+y=\sin x$ |
| No lineal | Hay productos, potencias o funciones no lineales de las incógnitas | $y'=y^2$, $y''+yy'=0$ |
| Lineal homogénea | El término independiente de $y$ y sus derivadas es cero | $y''+y=0$ |
| Lineal no homogénea | Hay un término independiente distinto de cero | $y''+y=x$ |

El **orden** lo determina la derivada más alta, no la potencia: $y'=y^3$ es de primer orden.

### Condiciones iniciales y condiciones de contorno

Para un problema inicial regular se especifican, en general:

- Primer orden: $y(x_0)=y_0$.
- Segundo orden: $y(x_0)=y_0$ e $y'(x_0)=v_0$.
- Tercer orden: además, $y''(x_0)=a_0$.

Euler y RK4, tal como están implementados en las plantillas, resuelven **problemas de valores iniciales**, con todos esos datos en el mismo punto $x_0$.

Si te dan, por ejemplo, $y(0)=1$ e $y(2)=3$, tenés un problema de contorno. No podés inventar $y'(0)$: necesitás otro procedimiento, como disparo o diferencias finitas.

### Elegir el camino

| El enunciado pide… | Qué hacés |
|---|---|
| Solución general | Buscás una familia de funciones con constantes arbitrarias |
| Solución particular con condiciones iniciales | Encontrás la solución y determinás las constantes con los datos |
| Euler, RK4 o una tabla con paso $h$ | Aplicás el método numérico indicado |
| Una EDO de orden superior con Euler/RK4 | Despejás la derivada más alta y la convertís en un sistema de primer orden |
| Comprobar una solución | Derivás y reemplazás en la ecuación original y en las condiciones iniciales |

No toda EDO tiene una solución expresable con funciones elementales. Los métodos de abajo cubren formas frecuentes, no todas las ecuaciones posibles.

## 2. EDOs de primer orden: métodos analíticos

### 2.1. Integración directa: $y'=g(x)$

Integrás una vez:

$$
y(x)=\int g(x)\,dx+C.
$$

**Ejemplo:** $y'=3x^2-2$, con $y(0)=4$.

$$
y=x^3-2x+C,\qquad C=4.
$$

Resultado: $y=x^3-2x+4$.

### 2.2. Variables separables: $y'=g(x)h(y)$

**Cómo reconocerla:** podés dejar lo que depende de $y$ a un lado y lo que depende de $x$ al otro.

1. Buscá primero las soluciones constantes que cumplen $h(y)=0$.
2. Donde $h(y)\ne0$, separá variables.
3. Integrá ambos lados.
4. Despejá $y$ si se puede y usá las condiciones iniciales.

$$
\frac{dy}{h(y)}=g(x)\,dx
\quad\Longrightarrow\quad
\int\frac{dy}{h(y)}=\int g(x)\,dx+C.
$$

**Ejemplo:** $y'=-2xy$, con $y(0)=3$.

Para $y\ne0$:

$$
\frac{dy}{y}=-2x\,dx
\quad\Longrightarrow\quad
\ln|y|=-x^2+C.
$$

Absorbiendo el signo en una constante:

$$
y=Ce^{-x^2},\qquad y(0)=3\Longrightarrow C=3.
$$

La solución es $y=3e^{-x^2}$. La solución constante $y=0$ también resuelve la EDO, aunque no cumple este dato inicial.

**Cuidado:** dividir por $y$, por $y-1$ o por otra expresión puede eliminar soluciones. Revisá siempre cuándo se anula aquello por lo que dividiste.

### 2.3. Lineal de primer orden: $y'+P(x)y=Q(x)$

Si el coeficiente de $y'$ no es 1, primero dividí toda la ecuación por él, en un intervalo donde no se anule.

El **factor integrante** es:

$$
\mu(x)=e^{\int P(x)\,dx}.
$$

Multiplicás toda la ecuación por $\mu$. El lado izquierdo queda como una derivada de producto:

$$
(\mu y)'=\mu Q.
$$

Entonces:

$$
y=\frac{1}{\mu}\left(\int\mu Q\,dx+C\right).
$$

**Ejemplo:** $y'+2y=4$, con $y(0)=1$.

$$
\mu=e^{2x},\qquad (e^{2x}y)'=4e^{2x}.
$$

Integrando:

$$
e^{2x}y=2e^{2x}+C
\quad\Longrightarrow\quad
y=2+Ce^{-2x}.
$$

Como $1=2+C$, resulta $y=2-e^{-2x}$.

### 2.4. Bernoulli: $y'+P(x)y=Q(x)y^m$

Para $m\ne0,1$, usás:

$$
v=y^{1-m}.
$$

En un intervalo donde la sustitución sea válida, la ecuación se transforma en:

$$
v'+(1-m)P(x)v=(1-m)Q(x).
$$

Ahora es lineal en $v$. La resolvés y después volvés a $y$.

**Ejemplo:** $y'+y=y^2$, con $y(0)=2$.

Tomá $v=1/y$. Dividiendo por $y^2$:

$$
\frac{y'}{y^2}+\frac1y=1
\quad\Longrightarrow\quad
v'-v=-1.
$$

De aquí $v=1+Ce^x$, por lo tanto:

$$
y=\frac{1}{1+Ce^x}.
$$

Con $y(0)=2$, $C=-1/2$:

$$
y=\frac{1}{1-\tfrac12e^x}.
$$

El intervalo de esta solución que contiene a $0$ es $x<\ln2$: allí el denominador no se anula. Revisando la ecuación original, $y=0$ es otra solución que se perdió al dividir por $y^2$.

### 2.5. Homogénea de primer orden: $y'=F(y/x)$

Acá “homogénea” se refiere a esta forma particular de primer orden. No es el mismo criterio que “lineal homogénea”.

Usá, para $x\ne0$:

$$
v=\frac yx,\qquad y=xv,\qquad y'=v+xv'.
$$

La ecuación queda:

$$
xv'=F(v)-v.
$$

Habitualmente se puede separar. Si dividís por $F(v)-v$, revisá también sus ceros: pueden dar soluciones rectas $y=vx$.

**Ejemplo:** $y'=1+y/x$.

$$
v+xv'=1+v\Longrightarrow v'=\frac1x.
$$

Entonces $v=\ln|x|+C$ y:

$$
y=x(\ln|x|+C),\qquad x\ne0.
$$

### 2.6. Ecuación exacta: $M(x,y)\,dx+N(x,y)\,dy=0$

Con derivadas parciales continuas, comprobá:

$$
\frac{\partial M}{\partial y}=\frac{\partial N}{\partial x}.
$$

Esta igualdad permite construir un potencial local; en un dominio simplemente conexo también garantiza exactitud global. Buscás $\Phi$ tal que $\Phi_x=M$ y $\Phi_y=N$:

1. Integrá $M$ respecto de $x$, tratando $y$ como constante.
2. Agregá una función desconocida $g(y)$.
3. Derivá respecto de $y$ e igualá a $N$.
4. Hallá $g$ y escribí $\Phi(x,y)=C$.

**Ejemplo:**

$$
(2xy+3)\,dx+(x^2+2y)\,dy=0.
$$

Como $M_y=2x=N_x$, es exacta. Integrando $M$:

$$
\Phi=x^2y+3x+g(y).
$$

De $\Phi_y=x^2+g'(y)=x^2+2y$ sale $g(y)=y^2$. La solución implícita es:

$$
x^2y+3x+y^2=C.
$$

Si no es exacta, a veces existe un factor integrante sencillo:

- Si $(M_y-N_x)/N$ depende solo de $x$, podés usar $\mu(x)=e^{\int (M_y-N_x)/N\,dx}$.
- Si $(N_x-M_y)/M$ depende solo de $y$, podés usar $\mu(y)=e^{\int (N_x-M_y)/M\,dy}$.

Estas expresiones se usan donde los denominadores no se anulan. Después de multiplicar, volvés a comprobar exactitud.

## 3. EDOs de segundo orden: métodos analíticos

### 3.1. Si $y''=g(x)$, integrá dos veces

Cada integración incorpora una constante.

**Ejemplo:** $y''=6x$, con $y(0)=2$ e $y'(0)=1$.

$$
y'=3x^2+C_1,\qquad y=x^3+C_1x+C_2.
$$

Aplicando los datos: $C_1=1$, $C_2=2$. Resultado: $y=x^3+x+2$.

### 3.2. Lineal homogénea con coeficientes constantes

Forma:

$$
ay''+by'+cy=0,\qquad a\ne0.
$$

Planteá $y=e^{rx}$. Obtenés la **ecuación característica**:

$$
ar^2+br+c=0.
$$

| Raíces | Solución general |
|---|---|
| Reales distintas $r_1,r_2$ | $y=C_1e^{r_1x}+C_2e^{r_2x}$ |
| Real doble $r$ | $y=(C_1+C_2x)e^{rx}$ |
| Complejas $\alpha\pm i\beta$, $\beta>0$ | $y=e^{\alpha x}(C_1\cos\beta x+C_2\sin\beta x)$ |

**Ejemplo completo:** $y''+3y'+2y=0$, con $y(0)=1$, $y'(0)=0$.

$$
r^2+3r+2=(r+1)(r+2)=0.
$$

Entonces:

$$
y=C_1e^{-x}+C_2e^{-2x},\qquad
y'=-C_1e^{-x}-2C_2e^{-2x}.
$$

Usando las condiciones:

$$
C_1+C_2=1,\qquad -C_1-2C_2=0.
$$

De aquí $C_1=2$, $C_2=-1$ y $y=2e^{-x}-e^{-2x}$.

### 3.3. Lineal no homogénea: $ay''+by'+cy=g(x)$

La estructura es:

$$
y=y_h+y_p.
$$

$y_h$ resuelve la homogénea asociada. $y_p$ es una solución particular de la ecuación completa.

#### Coeficientes indeterminados

Sirve con coeficientes constantes cuando $g(x)$ es un polinomio, exponencial, seno, coseno o combinaciones de estos tipos.

| Término $g(x)$ | Propuesta inicial para $y_p$ |
|---|---|
| Constante | $A$ |
| Polinomio de grado $m$ | Polinomio completo de grado $m$, incluidos los términos menores |
| $e^{\lambda x}$ | $Ae^{\lambda x}$ |
| $\sin\omega x$ o $\cos\omega x$ | $A\cos\omega x+B\sin\omega x$ |
| $e^{\alpha x}\cos\beta x$ o $e^{\alpha x}\sin\beta x$ | $e^{\alpha x}(A\cos\beta x+B\sin\beta x)$ |

**Resonancia:** si la propuesta coincide con términos de $y_h$, multiplicá toda la propuesta por $x^s$, donde $s$ es la multiplicidad de la raíz característica correspondiente.

**Ejemplo:** $y''+y=2$.

$$
y_h=C_1\cos x+C_2\sin x.
$$

Proponés $y_p=A$. Al sustituir: $A=2$.

$$
y=C_1\cos x+C_2\sin x+2.
$$

**Ejemplo con resonancia:** $y''+y=\cos x$.

Como $\cos x$ y $\sin x$ ya están en $y_h$, probás:

$$
y_p=x(A\cos x+B\sin x).
$$

Sustituyendo se obtiene $A=0$, $B=1/2$. Entonces:

$$
y=C_1\cos x+C_2\sin x+\frac{x}{2}\sin x.
$$

Aplicá las condiciones iniciales a **$y_h+y_p$**, incluyendo la derivada de $y_p$.

#### Variación de parámetros

Sirve para una ecuación lineal normalizada:

$$
y''+p(x)y'+q(x)y=g(x),
$$

si conocés dos soluciones independientes $u_1,u_2$ de la homogénea. Definí:

$$
W=u_1u_2'-u_1'u_2.
$$

En un intervalo donde $W\ne0$, una solución particular es:

$$
y_p=-u_1\int\frac{u_2g}{W}\,dx
       +u_2\int\frac{u_1g}{W}\,dx.
$$

Las constantes de estas integrales pueden tomarse cero: sus aportes ya están incluidos en $y_h$. Si originalmente había $a(x)y''$, dividí primero por $a(x)$; eso también cambia el término $g$.

### 3.4. Reducción de orden en casos especiales

| Forma | Sustitución | Qué obtenés |
|---|---|---|
| No aparece $y$: $y''=F(x,y')$ | $v=y'$ | $v'=F(x,v)$; después integrás $y'=v$ |
| No aparece $x$: $y''=F(y,y')$ | $v(y)=y'$ | $y''=v\,dv/dy$; resolvés para $v$ y después $dy/dx=v(y)$ |

En el segundo caso se trabaja localmente donde pueda usarse $y$ como variable independiente. Revisá aparte las soluciones constantes o los tramos donde $y'=0$.

Si conocés una solución no nula $u_1$ de una ecuación homogénea normalizada $y''+p(x)y'+q(x)y=0$, otra solución independiente puede calcularse con:

$$
u_2=u_1\int\frac{e^{-\int p(x)\,dx}}{u_1^2}\,dx.
$$

## 4. EDOs de tercer orden: métodos analíticos

### 4.1. Si $y'''=g(x)$, integrá tres veces

**Ejemplo:** $y'''=6$, con $y(0)=1$, $y'(0)=2$, $y''(0)=3$.

$$
y''=6x+C_1,
$$

$$
y'=3x^2+C_1x+C_2,
$$

$$
y=x^3+\frac{C_1}{2}x^2+C_2x+C_3.
$$

Con los datos, $C_1=3$, $C_2=2$, $C_3=1$:

$$
y=x^3+\frac32x^2+2x+1.
$$

### 4.2. Lineal homogénea con coeficientes constantes

Para:

$$
ay'''+by''+cy'+dy=0,\qquad a\ne0,
$$

la característica es:

$$
ar^3+br^2+cr+d=0.
$$

Cada raíz aporta términos según su multiplicidad:

- Raíz real simple $r$: $Ce^{rx}$.
- Raíz real doble $r$: $(C_1+C_2x)e^{rx}$.
- Raíz real triple $r$: $(C_1+C_2x+C_3x^2)e^{rx}$.
- Par complejo $\alpha\pm i\beta$: $e^{\alpha x}(C_1\cos\beta x+C_2\sin\beta x)$.

Tenés que reunir **tres constantes independientes**.

**Ejemplo con tres raíces distintas:**

$$
y'''-y'=0\Longrightarrow r^3-r=r(r-1)(r+1)=0.
$$

$$
y=C_1+C_2e^x+C_3e^{-x}.
$$

**Ejemplo con raíz triple:**

$$
y'''-3y''+3y'-y=0
\Longrightarrow (r-1)^3=0.
$$

$$
y=(C_1+C_2x+C_3x^2)e^x.
$$

**Ejemplo con raíces complejas y condiciones iniciales:**

$$
y'''+y'=0,\qquad y(0)=1,\quad y'(0)=2,\quad y''(0)=0.
$$

La característica es $r(r^2+1)=0$, por lo que:

$$
y=C_1+C_2\cos x+C_3\sin x.
$$

Derivando y usando los datos:

$$
C_1+C_2=1,\qquad C_3=2,\qquad -C_2=0.
$$

Resultado: $y=1+2\sin x$.

### 4.3. Lineal no homogénea

También se usa $y=y_h+y_p$. Para coeficientes constantes valen las propuestas de coeficientes indeterminados y la misma regla de resonancia de la sección anterior.

**Ejemplo:** $y'''-y'=2$.

La homogénea tiene solución $y_h=C_1+C_2e^x+C_3e^{-x}$. Una constante ya aparece en $y_h$, así que para el término independiente constante probás $y_p=Ax$.

$$
y_p'''-y_p'=0-A=2\Longrightarrow A=-2.
$$

$$
y=C_1+C_2e^x+C_3e^{-x}-2x.
$$

Si no aparece $y$ en la ecuación, también podés hacer $v=y'$ y reducir el problema a una EDO de segundo orden para $v$. Después integrás $y'=v$ y agregás la constante restante.

### 4.4. Caso adicional: Euler-Cauchy

No lo confundas con el **método numérico de Euler**. Es una familia de ecuaciones con coeficientes variables:

$$
ax^2y''+bxy'+cy=0
$$

o, en tercer orden:

$$
ax^3y'''+bx^2y''+cxy'+dy=0.
$$

En $x>0$ probás $y=x^r$. Las características correspondientes son:

$$
ar(r-1)+br+c=0
$$

y

$$
ar(r-1)(r-2)+br(r-1)+cr+d=0.
$$

Las raíces repetidas agregan factores $\ln x$, $(\ln x)^2$, etc. Un par complejo $\alpha\pm i\beta$ aporta $x^\alpha\cos(\beta\ln x)$ y $x^\alpha\sin(\beta\ln x)$. También podés usar $t=\ln x$ para transformar la ecuación en una de coeficientes constantes. En $x<0$ se trabaja por separado con $t=\ln|x|$.

**Ejemplo:** $x^2y''-xy'+y=0$, para $x>0$.

$$
r(r-1)-r+1=(r-1)^2=0
\Longrightarrow y=x(C_1+C_2\ln x).
$$

## 5. El cambio clave para resolver numéricamente

Euler y RK4 se aplican a una ecuación o a un **sistema de primer orden**. Para una ecuación de orden superior, primero despejás la derivada más alta y después introducís nuevas incógnitas.

| EDO original | Definiciones | Sistema equivalente |
|---|---|---|
| $y'=f(x,y)$ | $y_1=y$ | $y_1'=f(x,y_1)$ |
| $y''=g(x,y,y')$ | $y_1=y$, $y_2=y'$ | $y_1'=y_2$, $y_2'=g(x,y_1,y_2)$ |
| $y'''=g(x,y,y',y'')$ | $y_1=y$, $y_2=y'$, $y_3=y''$ | $y_1'=y_2$, $y_2'=y_3$, $y_3'=g(x,y_1,y_2,y_3)$ |

### Ejemplo de segundo orden

$$
y''+2y'+5y=\sin x
\quad\Longrightarrow\quad
y''=\sin x-2y'-5y.
$$

Definís $y_1=y$, $y_2=y'$:

$$
\begin{cases}
y_1'=y_2,\\
y_2'=\sin x-2y_2-5y_1.
\end{cases}
$$

Si los datos eran $y(0)=1$, $y'(0)=0$, ahora son $y_1(0)=1$, $y_2(0)=0$.

En las funciones del código:

```cpp
double f1(double x, double y1, double y2) {
    return y2;
}

double f2(double x, double y1, double y2) {
    return sin(x) - 2*y2 - 5*y1;
}
```

### Ejemplo de tercer orden

$$
y'''+y''+2y'-y=x
\quad\Longrightarrow\quad
y'''=x-y''-2y'+y.
$$

Definís $y_1=y$, $y_2=y'$, $y_3=y''$:

$$
\begin{cases}
y_1'=y_2,\\
y_2'=y_3,\\
y_3'=x-y_3-2y_2+y_1.
\end{cases}
$$

En el código:

```cpp
double f1(double x, double y1, double y2, double y3) {
    return y2;
}

double f2(double x, double y1, double y2, double y3) {
    return y3;
}

double f3(double x, double y1, double y2, double y3) {
    return x - y3 - 2*y2 + y1;
}
```

**Un sistema de dos ecuaciones no necesariamente proviene de una sola EDO de segundo orden.** También puede venir dado directamente: $y_1'=f_1(x,y_1,y_2)$, $y_2'=f_2(x,y_1,y_2)$. En ese caso copiás ambas funciones, sin imponer que $f_1=y_2$.

## 6. Euler explícito para 1, 2 o 3 ecuaciones

En cada paso:

$$
x_{i+1}=x_i+h.
$$

Para una ecuación:

$$
y_{i+1}=y_i+h f(x_i,y_i).
$$

Para dos:

$$
\begin{aligned}
y_{1,i+1}&=y_{1,i}+h f_1(x_i,y_{1,i},y_{2,i}),\\
y_{2,i+1}&=y_{2,i}+h f_2(x_i,y_{1,i},y_{2,i}).
\end{aligned}
$$

Para tres:

$$
\begin{aligned}
y_{1,i+1}&=y_{1,i}+h f_1(x_i,y_{1,i},y_{2,i},y_{3,i}),\\
y_{2,i+1}&=y_{2,i}+h f_2(x_i,y_{1,i},y_{2,i},y_{3,i}),\\
y_{3,i+1}&=y_{3,i}+h f_3(x_i,y_{1,i},y_{2,i},y_{3,i}).
\end{aligned}
$$

**Todas las derivadas del paso usan el mismo punto anterior.** En los arreglos, todos los argumentos de las funciones llevan `[i]`, nunca `[i+1]`.

### Ejemplo de primer orden: dos pasos

$y'=x+y$, $y(0)=1$, $h=0.1$.

Primer paso:

$$
f(0,1)=1,\qquad y_1=1+0.1(1)=1.1.
$$

Segundo paso:

$$
f(0.1,1.1)=1.2,\qquad y_2=1.1+0.1(1.2)=1.22.
$$

Resultado: $y(0.2)\approx1.22$.

En este ejemplo $y_1,y_2$ son valores en pasos sucesivos de una sola incógnita; en los sistemas usamos $y_{j,i}$ para distinguir componente $j$ y paso $i$.

### Ejemplo de segundo orden: primera iteración a mano

$$
y''=-2y'-5y,\qquad y(0)=1,\qquad y'(0)=0,\qquad h=0.1.
$$

Sistema: $y_1'=y_2$, $y_2'=-2y_2-5y_1$.

Pendientes en el punto inicial:

$$
f_1(0,1,0)=0,\qquad f_2(0,1,0)=-5.
$$

Actualización:

$$
y_{1,1}=1+0.1(0)=1,\qquad
y_{2,1}=0+0.1(-5)=-0.5.
$$

Por lo tanto $y(0.1)\approx1$ e $y'(0.1)\approx-0.5$.

### Ejemplo de tercer orden: primera iteración a mano

$$
y'''=x-y''-2y'+y,\qquad
y(0)=1,\quad y'(0)=0,\quad y''(0)=2,\quad h=0.1.
$$

Pendientes iniciales:

$$
f_1=0,\qquad f_2=2,\qquad f_3=0-2-2(0)+1=-1.
$$

Actualización:

$$
y_{1,1}=1,\qquad y_{2,1}=0.2,\qquad y_{3,1}=1.9.
$$

Así, $y(0.1)\approx1$, $y'(0.1)\approx0.2$, $y''(0.1)\approx1.9$.

## 7. RK4 para una ecuación o un sistema

Para evitar escribir la fórmula tres veces, agrupá las incógnitas en un vector:

$$
\mathbf Y=(y_1,y_2,y_3),\qquad
\mathbf F=(f_1,f_2,f_3).
$$

Con una o dos ecuaciones usás solo las componentes correspondientes. Las cuatro etapas son:

$$
\begin{aligned}
\mathbf k_1&=\mathbf F(x_i,\mathbf Y_i),\\
\mathbf k_2&=\mathbf F(x_i+h/2,\mathbf Y_i+(h/2)\mathbf k_1),\\
\mathbf k_3&=\mathbf F(x_i+h/2,\mathbf Y_i+(h/2)\mathbf k_2),\\
\mathbf k_4&=\mathbf F(x_i+h,\mathbf Y_i+h\mathbf k_3).
\end{aligned}
$$

Finalmente:

$$
\mathbf Y_{i+1}=\mathbf Y_i+
\frac h6(\mathbf k_1+2\mathbf k_2+2\mathbf k_3+\mathbf k_4).
$$

En esta convención, los $k$ son **pendientes**, sin multiplicar por $h$. Hay libros que incluyen $h$ dentro de los $k$: no mezcles ambas convenciones.

Para sistemas, calculá **todas las componentes de una etapa antes de la siguiente**. En las plantillas, `k21` significa etapa 2, ecuación 1; `k12`, etapa 1, ecuación 2.

### Ejemplo escalar: $y'=y$, $y(0)=1$, $h=0.1$

$$
k_1=1,
$$

$$
k_2=1+0.05(1)=1.05,
$$

$$
k_3=1+0.05(1.05)=1.0525,
$$

$$
k_4=1+0.1(1.0525)=1.10525.
$$

$$
y(0.1)\approx1+\frac{0.1}{6}(1+2(1.05)+2(1.0525)+1.10525)
=1.1051708333.
$$

## 8. Qué archivo usar y qué cambiar

| Cantidad de ecuaciones de primer orden después de reducir | Euler | RK4 |
|---|---|---|
| 1 | [Euler_1Ecuacion.cpp](PARA%20EDITAR/EDOs/Euler_1Ecuacion.cpp) | [RK4_1Ecuacion.cpp](PARA%20EDITAR/EDOs/RK4_1Ecuacion.cpp) |
| 2 | [Euler_2Ecuaciones.cpp](PARA%20EDITAR/EDOs/Euler_2Ecuaciones.cpp) | [RK4_2Ecuaciones.cpp](PARA%20EDITAR/EDOs/RK4_2Ecuaciones.cpp) |
| 3 | [Euler_3Ecuaciones.cpp](PARA%20EDITAR/EDOs/Euler_3Ecuaciones.cpp) | [RK4_3Ecuaciones.cpp](PARA%20EDITAR/EDOs/RK4_3Ecuaciones.cpp) |

En cada plantilla cambiás:

1. Los `return` de `f1`, `f2` y `f3`, según cuántas ecuaciones haya.
2. `x[0]`, el punto inicial.
3. `y1[0]`, `y2[0]` y `y3[0]`, según las condiciones iniciales.
4. `h`, el paso.
5. `n`, la cantidad de pasos.

En estas plantillas:

$$
x_f=x_0+nh.
$$

Con $n$ pasos tenés $n+1$ puntos, contando el inicial. Si $(x_f-x_0)/h$ no es entero, tenés que ajustar el paso o programar un último paso más corto. Estas plantillas usan $h$ fijo.

Para ir hacia atrás, usá $h<0$ y una cantidad positiva de pasos.

En C++, escribí `sin(x)`, `cos(x)`, `exp(x)`, `sqrt(y1)` y `pow(y1, 2)` o `y1*y1`. **`^` no calcula potencias en C++.** En el evaluador interactivo de `CODIGO/`, en cambio, sí se acepta `^`.

El programa [edo.cpp](CODIGO/edo.cpp), al compilarlo, ofrece Euler escalar en la opción 2, RK4 escalar en la 6, segundo orden/sistema de dos ecuaciones con Euler o RK4 en la 11, y sistemas con Euler en las opciones 13 y 14. Ver también [SISTEMAS_EDO.md](CODIGO/SISTEMAS_EDO.md).

## 9. Precisión, error y estabilidad

| Método | Orden global habitual | Al dividir $h$ por 2, el error se reduce aproximadamente… |
|---|---|---|
| Euler explícito | $O(h)$ | A la mitad |
| Heun / punto medio | $O(h^2)$ | A la cuarta parte |
| RK4 clásico | $O(h^4)$ | A la dieciseisava parte |

Esto supone una solución suficientemente regular, un intervalo fijo y pasos dentro del régimen de convergencia. Un paso inestable puede dar resultados malos aunque la fórmula esté bien implementada.

Si conocés la solución exacta:

$$
E_{\text{abs}}=|y_{\text{exacta}}-y_{\text{aprox}}|.
$$

$$
E_{\text{rel}}=\frac{|y_{\text{exacta}}-y_{\text{aprox}}|}{|y_{\text{exacta}}|},
\qquad E_{\%}=100E_{\text{rel}}.
$$

El error relativo no está definido cuando el valor exacto es cero.

Si no conocés la exacta, compará los resultados con $h$ y $h/2$ en el **mismo punto final**. Para un método de orden $p$, en el régimen asintótico:

$$
E_{h/2}\approx\frac{|y_{h/2}-y_h|}{2^p-1}.
$$

En sistemas, hacé esta comparación por componente o usando una norma del vector de diferencias.

**Ejemplo de estabilidad:** para $y'=-\lambda y$ con $\lambda>0$, Euler produce $y_{i+1}=(1-\lambda h)y_i$. Con $h>0$ necesita $|1-\lambda h|<1$, es decir $0<h<2/\lambda$, para reproducir el decaimiento sin crecimiento artificial. Si una ecuación exige pasos extremadamente pequeños por estabilidad, puede ser necesario un método implícito.

## 10. Lista para resolver un ejercicio de parcial

1. Identificá el orden y qué solución o aproximación pide el enunciado.
2. Anotá las condiciones iniciales y el punto donde tenés que responder.
3. Si vas a resolver a mano, clasificá la ecuación y elegí un método compatible.
4. Si usás Euler/RK4, despejá la derivada más alta.
5. Para segundo o tercer orden, escribí el cambio de variables, el sistema y los datos transformados.
6. Calculá $h$ o $n$, recordando que hay $n+1$ puntos.
7. Desarrollá la primera iteración con los datos anteriores en todas las componentes.
8. Completá la tabla y recuperá qué componente representa $y$, $y'$ o $y''$.
9. Si tenés solución exacta, comprobá la ecuación, las condiciones iniciales y el error.
10. No confundas el orden de la EDO con el orden de precisión del método: una EDO de tercer orden también se puede aproximar con Euler, que es un método de orden 1.
