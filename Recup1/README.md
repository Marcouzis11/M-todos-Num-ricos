# Recuperatorio 1 — empezá acá

Para abrir el menú en Linux, desde la raíz del repositorio:

```bash
bash Recup1/iniciar.sh
```

En Windows, abrí `iniciar.bat`. Compila con `g++` del PATH o con el configurado en `compilador.local.bat`. En este PC quedó configurado un compilador portátil, sin cambiar el PATH global. Si llevás la carpeta a otra PC con los once `.exe`, el menú permite usarlos sin compilador e informa que no recompila; para modificar código en esa PC necesitás C++11 o posterior. Se verificaron compilación y pruebas en Windows, además del soporte de los scripts Linux.

**Novedades para los parciales 2024–2026:** consultá `NOVEDADES.md`. Incluye ejemplos completos para pegar en consola, cocientes de Newton, relajación, doble corte, reordenamiento, errores verdaderos, gráficos offline y raíces de los ajustes.

El menú compila los programas y permite elegir el tema. En Linux recompila solo lo que cambió. **No tenés que modificar funciones ni matrices en el código:** en `CODIGO/` se ingresan por consola.

Cada ejercicio genera un bloque **TEXTO PARA COPIAR EN LA HOJA** con datos, fórmulas, desarrollo, tablas y resultados. Se guarda también en `salida_<nombre_del_programa>.txt`, **junto al ejecutable**, agregando cada ejercicio al final sin borrar los anteriores. Seleccioná ese bloque en la consola o abrí el archivo.

## Qué hay en cada carpeta

- **`PARAEDITAR/`**: los once `.cpp`, los dos `data.txt` y los apuntes de `Cursada2026Promocion/`, copiados sin cambios y con las mismas rutas. Conserva los problemas de los originales: es la copia para editar o consultar la cursada.
- **`CODIGO/`**: los mismos once `.cpp` y organización, con implementaciones nuevas para resolver desde la consola. La lógica y las fórmulas están en `CODIGO/comun/*.h`, compartidas para mantener los controles consistentes. Los apuntes originales se conservan como referencia.
- **`tests/`**: comprobaciones numéricas y de la interacción por consola.

No se copiaron ejecutables viejos ni la distribución de GeoGebra. Los programas se compilan para la computadora donde los vayas a usar. Para transportar el paquete, copiá **toda `Recup1/`**, incluidos `CODIGO/comun/` y los `data.txt`.

## Qué abrir según el enunciado

| Menú | Si te piden… | Programa en `CODIGO/` | Qué deja listo para copiar |
|---|---|---|---|
| 1 | Bisección / Bolzano | `LocalizacionRaices/Cerrados/Biseccion.cpp` | Signos, hipótesis de continuidad, tabla, cota, aproximación e iteraciones |
| 2 | Regula falsi / falsa posición | `LocalizacionRaices/Cerrados/RegulaFalsi.cpp` | Interpolación de la secante del intervalo, tabla y residuo |
| 3 | Newton-Raphson | `LocalizacionRaices/Abiertos/Newton-Raphson.cpp` | Fórmula, derivada, reemplazo inicial, tabla y resultado |
| 4 | Secante | `LocalizacionRaices/Abiertos/Secante.cpp` | Dos semillas, fórmula, tabla, errores y residuo |
| 5 | Punto fijo | `LocalizacionRaices/Abiertos/Punto-Fijo.cpp` | Iteración de g, tabla y diagnóstico local de g' |
| 6 | Triangular / resolver un sistema / determinante | `SistEcuacionesLineales/Eliminacion-Gaussiana.cpp` | Pivoteos, multiplicadores, matrices, retrosustitución, determinante y comprobación |
| 7 | Jacobi / Gauss-Seidel / relajación | `SistEcuacionesLineales/JacobiGaussSeidelRelax.cpp` | Dominancia, despejes, primer paso por componentes, tabla y comprobación |
| 8 | Lagrange / polinómica / diferencias divididas | `Interpolacion/interpolacion.cpp` | Bases o sistema o diferencias, polinomio expandido, control en nodos, consultas y tabla |
| 9 | Spline / trazador / cúbica normal | `Interpolacion/Spline.cpp` | Condiciones, sistema tridiagonal, coeficientes, polinomios por tramo, continuidad, consultas e integral |
| 10 | Recta de mínimos cuadrados | `Regresión/regresionLineal.cpp` | Sumatorias, ecuaciones normales, coeficientes, residuos, estadísticos y predicciones |
| 11 | Regresión polinómica u otro ajuste | `Regresión/polynomialRegression.cpp` | Grado elegido, ecuaciones normales, modelo, estadísticos y predicciones; también bases y linealizaciones |

## Cómo ingresar los datos

**Funciones y números.** Acepta `x^3-2*x+exp(-x)`, `ln(x)`, `log10(x)`, `sqrt(x)`, `abs(x)`, `sin(x)` o `sen(x)`, `cos(x)`, `tan(x)`, funciones inversas/hiperbólicas y constantes `pi`, `e`. `log` significa logaritmo natural. Los ángulos están en radianes.

Acepta `cbrt(x)` para raíz cúbica real, incluso si x es negativo. Podés usar `1e-6`, `10^-6`, `pi/2`, decimales con punto o coma, `2x` y `3(x+1)`. Es preferible escribir `*` para que la expresión sea clara. `-x^2` significa `-(x^2)`; si querés el cuadrado de un negativo escribí `(-x)^2`. **No uses `**` ni `pow(x,2)`: escribí `x^2`.**

**Tablas.** Elegí `1` y pegá los x en una línea y los y en otra:

```text
0 1 2
1 4 9
```

Los valores se separan con espacios, `;` o `|`; la coma se reserva para decimales. En una lista, cada cuenta tiene que ir sin espacios internos: `pi/2 1+2 4`. Los programas de interpolación y splines ordenan los pares por x y rechazan x repetidos. Regresión permite x repetidos si alcanzan los datos independientes para el modelo.

**Sistemas.** Pegá una fila por línea con los coeficientes y el término independiente al final. Para un sistema de dos incógnitas:

```text
0 2 4
1 3 7
```

Eso representa `2*x2=4`, `x1+3*x2=7`. La solución es `x1=1`, `x2=2`.

**Archivos.** En sistemas y tablas también podés elegir `2`. ENTER usa `data.txt` junto al ejecutable; o pegá una ruta sin comillas. Un archivo de puntos tiene dos columnas `x y`; uno de sistemas, n filas con n+1 columnas. Se aceptan líneas vacías, comentarios con `#`, CRLF y última línea sin salto. Los `data.txt` incluidos son ejemplos de la cursada: reemplazalos si elegís esa opción para otro ejercicio.

ENTER toma el valor entre corchetes. `salir` o el fin de entrada cierran el programa. Un dato mal formado se vuelve a pedir; un problema matemático informa el motivo y vuelve al menú general.

## Decisiones que tenés que tomar según el enunciado

**Raíces.** Elegí la función, semillas/intervalo y criterio de error. Podés cortar por tolerancia o pedir N iteraciones. El programa distingue “resultado con criterio alcanzado” de “última aproximación sin convergencia certificada”. No confunde residuo con error verdadero.

- El error absoluto aproximado es `|x_n-x_(n-1)|`.
- El relativo porcentual divide por `|x_n|` y multiplica por 100.
- El IUA divide por `|x_n+x_(n-1)|/2` y multiplica por 100.
- Si elegís porcentaje, una tolerancia de `0.1` significa **0.1 %**.
- Bisección permite cortar por `(b-a)/2`, calculado en el intervalo antes de obtener el punto medio. La primera diferencia entre iterados se deja sin valor por defecto. Si el enunciado fija una aproximación previa, ingresala en Opciones adicionales; se usa también en el primer error IUA.
- En Newton, ingresá la derivada exacta si te la exigen. ENTER usa una aproximación por diferencia central, identificada en la salida.
- En punto fijo ingresá **g(x) ya despejada**. En Opciones adicionales podés dar lambda; se itera `(1-lambda)*x+lambda*g(x)` y el residuo sigue siendo el de `g(x)-x`. Se controla `g(x)-x`. El valor local de `|g'|` se informa como diagnóstico; la condición de contracción en todo un intervalo requiere justificarla.
- Bolzano requiere continuidad: el cambio de signo solo no demuestra que no haya una discontinuidad dentro del intervalo.

**Sistemas iterativos.** Elegí Jacobi, Seidel o SOR; el vector inicial; y el criterio de corte. Se puede pedir norma infinito, norma euclídea, relativo porcentual, IUA vectorial o residuo. El IUA vectorial usado se imprime explícitamente. SOR solicita omega entre 0 y 2. El orden original se conserva por defecto. En **Opciones adicionales** podés indicar una permutación manual o buscar automáticamente una que deje dominancia estricta por filas. Se informa qué filas se movieron; no se permutan variables ni el vector inicial. Una diagonal nula exige reordenar. Los criterios 6 y 7 exigen paso y residuo simultáneamente en norma infinito o euclídea; 8 y 9 comparan contra un vector de referencia. Este vector puede ingresarse o calcularse por Gauss, identificado como referencia numérica. La falta de dominancia estricta no se confunde con divergencia.

**Interpolación.** Seleccioná solo los nodos que pide el enunciado: con n puntos se construye un polinomio de grado máximo n-1. Si dan f(x), podés calcular el error verdadero en cada consulta. Si dan una cota M de la derivada de orden n, podés ingresarla para el resto teórico `M/n! * |producto(x-x_i)|`. Esa cota debe valer en todo el intervalo que contiene los nodos y la consulta. La evaluación usa la forma baricéntrica del mismo polinomio para reducir pérdida de precisión; el polinomio expandido también se muestra y se comprueba.

**Splines.** Elegí lineal, cuadrático, cúbico natural o cúbico sujeto. El natural impone segundas derivadas nulas en los extremos; el sujeto solicita las pendientes. El cuadrático necesita una condición adicional: podés dar `c_0`; ENTER pone `c_0=0` (primer tramo recto). Los tramos se numeran desde **0**, igual que los nodos. Se muestran polinomios en `(x-x_i)` y expandidos. El último nodo pertenece al último tramo. Fuera del dominio se rechaza la consulta.

El menú del spline permite evaluar, generar tablas equiespaciadas y obtener integrales por tramos en un subintervalo del dominio. Para conservar la cantidad de puntos, usá `número de puntos - 1` subintervalos (es el valor ofrecido por defecto). Si elegís un paso h, debe dividir exactamente el intervalo; el programa no inventa un último paso diferente. Las derivadas en un nodo de un spline lineal/cuadrático requieren revisar los límites laterales que aparecen en el control de continuidad.

**Regresión.** La forma es `y=a0+a1*x+...` (coeficientes en orden de potencia creciente). Se muestran sumatorias y desarrollo de las ecuaciones normales por Gauss; el resultado numérico se obtiene con QR y variable centrada/escalada para mejorar la precisión. Para valores de x grandes, la forma en la variable t que imprime el programa conserva mejor la precisión.

- `Sr` es la suma de residuos cuadrados; `St`, la suma de cuadrados respecto de la media.
- `ECM=Sr/n`; `RMSE=sqrt(Sr/n)`; error estándar `sqrt(Sr/(n-m))`, con m coeficientes. El original llamaba ECM a una cantidad con raíz: acá se muestran ambas con su fórmula.
- `R²=1-Sr/St`, sin valor absoluto. Si St=0, se informa que no está definido.
- Para la recta se muestra **r de Pearson con signo**, también cuando la pendiente es negativa. Para polinomios de mayor grado se identifica el índice de correlación múltiple por separado.
- Si n=m no hay grados de libertad para el error estándar; se informa en vez de dividir por cero.
- El programa polinómico también acepta funciones de base ingresadas por teclado, y modelos exponencial, potencial, logarítmico y de saturación. Explica la transformación y qué residuos minimiza. Linealizar `ln(y)` o `1/y` cambia el objetivo respecto de minimizar errores en y.

## Ejemplo completo para empezar

Ejecutá el menú, elegí `1` (bisección) e ingresá, una respuesta por línea:

```text
x^2-2
0
2
5
1
1e-6
100
sqrt(2)
0
```

Corresponde a f(x), a, b, criterio de cota, corte por tolerancia, tolerancia, máximo de iteraciones y raíz de referencia. El bloque final muestra `x ≈ 1.414213`, su cota, la tabla y el error contra `sqrt(2)`.

En el menú `8`, para los puntos `(0,1)`, `(1,4)`, `(2,9)`: elegí carga por listas, ingresá `0 1 2` y `1 4 9`, elegí Lagrange, dejá vacías f y M si no están dadas, y usá la opción de evaluar en `0.5`. Debe dar **2.25**.

## Compilar y comprobar

```bash
bash Recup1/CODIGO/compilar.sh
python3 Recup1/tests/verificar.py
```

Para recompilar todo sin mirar las fechas:

```bash
bash Recup1/CODIGO/compilar.sh --forzar
```

También podés abrir **un solo `.cpp`** en Code::Blocks/Dev-C++ o compilarlo con `g++ -std=c++11`. Conservá las carpetas para que encuentre `comun/`; no agregues los once `.cpp` a un mismo ejecutable, porque cada uno tiene su `main`.

Límites: sistemas de hasta 20 incógnitas, interpolación polinómica de hasta 20 nodos, tablas/splines/ajustes de hasta 100 puntos, polinomios de regresión de grado hasta 10 y tablas generadas de hasta 1000 subintervalos. Las tolerancias usan `<` por defecto; Opciones adicionales permite elegir `<=`. Los resúmenes de raíces y sistemas incluyen diez decimales y los errores conservan notación significativa. Todos usan `double`, controles de dominio y límites de iteraciones. Los avisos de mal condicionamiento o falta de convergencia forman parte del resultado y no deben omitirse al interpretarlo.


### Pruebas en Windows

Con `g++` disponible en PATH:

```powershell
Recup1\CODIGO\compilar.bat
python Recup1\tests\verificar.py
```

Si el compilador está configurado solo en `compilador.local.bat`, abrí cmd y ejecutá `call Recup1\compilador.local.bat` antes de Python. También se puede definir `CXX` con la ruta absoluta de g++ para las pruebas. Los archivos CSV y SVG generados se guardan junto al ejecutable, con nombres consecutivos para conservar los anteriores.
