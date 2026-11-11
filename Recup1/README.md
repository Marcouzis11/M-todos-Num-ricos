# Recuperatorio 1 — empezá acá

Para abrir el menú en Linux, desde la raíz del repositorio:

```bash
bash Recup1/iniciar.sh
```

En Windows, abrí `iniciar.bat`. Compila con `g++` del PATH o con el configurado en `compilador.local.bat`. En este PC quedó configurado un compilador portátil, sin cambiar el PATH global. Para llevarlo a otra PC sin compilador, incluí los once ejecutables de métodos y el nuevo `CODIGO/menu.exe`, compilados para Windows. El menú informa si usa los ejecutables existentes sin recompilar; para modificar código necesitás C++11 o posterior. Los scripts incluyen soporte para Windows y Linux.

**Novedades para los parciales 2024–2026:** consultá `NOVEDADES.md`. Incluye ejemplos completos para pegar en consola, cocientes de Newton, relajación, doble corte, reordenamiento, errores verdaderos, gráficos offline y raíces de los ajustes.

El menú compila los programas y permite elegir el tema. En Linux recompila solo lo que cambió. **No tenés que modificar funciones ni matrices en el código:** en `CODIGO/` se ingresan por consola.

**Cargá una sola vez y cambiá de método.** El menú conserva una matriz para sistemas y una tabla de puntos para interpolación, splines y regresión durante toda la sesión, incluso al volver al menú principal:

- Entrá por **6 o 7**, cargá la matriz y elegí Gauss, Jacobi, Gauss-Seidel o SOR. Al terminar volvés a elegir otro método usando la misma matriz. **5** carga otra matriz y **6** muestra la actual.
- Entrá por **8, 9, 10 u 11**, cargá la tabla y elegí el método: Lagrange, Vandermonde, Newton, un spline o un ajuste. En las consultas de cada método, **0** vuelve a la lista de métodos con la misma tabla. **15** carga otra tabla y **16** muestra la actual.
- En ambos menús, **0** vuelve al principal conservando los datos. Los parámetros propios de cada método, como tolerancias, vector inicial, grado o pendientes de borde, se piden al elegirlo. Cerrar el programa finaliza la sesión.

Los cálculos trabajan sobre copias: triangular o reordenar un sistema, ordenar los puntos o transformar un ajuste conserva los datos originales. Si un método falla, podés elegir otro con esos mismos datos. Una carga nueva fallida también conserva la matriz o tabla anterior.

Cada ejercicio genera un bloque **TEXTO PARA COPIAR EN LA HOJA** con datos, fórmulas, desarrollo, tablas y resultados. Al usar el menú común se guarda en **`CODIGO/salida_menu.txt`**, agregando cada cálculo sin borrar los anteriores. Si ejecutás un método individual, se sigue guardando en `salida_<nombre_del_programa>.txt`, junto a su ejecutable. Seleccioná ese bloque en la consola o abrí el archivo.

La consola de los once programas usa títulos por método, opciones en líneas separadas y tablas con bordes, encabezados completos y números alineados a la derecha. Las matrices identifican las filas, las columnas de coeficientes y el término independiente `b`. El detalle de los pasos aparece separado de la tabla de iteraciones y del resumen final.

Las tablas anchas se dividen en bloques que repiten el índice de fila, sin omitir columnas ni iteraciones. Conservan las 15 cifras significativas del informe; `--` indica un dato no disponible o no aplicable. El formato también se conserva en los archivos de salida, sin códigos de color. En Linux se detecta el ancho de la terminal; en ausencia de ese dato se usan 100 columnas. Podés fijarlo entre 40 y 240 columnas:

```bash
RECUP_ANCHO=80 bash Recup1/iniciar.sh
```

En Windows, desde `cmd`: `set RECUP_ANCHO=80` antes de abrir `iniciar.bat`. El formato de las tablas no cambia la precisión de los cálculos ni los archivos CSV para exportar datos.

## Qué hay en cada carpeta

- **`PARAEDITAR/`**: los once `.cpp`, los dos `data.txt` y los apuntes de `Cursada2026Promocion/`, copiados sin cambios y con las mismas rutas. Conserva los problemas de los originales: es la copia para editar o consultar la cursada.
- **`CODIGO/`**: los mismos once `.cpp` y organización, con implementaciones nuevas para resolver desde la consola, más `menu.cpp` para cambiar de método conservando los datos. La lógica y las fórmulas están en `CODIGO/comun/*.h`, compartidas para mantener los controles consistentes. Los apuntes originales se conservan como referencia.
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

La reutilización de datos está disponible al abrir `iniciar.sh`, `iniciar.bat` o el ejecutable `CODIGO/menu`. Los once programas individuales permiten resolver cada tema por separado con las respuestas actualizadas de `ejemplos/`. Jacobi, Gauss-Seidel y SOR comparten el mismo menú de errores tanto desde el menú común como desde su ejecutable individual. Desde el menú común, ENTER al pedir un archivo busca `CODIGO/SistEcuacionesLineales/data.txt` para sistemas o `CODIGO/Interpolacion/data.txt` para la tabla.

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

**Sistemas iterativos.** Elegí Jacobi, Seidel o SOR, el vector inicial y el tipo de error. El menú común para los tres métodos ofrece:

1. **Absoluto estimado:** mayor diferencia absoluta entre componentes de dos iteraciones consecutivas.
2. **Absoluto exacto o verdadero:** mayor diferencia absoluta respecto de la solución de referencia.
3. **Relativo porcentual estimado:** `100*||x_nuevo-x_anterior||inf/||x_nuevo||inf`.
4. **Relativo porcentual exacto o verdadero:** `100*||x-referencia||inf/||referencia||inf`.
5. **Porcentual IUA estimado:** usa como denominador `||x_nuevo+x_anterior||inf/2`.
6. **Criterios avanzados:** absoluto estimado euclídeo, residuo, corte simultáneo de estimado y residuo (máximos o euclídeos) y absoluto verdadero euclídeo.

Los errores verdaderos piden directamente la solución de referencia, una componente por incógnita. También podés escribir `gauss` para obtener una referencia numérica, identificada como sujeta al redondeo. Si la referencia es el vector nulo, elegí absoluto verdadero: el relativo porcentual verdadero tiene denominador cero. Para los porcentajes, `0.1` significa `0.1 %`; el informe siempre imprime la fórmula usada. Los porcentajes vectoriales usan cocientes de normas, no el máximo de los porcentajes individuales.

SOR solicita omega entre 0 y 2. El orden original se conserva por defecto. En **Opciones adicionales** podés indicar una permutación manual o buscar automáticamente dominancia estricta por filas, incorporar una referencia opcional para comparar resultados y elegir `<` o `<=`. Si ya ingresaste una referencia para el error verdadero, no se vuelve a pedir. Se informa qué filas se movieron; no se permutan variables ni el vector inicial. Una diagonal nula exige reordenar. La falta de dominancia estricta no se confunde con divergencia.

Jacobi también necesita `X^(0)`: cada nueva componente se calcula con los valores del vector anterior. Si el enunciado dice `X0j=1 (j=1,...,6)`, ingresá `1 1 1 1 1 1`. La consola muestra un ejemplo con la cantidad de componentes del sistema y el informe registra el vector que usaste.

**Pivoteo en matrices.** Todo desarrollo por Gauss, incluidos Vandermonde, ecuaciones normales de regresión y referencias por Gauss de los iterativos, identifica su **pivoteo parcial escalado**. Por columna informa la fila elegida, el pivote candidato, su escala y el cociente `|candidato|/escala`. Cada intercambio anuncia las filas, muestra la matriz resultante y contabiliza el cambio de signo del determinante. También informa si no hubo intercambios. Jacobi, Gauss-Seidel y SOR indican que no aplican pivoteo parcial durante sus iteraciones; el sistema tridiagonal del spline se resuelve por Thomas sin pivoteo parcial.

**Interpolación.** Seleccioná solo los nodos que pide el enunciado: con n puntos se construye un polinomio de grado máximo n-1. Si dan f(x), podés calcular el error verdadero en cada consulta. Si dan una cota M de la derivada de orden n, podés ingresarla para el resto teórico `M/n! * |producto(x-x_i)|`. Esa cota debe valer en todo el intervalo que contiene los nodos y la consulta. La evaluación usa la forma baricéntrica del mismo polinomio para reducir pérdida de precisión; el polinomio expandido también se muestra y se comprueba.

**Splines.** Elegí lineal, cuadrático, cúbico natural o cúbico sujeto. El natural impone segundas derivadas nulas en los extremos; el sujeto solicita las pendientes. El cuadrático necesita una condición adicional: podés dar `c_0`; ENTER pone `c_0=0` (primer tramo recto). Los tramos se numeran desde **0**, igual que los nodos. Se muestran polinomios en `(x-x_i)` y expandidos. El último nodo pertenece al último tramo. Fuera del dominio se rechaza la consulta.

El menú del spline permite evaluar, generar tablas equiespaciadas y obtener integrales por tramos en un subintervalo del dominio. Para conservar la cantidad de puntos, usá `número de puntos - 1` subintervalos (es el valor ofrecido por defecto). Si elegís un paso h, debe dividir exactamente el intervalo; el programa no inventa un último paso diferente. Las derivadas en un nodo de un spline lineal/cuadrático requieren revisar los límites laterales que aparecen en el control de continuidad.

**Regresión.** La forma es `y=a0+a1*x+...` (coeficientes en orden de potencia creciente). Se muestran sumatorias y desarrollo de las ecuaciones normales por Gauss; el resultado numérico se obtiene con QR y variable centrada/escalada para mejorar la precisión. Para valores de x grandes, la forma en la variable t que imprime el programa conserva mejor la precisión.

En la regresión lineal se destaca **`f(x)=a*x+b`**, con `a` como pendiente y `b` como ordenada al origen (`a=c1`, `b=c0`). Se muestra cómo las derivadas parciales de `S(a,b)=suma[y_i-(a*x_i+b)]^2` llevan a las ecuaciones normales, se reemplazan sus sumatorias y se arma la matriz con incógnitas en orden `[a,b]` para resolverla por Gauss. El archivo `ejemplos/regresion_lineal_cursada.txt` permite reproducir los puntos del código original. En los demás ajustes también se desarrolla una ecuación normal por cada coeficiente.

Para la recta se numeran los puntos de `i=0` a `n`: la cantidad de datos es **`n+1 = Σ1`**. La matriz de coeficientes es `[[Σx_i², Σx_i], [Σx_i, n+1]]`; el vector independiente es `[Σ(x_i*y_i), Σy_i]`. Los indicadores de la recta usan esa misma convención: `ECM=Sr/(n+1)` y error estándar `sqrt(Sr/(n+1-m))`, con `m=2`. En los demás modelos, `n` sigue representando la cantidad de datos.

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

En el menú `8`, para los puntos `(0,1)`, `(1,4)`, `(2,9)`: elegí carga por listas, ingresá `0 1 2` y `1 4 9`, elegí **1 (Lagrange)**, dejá vacías f y M si no están dadas, y usá la opción de evaluar en `0.5`. Debe dar **2.25**. Elegí **0** en las consultas y luego **3 (Newton)** para usar los mismos puntos; o **8 (regresión lineal)** para ajustar una recta sin volver a cargarlos.

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
