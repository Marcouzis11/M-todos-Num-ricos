// Version para examen: los datos se ingresan por consola.
// La implementacion y las formulas estan en ../comun/splines.h.
#include "../comun/splines.h"

int main(int, char **argv) {
    return recup::ejecutar(argv[0], [](recup::Sesion &s) {
        recup::splines(s);
    });
}
