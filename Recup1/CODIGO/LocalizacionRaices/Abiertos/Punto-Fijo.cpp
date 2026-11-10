// Version para examen: los datos se ingresan por consola.
// La implementacion y las formulas estan en ../../comun/raices.h.
#include "../../comun/raices.h"

int main(int, char **argv) {
    return recup::ejecutar(argv[0], [](recup::Sesion &s) {
        recup::raiz(s, recup::PUNTO_FIJO);
    });
}
