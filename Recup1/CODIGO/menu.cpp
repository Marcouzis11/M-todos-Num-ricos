// Menu comun: conserva los datos originales durante toda la sesion.
#include "comun/raices.h"
#include "comun/sistemas.h"
#include "comun/interpolacion.h"
#include "comun/splines.h"
#include "comun/regresion.h"

namespace recup {
// Un fallo de un metodo vuelve al menu y conserva los datos cargados.
void intentar(Sesion &s,const std::function<void()> &tarea){
    try{tarea();}
    catch(const std::exception &e){informarFallo(s,e);}
}
void nuevaMatriz(Sesion &s,Sistema &datos){
    Sistema nuevos;
    cargarSistema(s,nuevos.a,nuevos.b,"SistEcuacionesLineales/data.txt");
    datos=std::move(nuevos);
    s.hoja<<"Matriz cargada. Se conserva para todos los metodos de sistemas.\n";
    s.mostrar();
}
void nuevaTabla(Sesion &s,Puntos &datos){
    // Se admiten x repetidos: cada interpolador valida su propia copia.
    Puntos nuevos=puntos(s,false,nullptr,"Interpolacion/data.txt");
    datos=std::move(nuevos);
    s.hoja<<"Tabla cargada. Se conserva para interpolacion, splines y regresion.\n";
    s.mostrar();
}
void menuSistemas(Sesion &s,Sistema &datos,int sugerido){
    for(;;){
        if(datos.a.empty()){
            titulo(std::cout,"CARGAR MATRIZ DEL SISTEMA",'-');
            intentar(s,[&](){nuevaMatriz(s,datos);});
            if(datos.a.empty())continue;
        }
        titulo(std::cout,"SISTEMAS | MISMA MATRIZ",'-');
        std::cout<<"Matriz activa: "<<datos.a.size()<<" incognitas. No hace falta volver a cargarla.\n";
        int op=s.entero("Metodo: 1=Eliminacion Gaussiana, 2=Jacobi, 3=Gauss-Seidel, 4=SOR, 5=cargar otra matriz, 6=ver matriz actual, 0=volver al menu principal",0,6,std::to_string(sugerido));
        if(op==0)return;
        intentar(s,[&](){
            if(op==5){nuevaMatriz(s,datos);return;}
            if(op==6){informarSistema(s,datos.a,datos.b);s.mostrar();return;}
            sugerido=op;
            if(op==1)eliminacion(s,&datos);
            else iterativos(s,&datos,op-1);
        });
    }
}
void menuTablas(Sesion &s,Puntos &datos,int sugerido){
    for(;;){
        if(datos.x.empty()){
            titulo(std::cout,"CARGAR TABLA DE PUNTOS",'-');
            intentar(s,[&](){nuevaTabla(s,datos);});
            if(datos.x.empty())continue;
        }
        titulo(std::cout,"INTERPOLACION / SPLINES / REGRESION | MISMA TABLA",'-');
        std::cout<<"Tabla activa: "<<datos.x.size()<<" puntos. No hace falta volver a cargarlos.\n"
                 <<"En las consultas de cada metodo, 0 vuelve a este menu para cambiar de metodo.\n";
        int op=s.entero("Metodo: 1=Lagrange, 2=Vandermonde y Gauss, 3=Newton con diferencias divididas, 4=Spline lineal, 5=Spline cuadratico, 6=Spline cubico natural, 7=Spline cubico sujeto, 8=Regresion lineal, 9=Regresion polinomica, 10=Regresion con base de funciones, 11=Ajuste exponencial, 12=Ajuste potencial, 13=Ajuste logaritmico, 14=Ajuste de saturacion, 15=cargar otra tabla, 16=ver tabla actual, 0=volver al menu principal",0,16,std::to_string(sugerido));
        if(op==0)return;
        intentar(s,[&](){
            if(op==15){nuevaTabla(s,datos);return;}
            if(op==16){puntos(s,false,&datos);s.mostrar();return;}
            sugerido=op;
            if(op<=3)interpolacion(s,&datos,op);
            else if(op<=7)splines(s,&datos,op-3);
            else if(op==8)regresion(s,true,&datos);
            else regresion(s,false,&datos,op-8);
        });
    }
}
void menu(Sesion &s){
    Sistema sistema;Puntos tabla;
    for(;;){
        titulo(std::cout,"RECUPERATORIO 1 | METODOS NUMERICOS");
        std::cout<<"LOCALIZACION DE RAICES\n"
                 <<"  1) Biseccion\n  2) Regula Falsi\n  3) Newton-Raphson\n  4) Secante\n  5) Punto fijo\n\n"
                 <<"SISTEMAS DE ECUACIONES (comparten matriz)\n"
                 <<"  6) Sistemas: Gauss / Jacobi / Gauss-Seidel / SOR\n"
                 <<"  7) Sistemas: acceso a iterativos\n\n"
                 <<"INTERPOLACION Y AJUSTE DE CURVAS (comparten tabla)\n"
                 <<"  8) Interpolacion\n  9) Splines\n 10) Regresion lineal\n 11) Regresion polinomica y otros ajustes\n\n"
                 <<"  0) Salir\n\n"
                 <<"Los datos se conservan al volver a este menu, hasta cerrar el programa.\n"
                 <<"Los informes de esta sesion se guardan en: "<<s.ruta<<'\n';
        if(!sistema.a.empty())std::cout<<"Matriz disponible: "<<sistema.a.size()<<" incognitas.\n";
        if(!tabla.x.empty())std::cout<<"Tabla disponible: "<<tabla.x.size()<<" puntos.\n";
        int op=s.entero("Elegi una opcion [0-11]",0,11,"0");
        if(op==0)return;
        intentar(s,[&](){
            if(op<=5)raiz(s,static_cast<MetodoRaiz>(op-1));
            else if(op<=7)menuSistemas(s,sistema,op==6?1:3);
            else menuTablas(s,tabla,op==8?1:op==9?6:op==10?8:9);
        });
    }
}
} // namespace recup

int main(int,char **argv){
    return recup::ejecutar(argv[0],[](recup::Sesion &s){recup::menu(s);});
}
