# Guía de técnicas para resolver integrales

Una guía para elegir el método, hacer el procedimiento y verificar el resultado. Incluye las técnicas analíticas habituales y los métodos numéricos del parcial. Para aplicarlas en ecuaciones diferenciales, consultá [la guía de EDOs](05_GUIA_EDOS.md).

## 1. Cómo decidir qué técnica usar

| Qué ves en el integrando | Qué probar |
|---|---|
| Sumas, constantes, potencias sencillas | Separar e integrar con la tabla |
| Una función compuesta acompañada por la derivada de lo de adentro | Sustitución $u=g(x)$ |
| Un producto como $xe^x$, $x\sin x$ o una función como $\ln x$ | Integración por partes |
| Un cociente de polinomios | División y fracciones parciales |
| Potencias o productos de senos y cosenos | Identidades trigonométricas y sustitución |
| $\sqrt{a^2-x^2}$, $\sqrt{a^2+x^2}$ o $\sqrt{x^2-a^2}$ | Completar cuadrados y/o sustitución trigonométrica |
| Potencias fraccionarias de $x$ | Sustitución que elimine las raíces |
| Una función racional de $\sin x$ y $\cos x$ | Puede servir $t=\tan(x/2)$ |
| Integral definida en un intervalo simétrico | Comprobar si la función es par o impar |
| Extremos infinitos o puntos donde la función se hace infinita | Integral impropia: trabajar con límites |
| Tabla de datos o una integral definida difícil | Trapecio, Simpson, Gauss o integración de un spline |

El orden práctico es: **simplificar → reconocer una fórmula → sustitución → partes → técnicas específicas**. Un ejercicio puede necesitar varias técnicas combinadas.

## 2. Qué significa integrar

Una integral indefinida busca una **primitiva**:

$$
\int f(x)\,dx=F(x)+C,\qquad F'(x)=f(x).
$$

Una integral definida calcula un valor. Si $f$ es continua en $[a,b]$ y $F'=f$, el teorema fundamental del cálculo da:

$$
\int_a^b f(x)\,dx=F(b)-F(a).
$$

En la indefinida agregás $C$; en la definida, las constantes se cancelan al evaluar.

### Propiedades que podés usar

$$
\int[\alpha f(x)+\beta g(x)]\,dx
=\alpha\int f(x)\,dx+\beta\int g(x)\,dx.
$$

$$
\int_a^b f=-\int_b^a f,\qquad
\int_a^a f=0,\qquad
\int_a^b f=\int_a^c f+\int_c^b f.
$$

Las propiedades de integrales definidas requieren que las integrales involucradas existan. **No hay una regla para integrar un producto multiplicando sus primitivas, ni un cociente dividiéndolas.**

## 3. Tabla de primitivas básicas

Las fórmulas se usan en intervalos donde las funciones estén definidas. En las fórmulas con $a$, suponé $a>0$.

| Integral | Primitiva, más $C$ |
|---|---|
| $\int k\,dx$ | $kx$ |
| $\int x^p\,dx$, $p\ne-1$ | $x^{p+1}/(p+1)$ |
| $\int dx/x$ | $\ln\lvert x\rvert$ |
| $\int e^x\,dx$ | $e^x$ |
| $\int e^{kx}\,dx$, $k\ne0$ | $e^{kx}/k$ |
| $\int a^x\,dx$, $a>0$, $a\ne1$ | $a^x/\ln a$ |
| $\int\sin x\,dx$ | $-\cos x$ |
| $\int\cos x\,dx$ | $\sin x$ |
| $\int\sec^2x\,dx$ | $\tan x$ |
| $\int\csc^2x\,dx$ | $-\cot x$ |
| $\int\sec x\tan x\,dx$ | $\sec x$ |
| $\int\csc x\cot x\,dx$ | $-\csc x$ |
| $\int\tan x\,dx$ | $-\ln\lvert\cos x\rvert$ |
| $\int\cot x\,dx$ | $\ln\lvert\sin x\rvert$ |
| $\int\sec x\,dx$ | $\ln\lvert\sec x+\tan x\rvert$ |
| $\int\csc x\,dx$ | $\ln\lvert\csc x-\cot x\rvert$ |
| $\int dx/(a^2+x^2)$ | $\frac1a\arctan(x/a)$ |
| $\int dx/\sqrt{a^2-x^2}$ | $\arcsin(x/a)$ |
| $\int dx/(x^2-a^2)$ | $\frac{1}{2a}\ln\left\lvert\frac{x-a}{x+a}\right\rvert$ |
| $\int dx/\sqrt{x^2+a^2}$ | $\ln\lvert x+\sqrt{x^2+a^2}\rvert$ |

Para potencias reales no enteras, comprobá el dominio. Si el argumento es $kx+b$, aparece un factor $1/k$ por la regla de la cadena.

**Ejemplo:**

$$
\int\cos(3x+2)\,dx=\frac13\sin(3x+2)+C.
$$

### Simplificar antes de elegir un método

$$
\int\frac{x^2+2x+1}{x}\,dx
=\int\left(x+2+\frac1x\right)dx
=\frac{x^2}{2}+2x+\ln|x|+C.
$$

También sirve expandir productos, factorizar o escribir $\sqrt{x}=x^{1/2}$ y $1/x^3=x^{-3}$. Una cancelación no vuelve válido el integrando original en un punto donde no estaba definido.

## 4. Sustitución o cambio de variable

Es la regla de la cadena al revés. Si elegís $u=g(x)$, entonces $du=g'(x)\,dx$.

1. Elegí la expresión interna como $u$.
2. Calculá $du$.
3. Reescribí **toda** la integral en términos de $u$.
4. Integrá y, si es indefinida, volvé a $x$.

### Ejemplo: función compuesta con su derivada

$$
\int 2x\cos(x^2+1)\,dx.
$$

Tomá $u=x^2+1$, $du=2x\,dx$:

$$
\int\cos u\,du=\sin u+C
=\sin(x^2+1)+C.
$$

### Ejemplo: acomodar una constante

$$
\int x\sqrt{x^2+1}\,dx.
$$

Con $u=x^2+1$, $x\,dx=du/2$:

$$
\frac12\int u^{1/2}\,du
=\frac13u^{3/2}+C
=\frac13(x^2+1)^{3/2}+C.
$$

### Patrón frecuente: derivada sobre función

$$
\int\frac{g'(x)}{g(x)}\,dx=\ln|g(x)|+C,
\qquad g(x)\ne0.
$$

Por ejemplo, $\int 2x/(x^2+5)\,dx=\ln(x^2+5)+C$.

### Sustitución en una integral definida

Si cambiás de variable, cambiá también los límites:

$$
\int_a^b f(g(x))g'(x)\,dx=\int_{g(a)}^{g(b)}f(u)\,du.
$$

**Ejemplo:**

$$
\int_0^1 2xe^{x^2}\,dx
=\int_0^1 e^u\,du=e-1.
$$

Acá $u=x^2$, de modo que $x=0\Rightarrow u=0$ y $x=1\Rightarrow u=1$. Los límites coinciden numéricamente por este ejemplo; en general cambian. No evalúes una primitiva en $u$ usando límites que todavía representan $x$.

## 5. Integración por partes

Sale de la derivada de un producto:

$$
\boxed{\int u\,dv=uv-\int v\,du.}
$$

Elegí $u$ para que **se simplifique al derivarlo**, y $dv$ para que puedas integrarlo.

Una ayuda habitual para elegir $u$ es LIATE: logarítmicas, inversas trigonométricas, algebraicas, trigonométricas, exponenciales. Es una orientación; lo que decide es que la integral restante sea más sencilla.

### Ejemplo: $\int xe^x\,dx$

$$
u=x,\quad du=dx,\quad dv=e^x\,dx,\quad v=e^x.
$$

$$
\int xe^x\,dx=xe^x-\int e^x\,dx
=e^x(x-1)+C.
$$

### Ejemplo: integrar un logaritmo solo

Aunque no veas un producto, podés escribir $\ln x\cdot1$, para $x>0$:

$$
u=\ln x,\quad dv=dx,\quad du=dx/x,\quad v=x.
$$

$$
\int\ln x\,dx=x\ln x-x+C.
$$

### Partes repetidas y método tabular

Si aparece un polinomio multiplicado por una exponencial, seno o coseno, podés repetir partes hasta que las derivadas del polinomio se anulen.

$$
\int x^2e^x\,dx
=x^2e^x-2\int xe^x\,dx
=e^x(x^2-2x+2)+C.
$$

En el método tabular colocás las derivadas sucesivas del polinomio y las primitivas sucesivas del otro factor; combinás los productos diagonales con signos alternados $+,-,+,\ldots$.

### Cuando vuelve a aparecer la integral original

Sea $I=\int e^x\cos x\,dx$. Aplicando partes dos veces:

$$
I=e^x\cos x+\int e^x\sin x\,dx
=e^x\cos x+e^x\sin x-I.
$$

Despejás $I$:

$$
I=\frac{e^x}{2}(\sin x+\cos x)+C.
$$

### Partes en integrales definidas

$$
\int_a^b u(x)v'(x)\,dx
=[u(x)v(x)]_a^b-\int_a^b v(x)u'(x)\,dx.
$$

Por ejemplo, $\int_0^1 xe^x\,dx=[e^x(x-1)]_0^1=1$.

## 6. Cocientes de polinomios: fracciones parciales

Para $\int P(x)/Q(x)\,dx$:

1. Si $\deg P\ge\deg Q$, hacé primero división de polinomios.
2. Factorizá $Q$ sobre los reales.
3. Planteá la descomposición correspondiente.
4. Hallá las constantes igualando numeradores.
5. Integrá cada término.

| Factor del denominador | Términos que tenés que incluir |
|---|---|
| $x-a$ | $A/(x-a)$ |
| $(x-a)^m$ | $A_1/(x-a)+A_2/(x-a)^2+\cdots+A_m/(x-a)^m$ |
| Cuadrático irreducible $q(x)$ | $(Ax+B)/q(x)$ |
| $q(x)^m$ irreducible | $(A_1x+B_1)/q(x)+\cdots+(A_mx+B_m)/q(x)^m$ |

“Cuadrático irreducible” significa que no se factoriza en factores lineales reales: su discriminante es negativo.

### Ejemplo con factores lineales distintos

$$
\frac{3x+5}{x^2+3x+2}
=\frac{A}{x+1}+\frac{B}{x+2}.
$$

Como $3x+5=A(x+2)+B(x+1)$, resulta $A=2$, $B=1$.

$$
\int\frac{3x+5}{x^2+3x+2}\,dx
=2\ln|x+1|+\ln|x+2|+C.
$$

### Ejemplo con factor repetido

$$
\frac{1}{x(x+1)^2}
=\frac1x-\frac1{x+1}-\frac1{(x+1)^2}.
$$

$$
\int\frac{dx}{x(x+1)^2}
=\ln|x|-\ln|x+1|+\frac1{x+1}+C.
$$

### Cuadráticos: completar el cuadrado

$$
x^2+2x+5=(x+1)^2+4.
$$

Separá el numerador en una parte proporcional a la derivada del denominador y un resto:

$$
\int\frac{2x+3}{x^2+2x+5}\,dx
=\int\frac{2x+2}{x^2+2x+5}\,dx
+\int\frac{dx}{(x+1)^2+4}.
$$

$$
=\ln(x^2+2x+5)+\frac12\arctan\left(\frac{x+1}{2}\right)+C.
$$

## 7. Integrales trigonométricas

### Identidades que conviene tener a mano

$$
\sin^2x+\cos^2x=1,\qquad 1+\tan^2x=\sec^2x.
$$

$$
\sin^2x=\frac{1-\cos2x}{2},\qquad
\cos^2x=\frac{1+\cos2x}{2}.
$$

$$
\sin A\cos B=\frac12[\sin(A+B)+\sin(A-B)].
$$

$$
\cos A\cos B=\frac12[\cos(A-B)+\cos(A+B)].
$$

$$
\sin A\sin B=\frac12[\cos(A-B)-\cos(A+B)].
$$

### Potencias $\sin^m x\cos^n x$, con exponentes enteros no negativos

- Si $m$ es impar, separá un $\sin x$ y usá $u=\cos x$.
- Si $n$ es impar, separá un $\cos x$ y usá $u=\sin x$.
- Si ambos son pares, usá las identidades de ángulo doble.

**Ejemplo impar:**

$$
\int\sin^3x\cos^2x\,dx
=\int(1-\cos^2x)\cos^2x\sin x\,dx.
$$

Con $u=\cos x$, $du=-\sin x\,dx$:

$$
-\int(u^2-u^4)\,du
=-\frac{\cos^3x}{3}+\frac{\cos^5x}{5}+C.
$$

**Ejemplo par:**

$$
\int\sin^2x\,dx
=\frac12\int(1-\cos2x)\,dx
=\frac x2-\frac{\sin2x}{4}+C.
$$

### Potencias de tangente y secante

Para $\tan^m x\sec^n x$:

- Si $n\ge2$ es par, reservá $\sec^2x\,dx$ y usá $u=\tan x$.
- Si $m$ es impar y hay al menos un factor secante, reservá $\sec x\tan x\,dx$ y probá $u=\sec x$.
- En otros casos puede hacer falta reescribir con $\tan^2x=\sec^2x-1$ o integrar por partes.

$$
\int\tan^2x\sec^2x\,dx=\frac{\tan^3x}{3}+C.
$$

Para cotangente y cosecante se usan $1+\cot^2x=\csc^2x$, $(\cot x)'=-\csc^2x$ y $(\csc x)'=-\csc x\cot x$.

## 8. Sustitución trigonométrica para raíces

Con $a>0$:

| Raíz | Sustitución habitual | Identidad que la simplifica |
|---|---|---|
| $\sqrt{a^2-x^2}$ | $x=a\sin\theta$ | $1-\sin^2\theta=\cos^2\theta$ |
| $\sqrt{a^2+x^2}$ | $x=a\tan\theta$ | $1+\tan^2\theta=\sec^2\theta$ |
| $\sqrt{x^2-a^2}$ | $x=a\sec\theta$ | $\sec^2\theta-1=\tan^2\theta$ |

Elegí una rama de $\theta$ compatible con el intervalo. Recordá que $\sqrt{\cos^2\theta}=|\cos\theta|$, y lo mismo para otras funciones: el signo depende de la rama elegida.

**Ejemplo:** $\int\sqrt{9-x^2}\,dx$, para $-3<x<3$.

Tomá $x=3\sin\theta$, con $-\pi/2<\theta<\pi/2$. Entonces:

$$
dx=3\cos\theta\,d\theta,\qquad
\sqrt{9-x^2}=3\cos\theta.
$$

$$
\int\sqrt{9-x^2}\,dx
=9\int\cos^2\theta\,d\theta
=\frac92(\theta+\sin\theta\cos\theta)+C.
$$

Volviendo a $x$:

$$
\boxed{\frac x2\sqrt{9-x^2}+\frac92\arcsin(x/3)+C.}
$$

Si aparece $x^2+2x+5$, primero completá el cuadrado: $(x+1)^2+4$. El cambio se aplica a $x+1$.

## 9. Otros cambios útiles

### Eliminar raíces con potencias

Si aparece $\sqrt{x}$, probá $u=\sqrt{x}$, es decir $x=u^2$, $dx=2u\,du$.

$$
\int\frac{dx}{\sqrt{x}+1}
=\int\frac{2u}{u+1}\,du
=\int\left(2-\frac2{u+1}\right)du.
$$

$$
=2\sqrt{x}-2\ln(\sqrt{x}+1)+C,\qquad x>0.
$$

Si se mezclan raíces cuadradas y cúbicas de $x$, puede servir $x=t^6$: elegís un múltiplo común de los denominadores de los exponentes y respetás el dominio.

### Sustitución universal trigonométrica

Para una función racional de $\sin x$ y $\cos x$, puede servir:

$$
t=\tan(x/2),\qquad
\sin x=\frac{2t}{1+t^2},\qquad
\cos x=\frac{1-t^2}{1+t^2},\qquad
dx=\frac{2}{1+t^2}\,dt.
$$

Convierte el integrando en una función racional de $t$, que podés resolver con álgebra y fracciones parciales. Se aplica por intervalos donde el cambio esté definido; puede ser más largo que una identidad trigonométrica sencilla.

### Fórmula de reducción

Para familias con potencias altas, partes puede relacionar una integral con otra de menor exponente. Por ejemplo, para entero $n\ge2$:

$$
\int\sin^n x\,dx
=-\frac{\sin^{n-1}x\cos x}{n}
+\frac{n-1}{n}\int\sin^{n-2}x\,dx.
$$

Repetís hasta llegar a $\int1\,dx$ o $\int\sin x\,dx$.

## 10. Integrales definidas: simetría y área

Siempre que la integral exista:

- Si $f(-x)=f(x)$, es **par** y $\int_{-a}^a f(x)\,dx=2\int_0^a f(x)\,dx$.
- Si $f(-x)=-f(x)$, es **impar** y $\int_{-a}^a f(x)\,dx=0$.

Ejemplos:

$$
\int_{-2}^2 x^3\,dx=0,\qquad
\int_{-1}^1 x^2\,dx=2\int_0^1x^2\,dx=\frac23.
$$

La integral definida es **área con signo**. Para el área geométrica entre una curva y el eje:

$$
A=\int_a^b|f(x)|\,dx.
$$

Buscá los ceros y separá los tramos donde cambia de signo. Entre dos curvas:

$$
A=\int_a^b|f(x)-g(x)|\,dx.
$$

Por ejemplo, $\int_{-\pi}^{\pi}\sin x\,dx=0$, pero el área geométrica es $4$.

## 11. Integrales impropias

Son integrales con intervalo infinito o con una singularidad no acotada. Se definen mediante límites; solo tienen un valor finito cuando los límites correspondientes convergen.

### Extremo infinito

$$
\int_a^\infty f(x)\,dx
=\lim_{R\to\infty}\int_a^R f(x)\,dx.
$$

Ejemplo:

$$
\int_1^\infty\frac{dx}{x^2}
=\lim_{R\to\infty}\left(1-\frac1R\right)=1.
$$

### Singularidad en un extremo

$$
\int_0^1\frac{dx}{\sqrt{x}}
=\lim_{\varepsilon\to0^+}[2\sqrt{x}]_{\varepsilon}^1=2.
$$

### Singularidad dentro del intervalo

Si hay una singularidad en $c\in(a,b)$, separá en $\int_a^c f+\int_c^b f$. **Ambas integrales deben converger por separado.**

Por eso $\int_{-1}^1dx/x$ no converge como integral impropia, aunque al usar límites simétricos dé un valor principal de Cauchy igual a cero. No se puede aplicar la regla de función impar sin comprobar existencia.

### Criterios útiles

| Integral | Converge si… |
|---|---|
| $\int_1^\infty x^{-p}\,dx$ | $p>1$ |
| $\int_0^1 x^{-p}\,dx$ | $p<1$ |

Para funciones no negativas, la comparación con estas potencias ayuda: una función menor que otra integrable también es integrable; una función mayor que otra cuya integral diverge también diverge. La comparación debe valer cerca del punto problemático.

## 12. Cuando no aparece una primitiva elemental

Integrales como $\int e^{-x^2}\,dx$, $\int\sin(x)/x\,dx$ o $\int dx/\ln x$ no tienen primitivas elementales en sus intervalos de definición habituales. Pueden expresarse mediante funciones especiales o aproximarse numéricamente en un intervalo adecuado.

No encontrar una primitiva con sustitución o partes no demuestra que no exista una elemental. Para un ejercicio de métodos numéricos, usá el método que indique el enunciado y comprobá antes el dominio de integración.

## 13. Integración numérica: preparar los datos

Querés aproximar $I=\int_a^b f(x)\,dx$. Para las reglas compuestas con puntos equiespaciados:

$$
h=\frac{b-a}{n},\qquad x_i=a+ih,\qquad f_i=f(x_i).
$$

**$n$ es la cantidad de subintervalos. Hay $n+1$ puntos.**

Si tenés una tabla, comprobá que los $x_i$ estén ordenados y si son equiespaciados. Si hay singularidades, las reglas ordinarias y sus cotas de error pueden no ser aplicables directamente.

### 13.1. Trapecio simple y compuesto

Simple, usando solo los extremos:

$$
T=\frac{b-a}{2}[f(a)+f(b)].
$$

Compuesto:

$$
T_h=\frac h2\left[f_0+2\sum_{i=1}^{n-1}f_i+f_n\right].
$$

Con puntos no equiespaciados podés sumar trapecios de distintos anchos:

$$
T=\sum_{i=0}^{n-1}\frac{x_{i+1}-x_i}{2}(f_i+f_{i+1}).
$$

### 13.2. Simpson 1/3

Simple: **dos subintervalos y tres puntos equiespaciados**:

$$
S=\frac{b-a}{6}\left[f(a)+4f\left(\frac{a+b}{2}\right)+f(b)\right].
$$

Compuesto: $n$ tiene que ser **par**:

$$
S_h=\frac h3\left[
f_0+4\sum_{\substack{i=1\\i\text{ impar}}}^{n-1}f_i
+2\sum_{\substack{i=2\\i\text{ par}}}^{n-2}f_i+f_n\right].
$$

Pesos: $1,4,2,4,2,\ldots,4,1$.

### 13.3. Simpson 3/8

Simple: **tres subintervalos y cuatro puntos equiespaciados**:

$$
S_{3/8}=\frac{3h}{8}(f_0+3f_1+3f_2+f_3),\qquad h=(b-a)/3.
$$

Compuesto: $n$ tiene que ser **múltiplo de 3**:

$$
S_{3/8,h}=\frac{3h}{8}\left[
f_0+f_n+3\sum_{\substack{1\le i\le n-1\\3\nmid i}}f_i
+2\sum_{\substack{1\le i\le n-1\\3\mid i}}f_i\right].
$$

Si $n$ es impar y $n\ge3$, podés usar Simpson 3/8 en tres subintervalos y Simpson 1/3 en los restantes, que son una cantidad par. No dupliques el aporte de un subintervalo; el punto compartido es extremo de ambos bloques. Si el enunciado exige una regla específica, respetá esa elección.

### 13.4. Cuadratura de Gauss-Legendre

Primero transformás $[a,b]$ en $[-1,1]$:

$$
x=\frac{a+b}{2}+\frac{b-a}{2}t,\qquad dx=\frac{b-a}{2}\,dt.
$$

La regla de $m$ puntos es:

$$
I\approx\frac{b-a}{2}\sum_{j=1}^m w_j
f\left(\frac{a+b}{2}+\frac{b-a}{2}t_j\right).
$$

| Cantidad de puntos | Nodos $t_j$ | Pesos $w_j$ en el mismo orden |
|---|---|---|
| 2 | $-1/\sqrt3,\ 1/\sqrt3$ | $1,\ 1$ |
| 3 | $-\sqrt{3/5},\ 0,\ \sqrt{3/5}$ | $5/9,\ 8/9,\ 5/9$ |

Con $m$ puntos es exacta para polinomios de grado hasta $2m-1$ en aritmética exacta. Los nodos generalmente no coinciden con los de una tabla dada: necesitás poder evaluar la función o construir una aproximación de ella. Recordá el factor exterior $(b-a)/2$.

### 13.5. Datos no equiespaciados y splines

Opciones útiles:

- Sumar trapecios con el ancho real de cada intervalo.
- Construir un spline, generar una tabla equiespaciada y aplicar Simpson.
- Integrar directamente cada polinomio del spline.

Si en $[x_i,x_{i+1}]$ el spline es:

$$
S_i(x)=a_i+b_i(x-x_i)+c_i(x-x_i)^2+d_i(x-x_i)^3,
$$

con $h_i=x_{i+1}-x_i$, entonces:

$$
\int_{x_i}^{x_{i+1}}S_i(x)\,dx
=a_ih_i+\frac{b_ih_i^2}{2}+\frac{c_ih_i^3}{3}+\frac{d_ih_i^4}{4}.
$$

Sumás sobre todos los tramos. Eso integra exactamente el spline en aritmética exacta; la diferencia entre el spline y la función original sigue siendo una fuente de error.

### Ejemplo comparativo: $\int_0^1x^2\,dx$

El valor exacto es $1/3$. Con $n=2$, $h=0.5$ y valores $0,0.25,1$:

$$
T_h=\frac{0.5}{2}[0+2(0.25)+1]=0.375.
$$

$$
S_h=\frac{0.5}{3}[0+4(0.25)+1]=\frac13.
$$

Gauss de dos puntos también da $1/3$, porque integra exactamente polinomios de grado hasta 3.

## 14. Error y elección de la cantidad de subintervalos

En un intervalo fijo y para funciones suficientemente suaves:

| Método compuesto | Orden global | Derivada que controla una cota del error |
|---|---|---|
| Trapecio | $O(h^2)$ | $f''$ |
| Simpson 1/3 | $O(h^4)$ | $f^{(4)}$ |
| Simpson 3/8 | $O(h^4)$ | $f^{(4)}$ |

Para $a<b$, definí $M_2=\max_{[a,b]}|f''|$ y $M_4=\max_{[a,b]}|f^{(4)}|$. Con esas derivadas continuas:

$$
|I-T_h|\le\frac{b-a}{12}h^2M_2,
$$

$$
|I-S_{1/3,h}|\le\frac{b-a}{180}h^4M_4,
$$

$$
|I-S_{3/8,h}|\le\frac{b-a}{80}h^4M_4.
$$

Para garantizar una tolerancia $\varepsilon$ con trapecio:

$$
n\ge\sqrt{\frac{(b-a)^3M_2}{12\varepsilon}}.
$$

Para Simpson 1/3:

$$
n\ge\left(\frac{(b-a)^5M_4}{180\varepsilon}\right)^{1/4}.
$$

Redondeá hacia arriba y, para Simpson 1/3, subí al siguiente entero par si hace falta. Para Simpson 3/8 se usa 80 en lugar de 180 y se ajusta al siguiente múltiplo de 3. Respetá también el mínimo de subintervalos de cada regla.

### Estimación por refinamiento

Si calculás con $h$ y con $h/2$, para un método de orden $p$ en el régimen de convergencia:

$$
|I-I_{h/2}|\approx\frac{|I_{h/2}-I_h|}{2^p-1}.
$$

Usás $p=2$ para trapecio y $p=4$ para Simpson. Esta es una **estimación**, no una garantía general. La extrapolación de Richardson es:

$$
I_R=I_{h/2}+\frac{I_{h/2}-I_h}{2^p-1}.
$$

La integración adaptativa divide más los tramos donde estima mayor error y menos los tramos suaves. Es útil cuando la dificultad de la función cambia dentro del intervalo.

## 15. Cómo conectar esta guía con los códigos

- [integracion.cpp](CODIGO/integracion.cpp): programa interactivo con reglas numéricas y opciones de comparación y error.
- [Integracion Newton Cotes.cpp](PARA%20EDITAR/Integrar/Integracion%20Newton%20Cotes.cpp): funciones de trapecio y Simpson para editar. Su `main` está vacío: tenés que llamar a la rutina elegida y completar los datos si usás una tabla.
- [Gauss-Legendre.cpp](PARA%20EDITAR/Integrar/Gauss-Legendre.cpp): cambiás `f(x)` y ejecutás para cargar intervalo y cantidad de puntos.

En C++, $\ln x$ se escribe `log(x)`, $e^x$ se escribe `exp(x)`, $\sqrt{x}$ se escribe `sqrt(x)` y $x^2$ se escribe `x*x` o `pow(x, 2)`. `^` no calcula potencias en C++; el evaluador interactivo de `CODIGO/` sí lo acepta.

## 16. Errores frecuentes y cómo verificar

1. **Olvidar $C$** en una integral indefinida.
2. **Olvidar un factor de la cadena**, como $1/3$ al integrar $\cos3x$.
3. **Cambiar solo parte del integrando** en una sustitución: al terminar el cambio no deben quedar variables mezcladas.
4. **Confundir $u$ con $du$ o $dv$ con $v$** al integrar por partes.
5. **Usar $\ln x$ donde corresponde $\ln|x|$** sin justificar un dominio positivo.
6. **Aplicar $F(b)-F(a)$ atravesando una singularidad** sin analizar antes la integral impropia.
7. **Confundir integral con área geométrica** cuando la función cambia de signo.
8. **Usar Simpson con puntos no equiespaciados** o con una cantidad incompatible de subintervalos.
9. **Confundir puntos y subintervalos**: con 5 puntos hay 4 subintervalos.
10. **Olvidar el cambio de intervalo o el factor exterior en Gauss**.

Para verificar una primitiva, **derivala y comprobá que recuperás el integrando**. Para una integral definida, revisá signo, escala y dominio; si podés, compará con una aproximación numérica. Para una aproximación numérica, refiná el paso y comprobá si los resultados se estabilizan.
