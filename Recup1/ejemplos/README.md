# Entradas de ejemplo

Cada `.txt` contiene las respuestas para un programa individual, una por línea. Las líneas vacías representan ENTER. Se pueden pegar en la consola del programa correspondiente o redirigir desde cmd, por ejemplo desde la raíz del repositorio:

```bat
Recup1\CODIGO\LocalizacionRaices\Abiertos\Newton-Raphson.exe < Recup1\ejemplos\newton_2026.txt
```

No pegues estas respuestas en el menú general: primero elegí el tema. `seidel_doble_corte_control.txt` usa la matriz de estructura del ejemplo y un vector independiente **de control**, construido para obtener x=(1,2,3,4); no es una transcripción del vector borroso de la foto. Los demás ejemplos indican el año en su nombre. `interpolacion_error_2024.txt` genera las curvas y el gráfico de error.

`regresion_lineal_cursada.txt` reproduce los ocho puntos de `PARAEDITAR/Regresión/regresionLineal.cpp`. Usalo con el programa de regresión lineal (opción 10). La salida desarrolla las derivadas parciales de `S(a,b)`, las sumatorias y este sistema para `f(x)=a*x+b`:

```text
92*a + 20*b = 25
20*a +  8*b = 37

a = -1.60714285714286
b =  8.64285714285714
f(x) = -1.60714285714286*x + 8.64285714285714
```

Incluye la matriz aumentada, su resolución por Gauss y la función final. El resultado principal se calcula con QR para conservar precisión.
