// Version para examen: los datos se ingresan por consola.
// La implementacion y las formulas estan en ../comun/sistemas.h.
#include "../comun/sistemas.h"

int main(int, char **argv) {
    return recup::ejecutar(argv[0], [](recup::Sesion &s) {
        recup::iterativos(s);
    });
}
