#!/usr/bin/env python3
"""Pruebas numericas y de consola, sin paquetes externos. No modifica los originales.

Uso: bash Recup1/CODIGO/compilar.sh && python3 Recup1/tests/verificar.py
Los binarios y data.txt se copian a una carpeta temporal; las salidas quedan ahi.
"""
from pathlib import Path
import math
import os
from decimal import Decimal
import csv
import xml.etree.ElementTree as ET
import random
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT.parent / 'Cursada2026Promocion'
PROGRAMAS = {
    'bis': 'LocalizacionRaices/Cerrados/Biseccion',
    'rf': 'LocalizacionRaices/Cerrados/RegulaFalsi',
    'nr': 'LocalizacionRaices/Abiertos/Newton-Raphson',
    'sec': 'LocalizacionRaices/Abiertos/Secante',
    'pf': 'LocalizacionRaices/Abiertos/Punto-Fijo',
    'gauss': 'SistEcuacionesLineales/Eliminacion-Gaussiana',
    'iter': 'SistEcuacionesLineales/JacobiGaussSeidelRelax',
    'interp': 'Interpolacion/interpolacion',
    'spline': 'Interpolacion/Spline',
    'recta': 'Regresión/regresionLineal',
    'reg': 'Regresión/polynomialRegression',
    'menu': 'menu',
}
NUM = r'([-+]?\d+(?:\.\d*)?(?:[eE][-+]?\d+)?)'


def entrada(*lineas):
    return '\n'.join(map(str, lineas)) + '\n'


class Verificacion(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory(prefix='recup1 pruebas ')
        cls.base = Path(cls.tmp.name)
        cls.bin = {}
        for k, rel in PROGRAMAS.items():
            origen = ROOT / 'CODIGO' / (rel + ('.exe' if os.name == 'nt' else ''))
            if not origen.is_file():
                raise RuntimeError(f'Primero compilar: falta {origen}')
            destino = cls.base / (rel + ('.exe' if os.name == 'nt' else ''))
            destino.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(origen, destino)
            cls.bin[k] = destino
        for rel in ('Interpolacion/data.txt', 'SistEcuacionesLineales/data.txt'):
            shutil.copy2(ROOT / 'CODIGO' / rel, cls.base / rel)
        # Prueba directa del evaluador, independiente de los metodos numericos.
        probe = cls.base / 'evaluador.cpp'
        probe.write_text('''#include "base.h"
int main() { std::string s; while(std::getline(std::cin,s)) {
try { std::cout << std::setprecision(17) << recup::evaluar(s,2) << "\\n"; }
catch(const std::exception&) { std::cout << "ERROR\\n"; }
} }
''')
        cls.probe = cls.base / ('evaluador.exe' if os.name == 'nt' else 'evaluador')
        subprocess.run([os.environ.get('CXX', 'g++'), '-std=c++11', '-Wall', '-Wextra', '-Werror',
                        '-I', str(ROOT / 'CODIGO/comun'), str(probe),
                        '-o', str(cls.probe)], check=True, capture_output=True, text=True)

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def run_program(self, key, data, ok=True, avanzado=False, ancho=100):
        # Las entradas historicas no incluian el nuevo menu opcional al final.
        if data and not avanzado and key in ('bis','rf','nr','sec','pf','iter'):
            data += '0\n'
        p = subprocess.run([str(self.bin[key])], input=data, text=True, encoding='utf-8',
                           cwd=self.base, capture_output=True, timeout=10,
                           env=dict(os.environ, RECUP_ANCHO=str(ancho)))
        self.assertEqual(p.returncode, 0 if ok else 1, p.stdout + p.stderr)
        self.assertNotIn('No se pudo abrir', p.stdout if ok and key != 'menu' else '')
        return p.stdout

    def tabla(self, text, indice):
        """Reconstruye filas a partir de los paneles que repiten la columna indice."""
        filas, columnas = {}, None
        for linea in text.splitlines():
            if not (linea.startswith('|') and linea.endswith('|')):
                continue
            celdas = [c.strip() for c in linea[1:-1].split(' | ')]
            if celdas[0] == indice:
                columnas = celdas
            elif columnas and celdas[0].isdigit() and len(celdas) == len(columnas):
                filas.setdefault(celdas[0], {}).update(zip(columnas, celdas))
            else:
                columnas = None
        return filas

    def valor(self, text, patron):
        match = re.findall(patron + NUM, text)
        self.assertTrue(match, f'No se encontro {patron!r} en:\n{text}')
        return float(match[-1])

    def cerca(self, text, patron, valor, tol=1e-8):
        obtenido = self.valor(text, patron)
        self.assertTrue(math.isfinite(obtenido))
        self.assertAlmostEqual(obtenido, valor, delta=tol)

    def test_01_arbol_y_originales_intactos(self):
        fuente = SRC if SRC.is_dir() else ROOT / 'PARAEDITAR'
        originales = {p.relative_to(fuente) for p in fuente.rglob('*.cpp')}
        self.assertEqual(len(originales), 11)
        for branch in ('PARAEDITAR', 'CODIGO'):
            copia = {p.relative_to(ROOT / branch) for p in (ROOT / branch).rglob('*.cpp')
                     if p.relative_to(ROOT / branch) != Path('menu.cpp')}
            self.assertEqual(originales, copia)
        for p in SRC.rglob('*'):
            if p.is_file() and p.suffix in ('.cpp', '.md', '.txt') and 'GeoGebra' not in str(p):
                self.assertEqual(p.read_bytes(), (ROOT/'PARAEDITAR'/p.relative_to(SRC)).read_bytes())

    def test_02_expresiones(self):
        casos = {'-2^2': -4, '2^3^2': 512, '2^-2': .25, '2x+1': 5,
                 '(x+1)(x-1)': 3, '2sen(pi/2)': 2, '0,5+1e-3': .501,
                 'ln(e)+log10(100)+sqrt(4)': 5, '3(x+1)': 9,
                 'abs(-3)+cos(0)': 4}
        p = subprocess.run([str(self.probe)], input=entrada(*casos), text=True,
                           capture_output=True, check=True)
        for (expr, esperado), salida in zip(casos.items(), p.stdout.splitlines()):
            with self.subTest(expr=expr):
                self.assertAlmostEqual(float(salida), esperado, delta=1e-12)
        invalidas = ['1/0', 'sqrt(-1)', 'exp(10000)', 'ln(0)', 'x+', 'x)',
                     'invento(x)', '1 2', '(x+2', '.', '', 'nan', 'inf']
        p = subprocess.run([str(self.probe)], input=entrada(*invalidas), text=True,
                           capture_output=True, check=True)
        self.assertEqual(p.stdout.splitlines(), ['ERROR'] * len(invalidas))

    def test_03_biseccion_cota_y_tabla(self):
        out = self.run_program('bis', entrada('x^2-2', 0, 2, 5, 1, '1e-9', 100, 'sqrt(2)'))
        self.cerca(out, r'RESULTADO: x = ', math.sqrt(2), 1e-9)
        self.assertIn('Primer reemplazo:', out)
        primera = self.tabla(out, 'n')['1']
        for columna in ('E_abs', 'E_rel%', 'E_IUA%'):
            self.assertEqual(primera[columna], '--')
        self.assertIn('Error absoluto verdadero', out)

    def test_04_biseccion_extremos_y_intervalo_invertido(self):
        out = self.run_program('bis', entrada('x-2', 3, 2, 5, 1, '1e-6', 10, ''))
        self.cerca(out, r'RESULTADO: x = ', 2)
        self.cerca(out, r'Iteraciones realizadas = ', 0)
        self.assertIn('Raiz en un extremo', out)
        out = self.run_program('bis', entrada('x^2+1', -1, 1, 5, 1, '1e-6', 10, ''), ok=False)
        self.assertIn('No hay cambio de signo', out)
        self.assertNotIn('No hay raiz', out)

    def test_05_raices_abiertas_y_regula(self):
        for key, prefijo in (
            ('rf', ['x^2-2', 0, 2]),
            ('nr', ['x^2-2', '2*x', 1]),
            ('nr', ['x^2-2', '', 1]),
            ('sec', ['x^2-2', 1, 2]),
            ('pf', ['exp(-x)', '-exp(-x)', 0]),
        ):
            with self.subTest(key=key, prefijo=prefijo):
                out = self.run_program(key, entrada(*prefijo, 4, 1, '1e-10', 200, ''))
                esperado = .5671432904097838 if key == 'pf' else math.sqrt(2)
                self.cerca(out, r'RESULTADO: x = ', esperado)
                self.assertIn('Residuo absoluto', out)

    def test_06_raices_fallos_y_limites(self):
        for key, prefijo, msg in (
            ('nr', ['x^2+1', '2*x', 0], 'Derivada nula'),
            ('sec', ['x^2+1', -1, 1], 'Denominador nulo'),
            ('pf', ['2*x+1', '2', 0], 'maximo'),
        ):
            with self.subTest(key=key):
                out = self.run_program(key, entrada(*prefijo, 4, 1, '1e-10', 5, ''))
                self.assertIn(msg.lower(), out.lower())
                self.assertNotIn('RESULTADO: x', out)
        out = self.run_program('bis', entrada('ln(x)', -1, 2, 5, 1, '1e-6', 10, ''), ok=False)
        self.assertIn('no finito', out)
        out = self.run_program('nr', entrada('sqrt(x-5)-1', '1/(2*sqrt(x-5))', 7, 4, 1, '1e-8', 30, ''))
        self.cerca(out, r'RESULTADO: x = ', 6)

    def test_07_raices_iteraciones_y_criterios(self):
        out = self.run_program('bis', entrada('x^2-2', 0, 2, 5, 2, 3, ''))
        self.cerca(out, r'ULTIMA APROXIMACION .*: x = ', 1.25)
        self.cerca(out, r'Iteraciones realizadas = ', 3)
        for criterio in (1, 2, 3):
            out = self.run_program('nr', entrada('x^2-2', '2*x', 1, criterio, 1, '1e-7', 100, ''))
            self.cerca(out, r'RESULTADO: x = ', math.sqrt(2))

    def test_08_gauss_pivoteo_y_determinante(self):
        out = self.run_program('gauss', entrada(1, 2, '0 2 4', '1 3 7'))
        self.cerca(out, r'x1 = ', 1)
        self.cerca(out, r'x2 = ', 2)
        self.cerca(out, r'producto U_ii = ', -2)
        self.cerca(out, r'\|\|A\*x-b\|\|inf = ', 0)
        self.assertIn('<->', out)
        self.assertIn('PIVOTEO PARCIAL ESCALADO: intercambio F1 <-> F2.', out)
        self.assertIn('RESUMEN DE PIVOTEO: 1 intercambio(s) de filas.', out)
        # El escalado puede elegir una entrada menor en valor absoluto.
        out = self.run_program('gauss', entrada(1, 2, '2 100 202', '1 1 3'))
        self.assertIn('Fila elegida: F2; candidato = 1; escala = 1; |candidato|/escala = 1', out)
        self.assertIn('PIVOTEO PARCIAL ESCALADO: intercambio F1 <-> F2.', out)
        self.cerca(out, r'x1 = ', 1)
        self.cerca(out, r'x2 = ', 2)
        self.cerca(out, r'producto U_ii = ', -98)
        out = self.run_program('gauss', entrada(1, 2, '1e-30 0 1e-30', '0 2e-30 4e-30'))
        self.cerca(out, r'x1 = ', 1)
        self.cerca(out, r'x2 = ', 2)
        self.assertIn('RESUMEN DE PIVOTEO: 0 intercambio(s) de filas.', out)
        self.assertIn('No se necesitaron intercambios de filas.', out)
        out = self.run_program('gauss', entrada(1, 2, '1e-300 0 1e-300', '0 1e-300 2e-300'))
        det = re.search(r'producto U_ii = (\S+)', out).group(1)
        self.assertLess(abs(Decimal(det)/Decimal('1e-600')-1), Decimal('1e-12'))
        self.cerca(out, r'x2 = ', 2)

    def test_09_gauss_singulares_y_archivo(self):
        for filas in (['1 2 3', '2 4 6'], ['1 2 3', '2 4 7'], ['0 0 0', '1 1 2']):
            out = self.run_program('gauss', entrada(1, 2, *filas), ok=False)
            self.assertIn('singular', out)
            self.assertNotIn('\nRESULTADO\n', out)
        out = self.run_program('gauss', entrada(2, ''))
        self.cerca(out, r'\|\|A\*x-b\|\|inf = ', 0, 1e-10)
        archivo = self.base/'sistema sin salto final.txt'
        archivo.write_bytes(b'# archivo con CRLF\r\n0 2 4\r\n1 3 7')
        out = self.run_program('gauss', entrada(2, archivo))
        self.cerca(out, r'x2 = ', 2)
        out = self.run_program('gauss', entrada(2, self.base/'no existe.txt'), ok=False)
        self.assertIn('No se pudo abrir', out)
        archivo.write_text('1 2\n3 4\n')
        out = self.run_program('gauss', entrada(2, archivo), ok=False)
        self.assertIn('n+1', out)

    def test_10_iterativos_tres_metodos_y_cinco_errores(self):
        for metodo in (1, 2, 3):
            for criterio, seleccion in [(1,[1]), (2,[6,1]), (3,[3]), (4,[5]), (5,[6,2])]:
                with self.subTest(metodo=metodo, criterio=criterio):
                    datos = [metodo, 1, 2, '4 1 6', '1 3 7']
                    if metodo == 3:
                        datos += [1.1]
                    out = self.run_program('iter', entrada(*datos, '', *seleccion, 1, '1e-8', 200))
                    self.cerca(out, r'x1 = ', 1)
                    self.cerca(out, r'x2 = ', 2)
                    self.assertIn('\nRESULTADO\n', out)

    def test_11_iterativos_sin_dominancia_y_sin_convergencia(self):
        out = self.run_program('iter', entrada(2, 1, 2, '1 2 5', '0.1 1 2.1', '', 1, 1, '1e-9', 100))
        self.assertIn('NO implica divergencia', out)
        self.cerca(out, r'x1 = ', 1)
        self.cerca(out, r'x2 = ', 2)
        out = self.run_program('iter', entrada(1, 1, 2, '1 2 3', '2 1 3', '', 1, 1, '1e-10', 5))
        self.assertIn('sin convergencia certificada', out)
        self.assertNotIn('\nRESULTADO\n', out)
        out = self.run_program('iter', entrada(2, 1, 2, '0 2 4', '1 3 7', '', 1, 1, '1e-8', 5), ok=False)
        self.assertIn('Diagonal nula', out)

    def test_12_iterativos_numero_fijo_y_vector_inicial(self):
        out = self.run_program('iter', entrada(1, 1, 2, '4 1 6', '1 3 7', '0 1', 1, 2, 1))
        self.cerca(out, r'x1 = ', 1.25)
        self.cerca(out, r'x2 = ', 7/3)
        self.cerca(out, r'Iteraciones = ', 1)
        self.assertIn('Jacobi necesita un punto de partida X^(0)', out)
        self.assertIn('Vector de partida X^(0) = (0, 1)', out)
        self.assertIn('Pivoteo parcial durante las iteraciones: no se aplica.', out)
        self.assertNotIn('PIVOTEO PARCIAL ESCALADO: intercambio', out)

    def test_13_interpolacion_tres_metodos(self):
        for metodo in (1, 2, 3):
            with self.subTest(metodo=metodo):
                out = self.run_program('interp', entrada(1, '2 0 1', '9 1 4', metodo, '(x+1)^2', 0, 1, .5, 0))
                self.cerca(out, r'RESULTADO: P\(0.5\) = ', 2.25)
                self.cerca(out, r'Maximo error en nodos de la forma expandida = ', 0)
                self.assertIn('POLINOMIO EXPANDIDO', out)
                self.assertIn('Resto teorico', out)

    def test_14_interpolacion_nodos_repetidos_y_cota(self):
        out = self.run_program('interp', entrada(1, '0 0 1', '0 1 2'), ok=False)
        self.assertIn('repetidas', out)
        out = self.run_program('interp', entrada(1, '0 1', '0 1', 1, 'x^2', 2, 1, .5, 1, 2, 2, 0, 1, 2, 0))
        self.cerca(out, r'RESULTADO: P\(0.5\) = ', .5)
        self.assertIn('EXTRAPOLACION', out)
        self.assertIn('TABLA DEL POLINOMIO', out)
        self.assertIn('= 0.25', out)

    def test_15_spline_natural_tres_puntos(self):
        out = self.run_program('spline', entrada(1, '0 1 2', '0 1 0', 3, '', 1, .5, 1, 2, 3, '', '', 0))
        self.cerca(out, r'RESULTADO: S\(0.5\) = ', .6875)
        self.cerca(out, r'RESULTADO: S\(2\) = ', 0)
        self.cerca(out, r'integral del spline = ', 1.25)
        self.assertIn('M1=-3', out)
        self.assertIn('Control natural:', out)

    def test_16_spline_sujeto_cubica_exacta(self):
        out = self.run_program('spline', entrada(1, '0 1 2', '0 1 8', 4, 'x^3', 0, 12, 1, .5, 3, 0, 2, 2, 1, 4, 0))
        self.cerca(out, r'RESULTADO: S\(0.5\) = ', .125)
        self.cerca(out, r'integral del spline = ', 4)
        self.assertIn('Control sujeto: S\'(x0)=0', out)
        self.assertIn('LISTAS PARA PEGAR', out)

    def test_17_spline_lineal_cuadratico_y_dos_puntos(self):
        out = self.run_program('spline', entrada(1, '0 1 2', '1 3 5', 1, '', 1, 2, 3, 0, 2, 0))
        self.cerca(out, r'RESULTADO: S\(2\) = ', 5)
        self.cerca(out, r'integral del spline = ', 6)
        out = self.run_program('spline', entrada(1, '0 1 2', '0 1 4', 2, '', 1, 1, .5, 0))
        self.cerca(out, r'RESULTADO: S\(0.5\) = ', .25)
        out = self.run_program('spline', entrada(1, '2 0', '5 1', 3, '', 1, 2, 1, 0, 0))
        self.cerca(out, r'RESULTADO: S\(2\) = ', 5)
        self.cerca(out, r'RESULTADO: S\(0\) = ', 1)

    def test_18_spline_tabla_paso_valido_e_invalido(self):
        out = self.run_program('spline', entrada(1, '0 1 2', '0 1 0', 3, '', 2, 2, .5, 0))
        self.assertIn('h=0.5; subintervalos=4', out)
        out = self.run_program('spline', entrada(1, '0 1 2', '0 1 0', 3, '', 2, 2, .3), ok=False)
        self.assertIn('No se modifica h silenciosamente', out)
        out = self.run_program('spline', entrada(1, '0 1', '0 1', 3, '', 1, 2), ok=False)
        self.assertIn('dentro del dominio', out)

    def test_19_regresion_lineal_r_con_signo(self):
        out = self.run_program('recta', entrada(1, '0 1 2 3', '5 3 1 -1', 1, 4, '', 0))
        self.cerca(out, r'c0 = ', 5)
        self.cerca(out, r'c1 = ', -2)
        self.cerca(out, r'r de Pearson \(con signo\) = ', -1)
        self.cerca(out, r'R\^2 = 1-Sr/St = ', 1)
        self.cerca(out, r'RESULTADO: y\(4\) = ', -3)
        self.assertIn('EXTRAPOLACION', out)

    def test_20_regresion_estadisticos(self):
        # Recta y=1/3+x, residuos [-1/3,2/3,-1/3].
        out = self.run_program('recta', entrada(1, '0 1 2', '0 2 2', 0))
        self.cerca(out, r'c0 = ', 1/3)
        self.cerca(out, r'c1 = ', 1)
        self.cerca(out, r'Sr = suma e_i\^2 = ', 2/3)
        self.cerca(out, r'St = suma \(y_i-media\)\^2 = ', 8/3)
        self.cerca(out, r'ECM .* = Sr/\(n\+1\) = ', 2/9)
        self.cerca(out, r'RMSE .* = sqrt\(Sr/\(n\+1\)\) = ', math.sqrt(2/9))
        self.cerca(out, r'R\^2 = 1-Sr/St = ', .75)
        self.cerca(out, r'r de Pearson \(con signo\) = ', math.sqrt(.75))

    def test_21_regresion_polinomica_y_cero_grados_libertad(self):
        out = self.run_program('reg', entrada(1, '-2 -1 0 1 2', '9 2 1 6 17', 1, 2, 1, 3, 34, 0))
        for j, esperado in enumerate((1, 2, 3)):
            self.cerca(out, rf'c{j} = ', esperado)
        self.cerca(out, r'RESULTADO: y\(3\) = ', 34)
        out = self.run_program('reg', entrada(1, '0 1 2', '1 6 17', 1, 2, 0))
        self.assertIn('sin grados de libertad', out)
        out = self.run_program('reg', entrada(1, '0 1 2', '2 3 4', 1, 0, 0))
        self.cerca(out, r'c0 = ', 3)

    def test_22_regresion_y_constante_y_base_singular(self):
        out = self.run_program('recta', entrada(1, '0 1 2', '5 5 5', 0))
        self.assertIn('R^2: no definido', out)
        self.assertIn('r de Pearson: no definido', out)
        out = self.run_program('recta', entrada(1, '1 1 1', '1 2 3'), ok=False)
        self.assertIn('iguales', out)
        out = self.run_program('reg', entrada(1, '0 1 2', '1 2 3', 2, 2, '1', '2'), ok=False)
        self.assertIn('rango suficiente', out)

    def test_23_regresion_bases_y_linealizaciones(self):
        casos = (
            (2, [2, 'sin(x)', 'cos(x)'], [0, math.pi/2, math.pi], [3, 2, -3], 0, 3),
            (3, [], [0, 1, 2], [2, 2*math.e, 2*math.e**2], .5, 2*math.sqrt(math.e)),
            (4, [], [1, 2, 3], [3, 12, 27], 4, 48),
            (5, [], [1, math.e, math.e**2], [2, 5, 8], math.e**3, 11),
            (6, [], [1, 2, 4], [2, 3, 4], 3, 3.6),
        )
        for tipo, opciones, xs, ys, q, esperado in casos:
            with self.subTest(tipo=tipo):
                out = self.run_program('reg', entrada(1, ' '.join(map(str,xs)), ' '.join(map(str,ys)), tipo, *opciones, 1, q, '', 0))
                self.cerca(out, r'RESULTADO: y\([^)]*\) = ', esperado)
        out = self.run_program('reg', entrada(1, '0 1 2', '0 1 2', 3), ok=False)
        self.assertIn('y>0', out)

    def test_24_regresion_numeros_grandes_y_x_repetidos(self):
        out = self.run_program('recta', entrada(1, '1000000 1000001 1000002 1000003', '1 3 5 7', 1, 1000004, '', 0))
        self.cerca(out, r'RESULTADO: y\(1000004\) = ', 9)
        out = self.run_program('recta', entrada(1, '0 0 1 1', '0 2 2 4', 0))
        self.cerca(out, r'c0 = ', 1)
        self.cerca(out, r'c1 = ', 2)

    def test_25_registros_y_fin_entrada(self):
        programa = self.bin['bis']
        log = programa.with_name('salida_' + programa.stem + '.txt')
        antes = log.read_text(encoding='utf-8') if log.exists() else ''
        out = self.run_program('bis', entrada('x-2', 1, 3, 5, 1, '1e-6', 20, ''))
        despues = log.read_text(encoding='utf-8')
        self.assertTrue(despues.startswith(antes))
        self.assertIn('TEXTO PARA COPIAR EN LA HOJA', despues[len(antes):])
        self.assertIn('f(x), ecuacion f(x)=0: x-2', despues[len(antes):])
        self.assertIn(str(log).replace(chr(92), "/"), out.replace(chr(92), "/"))
        for key in PROGRAMAS:
            out = self.run_program(key, '')
            self.assertIn('Entrada finalizada', out)

    def test_26_validacion_de_entradas(self):
        # Tolerancia negativa y cero se rechazan, y luego se acepta la positiva.
        out = self.run_program('bis', entrada('x^2-2', 0, 2, 5, 1, -1, 0, '1e-6', '2.5', 100, ''))
        self.assertIn('Debe ser mayor que cero', out)
        self.assertIn('Escribi un entero', out)
        self.cerca(out, r'RESULTADO: x = ', math.sqrt(2), 1e-6)

    def test_27_gauss_aleatorio_con_solucion_conocida(self):
        rng = random.Random(2026)
        for n in (1, 3, 5, 10, 20):
            a = [[rng.uniform(-2, 2) for _ in range(n)] for _ in range(n)]
            for i in range(n):
                a[i][i] += 4*n
            x = [rng.uniform(-3, 3) for _ in range(n)]
            b = [sum(ai*xj for ai, xj in zip(fila, x)) for fila in a]
            filas = [' '.join(map(str, fila+[bi])) for fila,bi in zip(a,b)]
            out = self.run_program('gauss', entrada(1, n, *filas))
            for i, esperado in enumerate(x):
                self.cerca(out, rf'x{i+1} = ', esperado)

    def test_28_regresion_grados_y_interpolacion_aleatoria(self):
        rng = random.Random(92)
        for grado in (0, 1, 2, 4, 8):
            c = [rng.uniform(-2, 2) for _ in range(grado+1)]
            xs = [-1+2*i/19 for i in range(20)]
            ys = [sum(cj*x**j for j,cj in enumerate(c)) for x in xs]
            out = self.run_program('reg', entrada(1, ' '.join(map(str,xs)), ' '.join(map(str,ys)), 1, grado, 0))
            for j, esperado in enumerate(c):
                self.cerca(out, rf'c{j} = ', esperado, 1e-7)
        xs = [-2, -1, 0, 1, 2]
        ys = [x**4-2*x+3 for x in xs]
        for metodo in (1, 2, 3):
            out = self.run_program('interp', entrada(1, ' '.join(map(str,xs)), ' '.join(map(str,ys)), metodo, '', '', 1, .3, 0))
            self.cerca(out, r'RESULTADO: P\(0.3\) = ', .3**4-.6+3)


    def test_29_newton_cociente_indice_y_error_por_iteracion(self):
        out=self.run_program('nr',entrada('x^3+2*x-5','3*x^2+2',1.2,1,2,4,'1.3282688557'))
        self.cerca(out,r'C\[3\] = ',.5464097473,1e-6)
        self.cerca(out,r'Error verdadero iteracion 3: absoluto=',1.3996843684e-9,1e-14)
        self.cerca(out,r'A diez decimales: x = ',1.3282688557,1e-10)
        self.assertIn('Para C3 pedir 4 pasos',out)

    def test_30_punto_fijo_relajado_y_raiz_cubica_real(self):
        out=self.run_program('pf',entrada('cbrt(5-2*x)','',1.2,1,2,5,'',1,.7,0),avanzado=True)
        self.cerca(out,r'ULTIMA APROXIMACION .*: x = ',1.3282688464746197,1e-12)
        self.cerca(out,r'lambda=',.7,1e-14)
        out=self.run_program('pf',entrada('cbrt(5-2*x)','',1.2,1,2,5,''))
        self.cerca(out,r'ULTIMA APROXIMACION .*: x = ',1.3292322075449303,1e-12)
        p=subprocess.run([str(self.probe)],input=entrada('cbrt(-8)'),text=True,capture_output=True,check=True)
        self.assertEqual(float(p.stdout),-2)

    def test_31_biseccion_2025_primera_comparacion_iua(self):
        out=self.run_program('bis',entrada('(x+1)/(x+4)-.25*x',1.75,3.2,3,1,.01,100,2,1,3.2,1,'',0),avanzado=True)
        primera=self.tabla(out,'n')['1']
        self.assertAlmostEqual(float(primera['E_IUA%']),25.55066079295154,delta=1e-10)
        self.cerca(out,r'RESULTADO: x = ',2.000103759765625,1e-14)
        self.cerca(out,r'Iteraciones realizadas = ',13)

    def test_32_corte_doble_no_atajo_por_residuo_cero(self):
        # En el paso 1 el residuo es cero, pero el salto es 1 > 0.5.
        out=self.run_program('iter',entrada(2,1,1,'1 1','0',6,3,1,.5,10,'1e-5'))
        self.cerca(out,r'Iteraciones = ',2)
        self.assertIn('Y residuo <',out)

    def test_33_corte_doble_norma_euclidea(self):
        for criterio,iteraciones in [(6,1),(7,2)]:
            out=self.run_program('iter',entrada(2,1,2,'1 0 1','0 1 1','0 0',6,3 if criterio==6 else 4,1,1.2,10,'1e-5'))
            self.cerca(out,r'Iteraciones = ',iteraciones)
        self.assertIn('Residuo euclideo iteracion',out)

    def test_34_comparacion_estricta_e_inclusiva(self):
        for comparacion,pasos in [(1,2),(2,1)]:
            out=self.run_program('bis',entrada('x^2-2',0,2,1,1,1,20,'',1,0,comparacion,'',0),avanzado=True)
            self.cerca(out,r'Iteraciones realizadas = ',pasos)

    def test_35_reorden_2024_y_referencia_exacta(self):
        out=self.run_program('iter',entrada(2,1,3,'0 2 5 -14','1 -4 2 -19','5 1 -2 16','0 0 0',2,'1 3 -4',1,'1e-4',100,1,2,1),avanzado=True)
        self.assertIn('Permutacion de ecuaciones: 3 2 1',out)
        self.cerca(out,r'Error verdadero iteracion 1: inf=',2.55)
        self.assertLess(self.valor(out,r'Error verdadero infinito = '),1e-4)
        self.assertIn('A diez decimales:',out)

    def test_36_permutacion_invalida_y_dominancia_imposible(self):
        out=self.run_program('iter',entrada(2,1,2,'4 1 6','1 3 7','0 0',1,2,1,1,1,'1 1'),ok=False,avanzado=True)
        self.assertIn('Permutacion invalida',out)
        out=self.run_program('iter',entrada(2,1,2,'1 2 3','2 1 3','0 0',1,2,1,1,2,''),avanzado=True)
        # En este sistema SI se puede reordenar (orden 2,1).
        self.assertIn('Permutacion de ecuaciones: 2 1',out)
        out=self.run_program('iter',entrada(2,1,2,'1 1 2','1 -1 0','0 0',1,2,1,1,2,''),avanzado=True)
        self.assertIn('No existe una permutacion',out)

    def test_37_regresion_2026_bases_y_redondeo(self):
        out=self.run_program('reg',entrada(1,'1 1.5 2 3 4 5','5.04 7.28 9.56 13.61 17.82 21.76',2,3,'1','x','1/x',4,3,0))
        self.cerca(out,r'Indice de correlacion multiple R=sqrt\(R\^2\) = ',.9999584918136352,1e-12)
        self.cerca(out,r'Coeficiente redondeado c0 = ',1.9751655,1e-7)
        self.cerca(out,r'Coeficiente redondeado c2 = ',-.95593693,1e-7)

    def test_38_regresion_2025_trigonometrica(self):
        out=self.run_program('reg',entrada(1,'1 1.2 1.5 2 2.6 2.9 3','-.236 .209 .853 1.746 2.231 2.163 2.110',2,2,'sin(x)','cos(x)',0))
        self.cerca(out,r'c0 = ',1.0007923964264187,1e-10)
        self.cerca(out,r'c1 = ',-1.9915083519703431,1e-10)
        self.cerca(out,r'Indice de correlacion multiple R=sqrt\(R\^2\) = ',.9999787168843552,1e-12)

    def test_39_raiz_modelo_lineal_2024(self):
        out=self.run_program('recta',entrada(1,'1 1.2 1.5 1.75 2','-.148 -.040 .181 .419 .700',2,1,2,'1e-12','1e-12',200,'1.2620326547',0))
        self.cerca(out,r'RESULTADO: raiz del modelo = ',1.2272482649899428,1e-11)
        self.cerca(out,r'Error absoluto verdadero = \|aproximado - referencia\| = ',abs(1.2272482649899428-1.2620326547),1e-11)

    def test_40_graficos_interpolacion_csv_error_y_no_sobrescribir(self):
        antes=set(self.base.rglob('grafico_interpolacion_*.csv'))
        out=self.run_program('interp',entrada(1,'0 1 2','0 1 4',1,'x^2+.5','',3,0,2,20,3,0,2,20,0))
        nuevos=set(self.base.rglob('grafico_interpolacion_*.csv'))-antes
        self.assertEqual(len(nuevos),2)
        for f in nuevos:
            with f.open(newline='') as h: filas=list(csv.DictReader(h))
            self.assertEqual(len(filas),21)
            for fila in filas:self.assertAlmostEqual(float(fila['error_absoluto']),.5,delta=1e-12)
            for svg in [f.with_suffix('.svg'),f.with_name(f.stem+'_error.svg')]:
                root=ET.parse(svg).getroot();self.assertEqual(root.tag,'{http://www.w3.org/2000/svg}svg')
                for el in root.iter():
                    if el.tag.endswith('path'):self.assertNotRegex(el.attrib.get('d',''),r'(?i)nan|inf')
        self.assertIn('GRAFICO DEL ERROR',out)

    def test_41_grafico_raices_con_huecos_de_dominio(self):
        antes=set(self.base.rglob('grafico_funcion_*.csv'))
        out=self.run_program('bis',entrada('ln(x)',.5,2,5,1,'1e-6',100,'',1,'',1,'',1,-1,2,30),avanzado=True)
        nuevo=(set(self.base.rglob('grafico_funcion_*.csv'))-antes).pop()
        with nuevo.open(newline='') as h:filas=list(csv.DictReader(h))
        self.assertTrue(any(not f['modelo'] for f in filas))
        ET.parse(nuevo.with_suffix('.svg'))
        self.assertIn('Muestras con evaluaciones invalidas: 11',out)

    def test_42_spline_2025_y_grafico(self):
        out=self.run_program('spline',entrada(1,'1 3 5 7 13','800 2310 3090 3940 4755',3,'',1,10,4,1,13,40,0))
        self.cerca(out,r'RESULTADO: S\(10\) = ',4629.283405172413,1e-8)
        self.assertIn('GRAFICO:',out)

    def test_43_redondeo_singular_no_destruye_modelo_principal(self):
        out=self.run_program('reg',entrada(1,'0 1 2','0 1 2',2,2,'x','x+.001',4,0,1,1.5,'',0))
        self.assertIn('El sistema redondeado no se pudo resolver',out)
        self.cerca(out,r'RESULTADO: y\(1.5\) = ',1.5)

    def test_44_referencia_numerica_gauss_y_error_verdadero(self):
        out=self.run_program('iter',entrada(2,1,2,'4 1 6','1 3 7','0 0',6,5,'gauss',1,'1e-7',100,1,0,1),avanzado=True)
        self.assertIn('sujeta al redondeo',out)
        self.assertIn('CALCULO DE REFERENCIA POR GAUSS: se detalla su pivoteo parcial escalado.',out)
        self.assertIn('RESUMEN DE PIVOTEO:',out)
        self.assertLess(self.valor(out,r'Error verdadero euclideo = '),1e-7)

    def test_45_corte_doble_raices_y_residuo_original_relajado(self):
        # g(x)=1: alcanzar el punto fijo no debe saltar el doble corte con un paso grande.
        out=self.run_program('pf',entrada('1','0',0,1,1,.5,10,'',1,1,1,'1e-5',0),avanzado=True)
        self.cerca(out,r'Iteraciones realizadas = ',2)
        # Lambda diminuto da pasos pequenos, pero g(x)-x sigue lejos de cero.
        out=self.run_program('pf',entrada('1','0',0,1,1,'1e-6',3,'',1,'1e-10',1,'1e-5',0),avanzado=True)
        self.assertNotIn('RESULTADO: x = ',out)
        self.assertGreater(self.valor(out,r'Residuo absoluto = '),.99)

    def test_46_tablas_angostas_conservan_todas_las_columnas_y_precision(self):
        datos = entrada('x^2-2', 0, 2, 5, 2, 4, '')
        angosta = self.run_program('bis', datos, ancho=40)
        ancha = self.run_program('bis', datos, ancho=240)
        self.assertEqual(self.tabla(angosta, 'n'), self.tabla(ancha, 'n'))
        self.assertEqual(len(self.tabla(angosta, 'n')), 4)
        self.assertIn('Columnas - bloque', angosta)
        for linea in angosta.splitlines():
            if linea.startswith(('|', '+')):
                self.assertLessEqual(len(linea), 40, linea)
        # Todos los bordes verticales coinciden dentro de cada panel.
        posiciones = None
        for linea in angosta.splitlines():
            if linea.startswith('+'):
                posiciones = [i for i, c in enumerate(linea) if c == '+']
            elif linea.startswith('|'):
                self.assertEqual([i for i, c in enumerate(linea) if c == '|'], posiciones)

    def test_47_diferencias_divididas_encabezados_completos(self):
        out = self.run_program('interp', entrada(1, '0 1 2 3', '0 1 4 9', 3, '', '', 0))
        filas = self.tabla(out, 'i')
        self.assertEqual(filas['0']['D[i,3]'], '0')
        self.assertEqual(filas['3']['D[i,1]'], '--')
        self.assertNotIn('| ...', out)

    def test_48_menu_vertical_y_registro_con_tablas(self):
        datos = entrada(1, '0 1 2', '1 3 5', 0)
        out = self.run_program('recta', datos)
        self.assertIn('RECUPERATORIO 1 | REGRESION LINEAL', out)
        self.assertIn('\n  2=archivo de filas x y', out)
        self.assertIn('DATOS DE ENTRADA', out)
        informe = out[out.index('\nTEXTO PARA COPIAR EN LA HOJA\n'):]
        informe = informe[:informe.index('\nGuardado en:')]
        log = self.bin['recta'].with_name('salida_regresionLineal.txt').read_text(encoding='utf-8')
        self.assertIn(informe, log)
        self.assertNotIn('\x1b', out)

    def test_49_interrupcion_conserva_tabla_parcial(self):
        out = self.run_program('pf', entrada('exp(x)', '', 2, 1, 2, 10, ''), ok=False)
        filas = self.tabla(out, 'n')
        self.assertTrue(filas)
        self.assertIn('CALCULO INTERRUMPIDO', out)
        self.assertNotIn('RESULTADO: x = ', out)

    def test_50_recta_ejemplo_original_derivadas_matriz_y_funcion(self):
        datos = (ROOT / 'ejemplos/regresion_lineal_cursada.txt').read_text()
        out = self.run_program('recta', datos)
        for nombre, esperado in [('x', 20), ('y', 37), ('x^2', 92), ('x*y', 25)]:
            self.cerca(out, r'suma ' + re.escape(nombre) + ' = ', esperado)
        self.assertIn('dS/da = -2*suma_i x_i*[y_i - (a*x_i+b)] = 0', out)
        self.assertIn('dS/db = -2*suma_i [y_i - (a*x_i+b)] = 0', out)
        self.assertIn('a*suma(x_i^2) + b*suma(x_i) = suma(x_i*y_i)', out)
        self.assertIn('(92)*a + (20)*b = 25', out)
        self.assertIn('(20)*a + (8)*b = 37', out)
        self.assertIn('n=7; cantidad de datos n+1=8', out)
        self.assertIn('[ Σ(x_i^2)  Σ(x_i) | Σ(x_i*y_i) ]', out)
        self.assertIn('[ Σ(x_i)    n+1    | Σ(y_i)     ]', out)
        self.assertIn('n+1 equivale a Σ(1)', out)
        normal = out.split('MATRIZ AUMENTADA: incognitas en orden [a, b]')[1]
        normal = normal.split('Desarrollo por Gauss')[0]
        filas = self.tabla(normal, 'Fila')
        self.assertEqual([filas['1'][c] for c in ('coef. de a', 'coef. de b', 'termino indep.')], ['92', '20', '25'])
        self.assertEqual([filas['2'][c] for c in ('coef. de a', 'coef. de b', 'termino indep.')], ['20', '8', '37'])
        self.cerca(out, r'Solucion por Gauss: a = ', -45/28)
        self.cerca(out, r'a \(pendiente\) = ', -45/28)
        self.cerca(out, r'b \(ordenada al origen\) = ', 121/14)
        funcion = re.search(r'f\(x\) = ' + NUM + r'\*x \+ ' + NUM, out)
        self.assertIsNotNone(funcion, out)
        self.assertAlmostEqual(float(funcion[1]), -45/28)
        self.assertAlmostEqual(float(funcion[2]), 121/14)
        # La formula final reproduce las mismas predicciones que los coeficientes.
        self.assertAlmostEqual(float(funcion[1])*2 + float(funcion[2]), 38/7)
        self.assertIn('Retrosustitucion:', out)

    def test_51_recta_ordenada_negativa_y_opcion_polinomica(self):
        for key, opciones in [('recta', []), ('reg', [1, 1])]:
            with self.subTest(programa=key):
                out = self.run_program(key, entrada(1, '0 1 2', '-3 -1 1', *opciones, 0))
                self.assertIn('f(x) = 2*x - 3', out)
                self.cerca(out, r'a \(pendiente\) = ', 2)
                self.cerca(out, r'b \(ordenada al origen\) = ', -3)
                self.assertNotIn('MODELO ORIGINAL:', out)

    def test_52_regresion_cuadratica_derivadas_por_coeficiente(self):
        out = self.run_program('reg', entrada(1, '0 1 2', '1 6 17', 1, 2, 0))
        for j in range(3):
            self.assertIn(f'dS/dc{j} = -2*suma_i phi{j}(x_i)*[y_i - modelo_i] = 0', out)
        self.assertIn('(3)*c0 + (3)*c1 + (5)*c2 = 24', out)
        self.assertIn('(3)*c0 + (5)*c1 + (9)*c2 = 40', out)
        self.assertIn('(5)*c0 + (9)*c1 + (17)*c2 = 74', out)
        self.assertIn('f(x) = ', out)
        for j, valor in enumerate([1, 2, 3]):
            self.cerca(out, rf'c{j} = ', valor)

    def test_53_menu_cuatro_metodos_una_matriz(self):
        config = ['', 1, 1, '1e-8', 200, 0]
        out = self.run_program('menu', entrada(
            6, 1, 2, '4 1 6', '1 3 7',
            1, 2, *config, 3, *config, 4, 1.1, *config, 0, 0))
        self.assertEqual(out.count('Cantidad de incognitas'), 1)
        self.assertEqual(out.count('Fila 1 (coeficientes y termino independiente)'), 1)
        for nombre in ['ELIMINACION GAUSSIANA', 'JACOBI', 'GAUSS-SEIDEL', 'SOR']:
            self.assertIn(nombre, out)
        self.assertEqual(out.count('\nRESULTADO\n'), 4)
        self.cerca(out, r'x1 = ', 1)
        self.cerca(out, r'x2 = ', 2)

    def test_54_menu_interpolacion_splines_regresion_una_tabla(self):
        out = self.run_program('menu', entrada(
            8, 1, '2 0 1', '9 1 4',
            1, '', '', 1, .5, 0,           # Lagrange: P(0.5)=2.25
            3, '', '', 1, .5, 0,           # Newton, sin volver a pegar x e y
            6, '', 1, .5, 0,               # Spline natural: S(0.5)=2.3125
            8, 0,                          # Recta
            9, 2, 0,                       # Polinomio de grado 2
            0, 0))
        self.assertEqual(out.count('Valores de x separados por espacios'), 1)
        self.assertEqual(out.count('Valores de y en el mismo orden'), 1)
        self.assertEqual(out.count('RESULTADO: P(0.5) = 2.25'), 2)
        self.cerca(out, r'RESULTADO: S\(0.5\) = ', 2.3125)
        self.cerca(out, r'c0 = ', 1)
        self.cerca(out, r'c1 = ', 2)
        self.cerca(out, r'c2 = ', 1)
        # Ordenar para interpolar no cambia el orden original usado por regresion.
        recta = out.split('REGRESION POR MINIMOS CUADRADOS')[0]
        self.assertEqual(self.tabla(recta, 'i')['0']['x_i'], '2')

    def test_55_menu_error_de_interpolacion_conserva_tabla_para_regresion(self):
        out = self.run_program('menu', entrada(
            10, 1, '0 0 1', '0 2 4', 1, 8, 0, 0, 0))
        self.assertIn('Hay abscisas repetidas', out)
        self.assertEqual(out.count('Valores de x separados por espacios'), 1)
        self.cerca(out, r'a \(pendiente\) = ', 3)
        self.cerca(out, r'b \(ordenada al origen\) = ', 1)

    def test_56_menu_reemplazo_atomico_y_datos_entre_familias(self):
        out = self.run_program('menu', entrada(
            6, 1, 1, '1 2', 1,          # Primera matriz: x1=2
            5, 2, self.base/'ausente.txt', 1,  # Falla la recarga; sigue x1=2
            5, 1, 1, '2 10', 1, 0,     # Nueva matriz: x1=5; volver al principal
            8, 1, '0 1 2', '1 3 5', 8, 0, 0,
            6, 1, 0,                    # Volver a sistemas sin cargar; x1=5
            10, 8, 0, 0, 0))           # Volver a la tabla sin cargar
        self.assertIn('No se pudo abrir', out)
        self.assertEqual(out.count('Cantidad de incognitas'), 2)
        self.assertEqual(out.count('Valores de x separados por espacios'), 1)
        self.assertEqual(out.count('\nx1 = 2\n'), 2)
        self.assertEqual(out.count('\nx1 = 5\n'), 2)
        self.cerca(out, r'a \(pendiente\) = ', 2)
        self.cerca(out, r'b \(ordenada al origen\) = ', 1)

    def test_57_menu_reordenar_no_modifica_matriz_original_y_archivos(self):
        out = self.run_program('menu', entrada(
            7, 1, 2, '0 2 4', '1 3 7',
            3, '', 1, 1, '1e-8', 100, 1, 1, '2 1', '', 1,
            1, 0, 0))
        self.assertIn('Permutacion de ecuaciones: 2 1', out)
        self.assertIn('F1 <-> F2', out)  # Gauss recibe de nuevo la fila original de diagonal nula.
        self.assertEqual(out.count('Cantidad de incognitas'), 1)
        self.cerca(out, r'x1 = ', 1)
        self.cerca(out, r'x2 = ', 2)
        # ENTER sigue encontrando los data.txt de cada familia desde el menu comun.
        out = self.run_program('menu', entrada(6, 2, '', 1, 0, 8, 2, '', 16, 0, 0))
        self.assertIn('Matriz cargada.', out)
        self.assertIn('Tabla cargada.', out)
        self.assertIn('SistEcuacionesLineales/data.txt', out)
        self.assertIn('Interpolacion/data.txt', out)

    def test_58_errores_verdaderos_sin_opciones_adicionales(self):
        for opcion, tolerancia, esperado in [(2,.05,1/24), (4,3,100/48)]:
            for key in ('iter', 'menu'):
                with self.subTest(error=opcion, programa=key):
                    if key == 'iter':
                        datos = entrada(2,1,2,'4 1 6','1 3 7','0 0',opcion,'1 2',1,tolerancia,100,0)
                    else:
                        datos = entrada(6,1,2,'4 1 6','1 3 7',3,'0 0',opcion,'1 2',1,tolerancia,100,0,0,0)
                    out = self.run_program(key,datos,avanzado=True)
                    self.cerca(out,r'Iteraciones = ',2)
                    self.cerca(out,r'ultimo E = ',esperado,1e-12)
                    self.assertEqual(out.count('Solucion exacta de referencia, separada por espacios'),1)
                    self.assertNotIn('Criterio avanzado:',out)
                    if opcion==4:
                        self.assertIn('Tolerancia porcentual (0.1 significa 0.1%)',out)
                        self.assertIn('Relativo porcentual exacto o verdadero',out)

    def test_59_error_porcentual_verdadero_referencia_nula(self):
        out = self.run_program('iter',entrada(2,1,1,'1 0','0',4,'0'),ok=False,avanzado=True)
        self.assertIn('no esta definido para una referencia nula',out)
        self.assertIn('Usa el error absoluto verdadero',out)
        self.assertNotIn('\nRESULTADO\n',out)


if __name__ == '__main__':
    unittest.main(verbosity=2)
