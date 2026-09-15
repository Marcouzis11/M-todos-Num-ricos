# Tabla de derivadas

Guía de consulta con las reglas y derivadas más usadas. En todas las fórmulas,
$u=u(x)$ y $v=v(x)$ son funciones derivables, $c$ es una constante y $u'$ significa
$\dfrac{du}{dx}$.

> **Cuidado con la notación:** $\operatorname{sen}^{-1}(x)$ suele significar
> $\operatorname{arcsen}(x)$, es decir, la **función inversa** del seno. No significa
> $1/\operatorname{sen}(x)$; esa función es la cosecante: $\operatorname{cosec}(x)$.

---

## 1. Definición de derivada

$$
f'(x)=\lim_{h\to 0}\frac{f(x+h)-f(x)}{h}
$$

Otras notaciones equivalentes:

$$
f'(x)=y'=\frac{dy}{dx}=D_xf(x)
$$

---

## 2. Reglas básicas

| Función | Derivada |
|---|---|
| $c$ | $0$ |
| $x$ | $1$ |
| $x^n$ | $n x^{n-1}$ |
| $c\,u$ | $c\,u'$ |
| $u\pm v$ | $u'\pm v'$ |
| $u\,v$ | $u'v+uv'$ |
| $\dfrac{u}{v}$ | $\dfrac{u'v-uv'}{v^2}$ |
| $f(u)$ | $f'(u)\,u'$ |

La última fila es la **regla de la cadena**. Por ejemplo:

$$
\frac{d}{dx}\operatorname{sen}(x^2)=\cos(x^2)\,2x
$$

### Potencias y raíces

| Función | Derivada |
|---|---|
| $u^n$ | $n u^{n-1}u'$ |
| $\sqrt{u}=u^{1/2}$ | $\dfrac{u'}{2\sqrt{u}}$ |
| $\sqrt[n]{u}=u^{1/n}$ | $\dfrac{u'}{n\,u^{(n-1)/n}}$ |
| $\dfrac{1}{u}=u^{-1}$ | $-\dfrac{u'}{u^2}$ |
| $u^v$ | $u^v\left(v'\ln u+v\dfrac{u'}{u}\right)$, si $u>0$ |

Casos conocidos de la última fórmula:

$$
\frac{d}{dx}x^x=x^x(\ln x+1), \qquad x>0
$$

$$
\frac{d}{dx}[f(x)]^{g(x)}=[f(x)]^{g(x)}
\left(g'(x)\ln f(x)+g(x)\frac{f'(x)}{f(x)}\right)
$$

---

## 3. Exponenciales y logaritmos

| Función | Derivada |
|---|---|
| $e^x$ | $e^x$ |
| $e^u$ | $e^u u'$ |
| $a^x$ | $a^x\ln a$ |
| $a^u$ | $a^u\ln(a)\,u'$ |
| $\ln x$ | $\dfrac{1}{x}$ |
| $\ln u$ | $\dfrac{u'}{u}$ |
| $\ln\lvert u\rvert$ | $\dfrac{u'}{u}$, si $u\ne 0$ |
| $\log_a x$ | $\dfrac{1}{x\ln a}$ |
| $\log_a u$ | $\dfrac{u'}{u\ln a}$ |

Para los logaritmos de base $a$ se requiere $a>0$ y $a\ne1$.

---

## 4. Funciones trigonométricas

Los ángulos se consideran expresados en **radianes**.

| Función | Derivada |
|---|---|
| $\operatorname{sen}u$ | $\cos u\,u'$ |
| $\cos u$ | $-\operatorname{sen}u\,u'$ |
| $\tan u$ | $\sec^2 u\,u'$ |
| $\cot u$ | $-\operatorname{cosec}^2u\,u'$ |
| $\sec u$ | $\sec u\tan u\,u'$ |
| $\operatorname{cosec}u$ | $-\operatorname{cosec}u\cot u\,u'$ |

### Formas equivalentes útiles

$$
\frac{d}{dx}\tan u=\frac{u'}{\cos^2u}
$$

$$
\frac{d}{dx}\cot u=-\frac{u'}{\operatorname{sen}^2u}
$$

---

## 5. Funciones trigonométricas inversas

También se escriben $\operatorname{sen}^{-1}$, $\cos^{-1}$, $\tan^{-1}$, etc. El superíndice
$-1$ indica la **función inversa**, no una potencia negativa.

| Función | Derivada |
|---|---|
| $\operatorname{arcsen}u$ | $\dfrac{u'}{\sqrt{1-u^2}}$ |
| $\arccos u$ | $-\dfrac{u'}{\sqrt{1-u^2}}$ |
| $\arctan u$ | $\dfrac{u'}{1+u^2}$ |
| $\operatorname{arccot}u$ | $-\dfrac{u'}{1+u^2}$ |
| $\operatorname{arcsec}u$ | $\dfrac{u'}{\lvert u\rvert\sqrt{u^2-1}}$ |
| $\operatorname{arccosec}u$ | $-\dfrac{u'}{\lvert u\rvert\sqrt{u^2-1}}$ |

En las dos primeras fórmulas se necesita $|u|<1$ para que la derivada real sea
finita. Para arcosecante y arcocosecante, $|u|>1$.

> Según el libro, la rama elegida para $\operatorname{arccot}$ puede variar. La
> convención de la tabla, cuya imagen está en $(0,\pi)$, produce la derivada negativa.

---

## 6. Funciones hiperbólicas

Definiciones principales:

$$
\operatorname{senh}x=\frac{e^x-e^{-x}}{2},
\qquad
\cosh x=\frac{e^x+e^{-x}}{2},
\qquad
\tanh x=\frac{\operatorname{senh}x}{\cosh x}
$$

| Función | Derivada |
|---|---|
| $\operatorname{senh}u$ | $\cosh u\,u'$ |
| $\cosh u$ | $\operatorname{senh}u\,u'$ |
| $\tanh u$ | $\operatorname{sech}^2u\,u'$ |
| $\coth u$ | $-\operatorname{cosech}^2u\,u'$ |
| $\operatorname{sech}u$ | $-\operatorname{sech}u\tanh u\,u'$ |
| $\operatorname{cosech}u$ | $-\operatorname{cosech}u\coth u\,u'$ |

---

## 7. Funciones hiperbólicas inversas

| Función | Derivada | Condición real usual |
|---|---|---|
| $\operatorname{arsenh}u$ | $\dfrac{u'}{\sqrt{1+u^2}}$ | $u\in\mathbb{R}$ |
| $\operatorname{arcosh}u$ | $\dfrac{u'}{\sqrt{u-1}\sqrt{u+1}}$ | $u>1$ |
| $\operatorname{artanh}u$ | $\dfrac{u'}{1-u^2}$ | $\lvert u\rvert<1$ |
| $\operatorname{arcoth}u$ | $\dfrac{u'}{1-u^2}$ | $\lvert u\rvert>1$ |
| $\operatorname{arsech}u$ | $-\dfrac{u'}{u\sqrt{1-u^2}}$ | $0<u<1$ |
| $\operatorname{arcosech}u$ | $-\dfrac{u'}{\lvert u\rvert\sqrt{1+u^2}}$ | $u\ne0$ |

También pueden aparecer con los nombres ingleses $\operatorname{asinh}$,
$\operatorname{acosh}$ y $\operatorname{atanh}$.

---

## 8. Valor absoluto y funciones definidas por partes

$$
\frac{d}{dx}|u|=\frac{u}{|u|}\,u', \qquad u\ne0
$$

En un punto donde $u=0$ hay que estudiar las derivadas laterales; la derivada
puede no existir. Por ejemplo, $|x|$ no es derivable en $x=0$.

Para una función definida por partes, se deriva cada expresión dentro de su
intervalo. En los puntos de unión hay que verificar:

1. Que la función sea continua.
2. Que existan las derivadas laterales.
3. Que ambas derivadas laterales sean iguales.

---

## 9. Derivación implícita

Si $y$ no está despejada y una ecuación relaciona $x$ con $y$, se derivan ambos
lados respecto de $x$, recordando que $y=y(x)$.

Ejemplo:

$$
x^2+y^2=R^2
$$

$$
2x+2y\,y'=0
\quad\Longrightarrow\quad
y'=-\frac{x}{y}
$$

En general, si $F(x,y)=0$:

$$
\frac{dy}{dx}=-\frac{F_x}{F_y}, \qquad F_y\ne0
$$

---

## 10. Derivación logarítmica

Es útil cuando hay productos, cocientes o potencias complicadas. Si $y>0$, se
toma logaritmo natural en ambos lados y después se deriva.

Ejemplo para $y=u^v$:

$$
\ln y=v\ln u
$$

$$
\frac{y'}{y}=v'\ln u+v\frac{u'}{u}
$$

$$
y'=u^v\left(v'\ln u+v\frac{u'}{u}\right)
$$

---

## 11. Función inversa

Si $f$ es invertible y $f'(f^{-1}(x))\ne0$:

$$
\left(f^{-1}\right)'(x)=\frac{1}{f'\left(f^{-1}(x)\right)}
$$

Esta regla permite deducir, por ejemplo, las derivadas de $\operatorname{arcsen}$,
$\arctan$ y las demás funciones inversas.

---

## 12. Curvas paramétricas

Si $x=x(t)$ e $y=y(t)$:

$$
\frac{dy}{dx}=\frac{dy/dt}{dx/dt}=\frac{y'(t)}{x'(t)},
\qquad x'(t)\ne0
$$

La segunda derivada es:

$$
\frac{d^2y}{dx^2}
=\frac{\dfrac{d}{dt}\left(\dfrac{dy}{dx}\right)}{dx/dt}
$$

---

## 13. Derivadas de orden superior

$$
f''(x)=\frac{d}{dx}f'(x),
\qquad
f^{(n)}(x)=\frac{d^n f}{dx^n}
$$

Algunas fórmulas frecuentes:

| Función | Derivada de orden $n$ |
|---|---|
| $e^{ax}$ | $a^n e^{ax}$ |
| $\operatorname{sen}(ax)$ | $a^n\operatorname{sen}\left(ax+\dfrac{n\pi}{2}\right)$ |
| $\cos(ax)$ | $a^n\cos\left(ax+\dfrac{n\pi}{2}\right)$ |
| $x^m$ | $\dfrac{m!}{(m-n)!}x^{m-n}$, si $0\le n\le m$ |

Para un producto, la regla de Leibniz es:

$$
(uv)^{(n)}=\sum_{k=0}^{n}\binom{n}{k}u^{(k)}v^{(n-k)}
$$

---

## 14. Ejemplos combinados

### Regla de la cadena

$$
f(x)=\sqrt{1+x^2}
\quad\Longrightarrow\quad
f'(x)=\frac{x}{\sqrt{1+x^2}}
$$

### Producto

$$
f(x)=x^2e^x
\quad\Longrightarrow\quad
f'(x)=2xe^x+x^2e^x=e^x(x^2+2x)
$$

### Cociente

$$
f(x)=\frac{\operatorname{sen}x}{x}
\quad\Longrightarrow\quad
f'(x)=\frac{x\cos x-\operatorname{sen}x}{x^2}
$$

### Trigonométrica inversa compuesta

$$
f(x)=\arctan(3x^2)
\quad\Longrightarrow\quad
f'(x)=\frac{6x}{1+9x^4}
$$

### Logaritmo compuesto

$$
f(x)=\ln(x^2+1)
\quad\Longrightarrow\quad
f'(x)=\frac{2x}{x^2+1}
$$

---

## 15. Errores frecuentes

- Olvidar multiplicar por $u'$ al aplicar la regla de la cadena.
- Confundir $\operatorname{sen}^{-1}x=\operatorname{arcsen}x$ con
  $(\operatorname{sen}x)^{-1}=1/\operatorname{sen}x$.
- Derivar un producto como $u'v'$; la regla correcta es $u'v+uv'$.
- Derivar un cociente como $u'/v'$; se usa
  $\dfrac{u'v-uv'}{v^2}$.
- Omitir el valor absoluto en las derivadas de $\operatorname{arcsec}$ y
  $\operatorname{arccosec}$.
- Usar las fórmulas trigonométricas con ángulos en grados: las derivadas
  estándar suponen radianes.
- Aplicar una fórmula sin revisar primero el dominio de la función y de su
  derivada.
