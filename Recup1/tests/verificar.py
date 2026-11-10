#!/usr/bin/env python3
"""Pruebas numericas y de consola, sin paquetes externos. No modifica los originales.

Uso: bash Recup1/CODIGO/compilar.sh && python3 Recup1/tests/verificar.py
Los binarios y data.txt se copian a una carpeta temporal; las salidas quedan ahi.
"""
from pathlib import Path
import math
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
            origen = ROOT / 'CODIGO' / rel
            if not origen.is_file():
                raise RuntimeError(f'Primero compilar: falta {origen}')
            destino = cls.base / rel
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
        cls.probe = cls.base / 'evaluador'
        subprocess.run(['g++', '-std=c++11', '-Wall', '-Wextra', '-Werror',
                        '-I', str(ROOT / 'CODIGO/comun'), str(probe),
                        '-o', str(cls.probe)], check=True, capture_output=True, text=True)

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def run_program(self, key, data, ok=True):
        p = subprocess.run([str(self.bin[key])], input=data, text=True,
                           cwd=self.base, capture_output=True, timeout=10)
        self.assertEqual(p.returncode, 0 if ok else 1, p.stdout + p.stderr)
        self.assertNotIn('No se pudo abrir', p.stdout if ok else '')
        return p.stdout

    def valor(self, text, patron):
        match = re.findall(patron + NUM, text)
        self.assertTrue(match, f'No se encontro {patron!r} en:\n{text}')
        return float(match[-1])

    def cerca(self, text, patron, valor, tol=1e-8):
        obtenido = self.valor(text, patron)
        self.assertTrue(math.isfinite(obtenido))
        self.assertAlmostEqual(obtenido, valor, delta=tol)

    def test_01_arbol_y_originales_intactos(self):
        originales = {p.relative_to(SRC) for p in SRC.rglob('*.cpp')}
        self.assertEqual(len(originales), 11)
        for branch in ('PARAEDITAR', 'CODIGO'):
            copia = {p.relative_to(ROOT / branch) for p in (ROOT / branch).rglob('*.cpp')}
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
        self.assertIn('-- | -- | --', out)
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
        out = self.run_program('gauss', entrada(1, 2, '1e-30 0 1e-30', '0 2e-30 4e-30'))
        self.cerca(out, r'x1 = ', 1)
        self.cerca(out, r'x2 = ', 2)
        out = self.run_program('gauss', entrada(1, 2, '1e-300 0 1e-300', '0 1e-300 2e-300'))
        self.assertIn('producto U_ii = 1e-600', out)
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
            for criterio in (1, 2, 3, 4, 5):
                with self.subTest(metodo=metodo, criterio=criterio):
                    datos = [metodo, 1, 2, '4 1 6', '1 3 7']
                    if metodo == 3:
                        datos += [1.1]
                    out = self.run_program('iter', entrada(*datos, '', criterio, 1, '1e-8', 200))
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
        self.cerca(out, r'ECM .* = Sr/n = ', 2/9)
        self.cerca(out, r'RMSE .* = sqrt\(Sr/n\) = ', math.sqrt(2/9))
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
        log = programa.with_name('salida_' + programa.name + '.txt')
        antes = log.read_text() if log.exists() else ''
        out = self.run_program('bis', entrada('x-2', 1, 3, 5, 1, '1e-6', 20, ''))
        despues = log.read_text()
        self.assertTrue(despues.startswith(antes))
        self.assertIn('TEXTO PARA COPIAR EN LA HOJA', despues[len(antes):])
        self.assertIn('f(x), ecuacion f(x)=0: x-2', despues[len(antes):])
        self.assertIn(str(log), out)
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


if __name__ == '__main__':
    unittest.main(verbosity=2)
