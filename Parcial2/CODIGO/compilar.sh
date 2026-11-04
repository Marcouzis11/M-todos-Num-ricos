#!/bin/sh
# Compila los programas del Parcial 2 en Linux/Mac.
cd "$(dirname "$0")"
for p in splines integracion derivacion edo edo_sistema2 edo_sistema3 raices sistemas interpolacion regresion; do
  [ -f "$p.cpp" ] || continue
  echo "Compilando $p.cpp ..."
  g++ -O2 -o "$p" "$p.cpp" -lm
done
echo "Listo."
