#!/usr/bin/env bash
# Se puede llamar desde cualquier carpeta. Cada .cpp es un programa independiente.
set -euo pipefail
base="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
compilador="${CXX:-g++}"
command -v "$compilador" >/dev/null || { echo "Falta g++. Instala un compilador C++11 o posterior." >&2; exit 1; }
programas=(
 'LocalizacionRaices/Cerrados/Biseccion'
 'LocalizacionRaices/Cerrados/RegulaFalsi'
 'LocalizacionRaices/Abiertos/Newton-Raphson'
 'LocalizacionRaices/Abiertos/Secante'
 'LocalizacionRaices/Abiertos/Punto-Fijo'
 'SistEcuacionesLineales/Eliminacion-Gaussiana'
 'SistEcuacionesLineales/JacobiGaussSeidelRelax'
 'Interpolacion/interpolacion'
 'Interpolacion/Spline'
 'Regresión/regresionLineal'
 'Regresión/polynomialRegression'
 'menu'
)
for p in "${programas[@]}"; do
    recompilar=0
    [[ ! -x "$base/$p" || "$base/$p.cpp" -nt "$base/$p" ]] && recompilar=1
    for cabecera in "$base"/comun/*.h; do
        [[ "$cabecera" -nt "$base/$p" ]] && recompilar=1
    done
    if [[ "$recompilar" == 1 || "${1:-}" == '--forzar' ]]; then
        echo "Compilando $p.cpp"
        "$compilador" -std=c++11 -O2 -Wall -Wextra -Wpedantic "$base/$p.cpp" -o "$base/$p"
    fi
done
echo "Los 11 metodos y el menu con datos compartidos estan listos."
