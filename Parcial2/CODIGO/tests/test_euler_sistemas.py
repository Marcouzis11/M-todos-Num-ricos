"""Pruebas de los programas interactivos: python3 CODIGO/tests/test_euler_sistemas.py."""
import math
from pathlib import Path
import re
import subprocess
import tempfile
import unittest


CODIGO = Path(__file__).resolve().parents[1]


class EulerSistemasTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix="euler-sistemas-")
        cls.directorio = Path(cls.temp.name)
        for programa in ("edo", "edo_sistema2", "edo_sistema3"):
            subprocess.run(
                ["g++", "-std=c++11", "-O2", str(CODIGO / (programa + ".cpp")),
                 "-o", str(cls.directorio / programa)], check=True, capture_output=True,
            )

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def ejecutar(self, programa, datos):
        proceso = subprocess.run(
            [str(self.directorio / programa)],
            input="\n".join(map(str, datos)) + "\n", text=True,
            capture_output=True, cwd=self.directorio, timeout=10, check=True,
        )
        return proceso.stdout

    def resolver(self, ecuaciones, inicial, h, xf, x0=0, cada=1):
        n = len(ecuaciones)
        datos = ecuaciones + [x0] + inicial + [h, xf]
        if abs((xf - x0) / h) > 25:
            datos.append(cada)
        return self.ejecutar("edo_sistema" + str(n), datos + [0])

    def resultado(self, salida, esperado, xf):
        self.assertIn("RESULTADO:", salida)
        bloque = salida.split("RESULTADO:")[-1]
        filas = re.findall(r"([yzw])\(([^)]+)\) ~ ([^\s]+)", bloque)
        self.assertEqual(len(filas), len(esperado))
        for (_, x, valor), referencia in zip(filas, esperado):
            self.assertAlmostEqual(float(x), xf, places=8)
            self.assertAlmostEqual(float(valor), referencia, places=8)

    def test_dos_ecuaciones_actualizacion_simultanea(self):
        salida = self.resolver(["z", "-y"], [1, 2], 0.1, 0.2)
        self.resultado(salida, [1.39, 1.78], 0.2)
        self.assertIn("PRIMERA ITERACION", salida)
        log = (self.directorio / "salida_edo_sistema2.txt").read_text()
        self.assertIn("z(0.2) ~ 1.7800000000", log)

    def test_tres_ecuaciones_no_lineales_y_dependencia_de_x(self):
        u = [1.0, 2.0, 3.0]
        for i in range(3):
            y, z, w = u
            x = i * 0.1
            u = [y + 0.1*(z*w+x), z + 0.1*(y-w+x*x), w + 0.1*(y*z-x)]
        salida = self.resolver(["z*w+x", "y-w+x^2", "y*z-x"], [1, 2, 3], 0.1, 0.3)
        self.resultado(salida, u, 0.3)

    def test_aliases_t_y1_y2_y3(self):
        for ecuaciones, esperado in [
            (["y2+t", "-y1"], [1.2, 1.9]),
            (["y2+t", "y3", "-y1-y2"], [1.2, 2.3, 2.7]),
        ]:
            with self.subTest(ecuaciones=ecuaciones):
                salida = self.resolver(ecuaciones, [1, 2, 3][:len(ecuaciones)], 0.1, 0.1)
                self.resultado(salida, esperado, 0.1)

    def test_ultimo_paso_y_marcha_hacia_atras(self):
        for x0, h, xf in [(0, 0.3, 1), (1, -0.3, 0)]:
            for n in (2, 3):
                with self.subTest(x0=x0, n=n):
                    salida = self.resolver(["1"]*n, [x0]*n, h, xf, x0=x0)
                    self.resultado(salida, [xf]*n, xf)

    def test_intervalo_nulo(self):
        salida = self.resolver(["z", "w", "y"], [1, 2, 3], 0.1, 0)
        self.resultado(salida, [1, 2, 3], 0)
        self.assertNotIn("PRIMERA ITERACION", salida)

    def test_direccion_incorrecta_y_limite_de_pasos(self):
        salida = self.resolver(["z", "y"], [1, 1], -0.1, 1)
        self.assertIn("El signo de h", salida)
        self.assertNotIn("RESULTADO:", salida)
        salida = self.ejecutar("edo_sistema3", ["z", "w", "y", 0, 1, 1, 1, 0.00001, 1, 0])
        self.assertIn("mas de 20000 pasos", salida)
        self.assertNotIn("RESULTADO:", salida)

    def test_dominio_invalido_no_imprime_resultado(self):
        salida = self.resolver(["sqrt(y)", "y"], [-1, 2], 0.1, 0.1)
        self.assertIn("No se pudo completar el paso 1", salida)
        self.assertNotIn("RESULTADO:", salida)

    def test_convergencia_de_orden_uno(self):
        for ecuaciones in (["z", "y"], ["z", "w", "y"]):
            errores = []
            for h in (0.05, 0.025):
                salida = self.resolver(ecuaciones, [1]*len(ecuaciones), h, 1, cada=20)
                valor = float(re.findall(r"y\(1\) ~ ([^\s]+)", salida)[-1])
                errores.append(abs(valor - math.e))
                self.assertRegex(salida, r"40 |20 ")
            orden = math.log2(errores[0]/errores[1])
            self.assertAlmostEqual(orden, 1, delta=0.06)

    def test_menu_principal_y_problema_escalar_se_conserva(self):
        salida = self.ejecutar("edo", [
            "y", 0, 1, 0.1, 0.2, "",  # Problema escalar inicial.
            13, "z", "-y", 0, 1, 2, 0.1, 0.2, "",
            14, "z", "w", "-y-z", 0, 1, 2, 3, 0.1, 0.2, "",
            2, "", 0,
        ])
        self.assertIn("z(0.2) ~ 1.7800000000", salida)
        self.assertIn("w(0.2) ~ 2.3500000000", salida)
        self.assertIn("RESULTADO:  y(0.2) ~ 1.2100000000", salida)


if __name__ == "__main__":
    unittest.main(verbosity=2)
