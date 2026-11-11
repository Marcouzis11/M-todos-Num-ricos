# Recup1 actualizado

Los cambios están aplicados en `C:\Users\marco\OneDrive\Documentos\GitHub\M-todos-Num-ricos\Recup1`. Abrí `iniciar.bat` para usar el menú. Los once programas están compilados para Windows; no necesitan Internet para resolver los ejercicios ni para generar gráficos.

## Qué se agregó

| Tema | Funciones nuevas |
|---|---|
| Newton | Cociente `C_k`, error verdadero por iteración cuando se da referencia y estimación del orden de convergencia mediante los pasos. **C3 aparece al calcular x4:** pedir cuatro iteraciones. |
| Raíces | Aproximación previa opcional para el primer error de bisección/falsa posición; comparación `<` o `<=`; condición adicional de residuo; resultado a diez decimales; gráfico SVG y CSV. |
| Punto fijo | Lambda explícito para `(1-lambda)*x+lambda*g(x)`, derivada de la iteración relajada y residuo de la ecuación original `g(x)-x`. También se acepta `cbrt(x)` para raíz cúbica real. |
| Gauss-Seidel, Jacobi y SOR | Corte simultáneo por paso y residuo, en norma infinito o euclídea; reordenamiento manual o búsqueda de dominancia estricta por filas; vector de referencia ingresado o calculado por Gauss; errores verdaderos por componente y por norma; corte por error verdadero. |
| Eliminación gaussiana | Diagnóstico separado de qué ocurre sin intercambiar filas, para justificar cuándo aparece un pivote nulo o pequeño. |
| Regresión | Índice `sqrt(1-Sr/St)` para bases, identificado por su fórmula; solución explícita del sistema normal redondeado; búsqueda de raíces del modelo; gráficos y CSV. |
| Interpolación y spline | Gráficos del modelo y, si se ingresó una función de referencia, de su error absoluto. La tabla del interpolador también incluye referencia y error. |

## Dónde están las opciones

En **raíces**, después de la referencia, aparece `Opciones adicionales`. Elegí `1` para configurar las variantes; ENTER conserva las opciones normales. En métodos cerrados podés dar, por ejemplo, `3.2` como aproximación previa para el parcial 2025. En punto fijo podés dar `0.7` como lambda.

En **sistemas iterativos**, los criterios nuevos son:

- **6:** paso y residuo en norma infinito.
- **7:** paso y residuo en norma euclídea.
- **8:** error verdadero en norma infinito.
- **9:** error verdadero en norma euclídea.

Para el pedido de Gauss-Seidel de 2026, elegí 6 o 7 según la norma que adoptes, corte por tolerancia, `1e-7` para el paso y `1e-5` para el residuo. El programa exige **ambas** condiciones. Después de las tolerancias y del máximo de iteraciones aparece `Opciones adicionales`: permite reordenar filas e ingresar el vector de referencia. `gauss` calcula una referencia numérica, que se informa como sujeta al redondeo.

En **regresión**, después del ajuste, el menú permite predecir, buscar una raíz, graficar o resolver el sistema normal redondeado. Para `a+b*v+c/v`, elegí tres bases: `1`, `x`, `1/x`. Para `a*sin(x)+b*cos(x)`, ingresá únicamente las dos bases `sin(x)` y `cos(x)`.

Los gráficos se guardan junto al ejecutable como `grafico_...svg` y `grafico_...csv`, usando nombres consecutivos. El SVG se abre en el navegador sin conexión; el CSV se puede importar en una planilla. Si se dio una función de referencia, también se genera `..._error.svg`. Las curvas se muestrean: no sustituyen una justificación analítica de continuidad o convergencia.

## Ejemplos y bloques para el examen

`Recup1/ejemplos` contiene siete entradas completas, una respuesta por línea, y su guía. Se ejecutaron con los programas reales. Las líneas vacías significan ENTER.

El ejemplo `seidel_doble_corte_control.txt` tiene un vector independiente construido para obtener `(1,2,3,4)`; permite probar el corte doble y **no transcribe el vector borroso de la fotografía**. Cargá los datos efectivos del examen al resolverlo.

Los bloques que cambian los algoritmos están en `CODIGO/comun/`:

- `raices.h`: cociente de Newton, lambda y cortes.
- `sistemas.h`: reordenamiento, referencias y doble corte.
- `regresion.h`: estadísticos y sistema normal redondeado.
- `graficos.h`: gráficos y búsqueda de raíz del modelo.

Los `.cpp` conservan sus ubicaciones y llaman a estas funciones. Si un enunciado pide transcribir un bloque modificado, tomalo de la cabecera correspondiente. `PARAEDITAR` conserva los originales sin cambios.

## Verificación y uso en otra PC

Se compilaron los once programas con C++11 y comprobaciones estrictas de advertencias; pasaron **45 pruebas**, incluidas las 28 anteriores y 17 nuevas. Se ejecutaron además siete ejemplos y se inspeccionaron visualmente las gráficas de interpolación y de error de 2024.

En este PC se configuró un compilador portátil mediante `compilador.local.bat`, sin cambiar el PATH global. Ese archivo es específico de esta computadora. El ZIP incluye fuentes, ejecutables Windows, guías y ejemplos; excluye esa configuración local y el compilador. Si copiás el paquete a otra PC Windows, podés usar los ejecutables incluidos. Para recompilar cambios allí necesitás `g++` con soporte C++11. El menú avisa si usa ejecutables existentes sin recompilar.

Las justificaciones analíticas, la interpretación de los resultados y la revisión de un fragmento de código propuesto en un enunciado requieren elaboración. Los cálculos y desarrollos impresos sirven como apoyo.
