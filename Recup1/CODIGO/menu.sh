#!/usr/bin/env bash
set -euo pipefail
base="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
bash "$base/compilar.sh"
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
)
while true; do
    cat <<'MENU'

================ RECUPERATORIO 1 ================
 1) Biseccion                  2) Regula Falsi
 3) Newton-Raphson             4) Secante
 5) Punto fijo
 6) Eliminacion Gaussiana      7) Jacobi / Gauss-Seidel / SOR
 8) Interpolacion              9) Splines
10) Regresion lineal          11) Regresion polinomica y otros ajustes
 0) Salir
MENU
    read -r -p 'Tema: ' opcion || exit 0
    [[ "$opcion" == 0 ]] && exit 0
    if [[ "$opcion" =~ ^([1-9]|10|11)$ ]]; then
        programa="${programas[opcion-1]}"
        # Una cuenta fallida vuelve al menu; conserva su diagnostico y registro.
        "$base/$programa" || true
    else
        echo 'Elegi una opcion del 0 al 11.'
    fi
done
