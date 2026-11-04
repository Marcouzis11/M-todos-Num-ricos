# Sistemas de EDO con Euler

- `edo_sistema2.cpp`: programa independiente para dos ecuaciones.
- `edo_sistema3.cpp`: programa independiente para tres ecuaciones.
- En `edo.cpp` tambien estan disponibles como opciones **13** y **14**.

La variable independiente es `x` (tambien se acepta `t`). Las incognitas son
`y`, `z`, `w`, o equivalentemente `y1`, `y2`, `y3`.

El programa pide las derivadas despejadas, las condiciones iniciales, el paso
y el punto final. Muestra el primer paso desarrollado, la tabla y los valores
finales. Cada programa independiente guarda todo en `salida_edo_sistema2.txt`
o `salida_edo_sistema3.txt`, en la carpeta desde donde se ejecuta.

Por ejemplo, para dos ecuaciones:

```text
dy/dx = z
dz/dx = -y
x0 = 0
y(x0) = 1
z(x0) = 2
h = 0.1
x final = 0.2
```

Euler da `y(0.2) = 1.39` y `z(0.2) = 1.78`.

Para tres ecuaciones, se puede usar `dy/dx = z`, `dz/dx = w`,
`dw/dx = -y-z`, con condiciones iniciales `(1, 2, 3)`. Con el mismo intervalo
y paso da `(1.43, 2.57, 2.35)`.

Todas las derivadas usan los valores del paso anterior. El ultimo paso se
acorta si hace falta para alcanzar exactamente el punto final. Para integrar
hacia atras se usa un paso negativo. Euler es de orden global 1.

## Compilar

Ejecutar `compilar.bat` en Windows con g++ instalado, o `sh compilar.sh` en Linux.
Los nuevos archivos `.h` deben permanecer junto a los `.cpp`.
Los ejecutables se generan en `CODIGO/`; los `.exe` anteriores en `PROGRAMAS/`
no se actualizan con estos comandos.

Tambien se pueden compilar por separado desde `CODIGO/`:

```sh
g++ -O2 edo_sistema2.cpp -o edo_sistema2
g++ -O2 edo_sistema3.cpp -o edo_sistema3
```

## Verificacion

```sh
python3 CODIGO/tests/test_euler_sistemas.py
```

Las pruebas compilan los programas en una carpeta temporal y comprueban
sistemas acoplados, nombres alternativos, convergencia, el ultimo paso,
integracion hacia atras y las opciones del menu principal.
