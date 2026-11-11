#ifndef RECUP1_BASE_H
#define RECUP1_BASE_H
// C++11, sin dependencias externas. Entrada, expresiones, informes y Gauss.
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "consola.h"

namespace recup {
using Vec = std::vector<double>;
using Mat = std::vector<Vec>;
struct FinEntrada {};
inline double finito(double v) {
    if (!std::isfinite(v)) throw std::runtime_error("Resultado no finito: revisar dominio, division por cero o desborde.");
    return v;
}
inline std::string numero(double x) {
    std::ostringstream s; s << std::setprecision(17) << x; return s.str();
}
inline std::string fijo10(double x) {
    std::ostringstream s; s << std::fixed << std::setprecision(10) << x; return s.str();
}
inline std::string trim(std::string s) {
    const auto a=s.find_first_not_of(" \t\r\n");
    if(a==std::string::npos) return "";
    return s.substr(a,s.find_last_not_of(" \t\r\n")-a+1);
}
// Analizador: ^ asociativo a derecha, -x^2 = -(x^2), multiplicacion implicita.
// La validacion de sintaxis no evalua en un x arbitrario (sqrt(x-5) es valida).
class Expresion {
    std::string texto; size_t pos=0; double x=0; bool sintaxis=false, permiteX=true;
    void espacio(){while(pos<texto.size() && std::isspace(static_cast<unsigned char>(texto[pos]))) ++pos;}
    char ver(){espacio(); return pos<texto.size()?texto[pos]:'\0';}
    bool tomar(char c){if(ver()==c){++pos; return true;}return false;}
    double controlar(double v){return sintaxis?1:finito(v);}
    double primaria(){
        if(tomar('(')){double v=suma(); if(!tomar(')')) throw std::runtime_error("Falta cerrar un parentesis."); return v;}
        char c=ver();
        if(std::isdigit(static_cast<unsigned char>(c)) || c=='.'){
            const char *inicio=texto.c_str()+pos; char *fin=nullptr;
            double v=std::strtod(inicio,&fin);
            if(fin==inicio) throw std::runtime_error("Numero invalido.");
            pos=static_cast<size_t>(fin-texto.c_str()); return controlar(v);
        }
        if(std::isalpha(static_cast<unsigned char>(c))){
            std::string nombre;
            while(pos<texto.size() && std::isalnum(static_cast<unsigned char>(texto[pos])))
                nombre+=static_cast<char>(std::tolower(static_cast<unsigned char>(texto[pos++])));
            if(nombre=="x") {if(!permiteX) throw std::runtime_error("Aca hace falta un numero, sin x."); return x;}
            if(nombre=="pi") return std::acos(-1.0);
            if(nombre=="e") return std::exp(1.0);
            if(!tomar('(')) throw std::runtime_error("Funcion o variable desconocida: "+nombre);
            double a=suma(); if(!tomar(')')) throw std::runtime_error("Falta ')' en "+nombre);
            if(sintaxis) a=0.5;
            if(nombre=="sin" || nombre=="sen") return controlar(std::sin(a));
            if(nombre=="cos") return controlar(std::cos(a));
            if(nombre=="tan" || nombre=="tg") return controlar(std::tan(a));
            if(nombre=="asin" || nombre=="arcsin") return controlar(std::asin(a));
            if(nombre=="acos" || nombre=="arccos") return controlar(std::acos(a));
            if(nombre=="atan" || nombre=="arctan") return controlar(std::atan(a));
            if(nombre=="sinh") return controlar(std::sinh(a));
            if(nombre=="cosh") return controlar(std::cosh(a));
            if(nombre=="tanh") return controlar(std::tanh(a));
            if(nombre=="exp") return controlar(std::exp(a));
            if(nombre=="ln" || nombre=="log") return controlar(std::log(a));
            if(nombre=="log10") return controlar(std::log10(a));
            if(nombre=="cbrt") return controlar(std::cbrt(a));
            if(nombre=="sqrt" || nombre=="raiz") return controlar(std::sqrt(a));
            if(nombre=="abs") return controlar(std::fabs(a));
            throw std::runtime_error("Funcion desconocida: "+nombre);
        }
        throw std::runtime_error("Se esperaba un numero, x o una funcion.");
    }
    double potencia(){double a=primaria(); if(tomar('^')){double b=unario(); return controlar(sintaxis?1:std::pow(a,b));} return a;}
    double unario(){if(tomar('+')) return unario(); if(tomar('-')) return -unario(); return potencia();}
    double producto(){
        double a=unario();
        for(;;){
            if(tomar('*')) {double b=unario(); a=controlar(a*b);}
            else if(tomar('/')) {double b=unario(); if(!sintaxis && b==0) throw std::runtime_error("Division por cero."); a=controlar(sintaxis?1:a/b);}
            else if(ver()=='(' || std::isalpha(static_cast<unsigned char>(ver()))) a=controlar(a*unario());
            else return a;
        }
    }
    double suma(){double a=producto(); for(;;){if(tomar('+')) a=controlar(a+producto()); else if(tomar('-')) a=controlar(a-producto()); else return a;}}
public:
    explicit Expresion(std::string s):texto(std::move(s)){
        std::replace(texto.begin(),texto.end(),',','.');
        if(texto.size()>1024) throw std::runtime_error("Expresion demasiado larga (maximo 1024 caracteres).");
    }
    double evaluar(double valor=0, bool soloSintaxis=false, bool variable=true){
        pos=0; x=valor; sintaxis=soloSintaxis; permiteX=variable;
        double v=suma(); if(ver()) throw std::runtime_error("Sobran caracteres en la expresion: "+texto.substr(pos));
        return soloSintaxis?0:finito(v);
    }
};
inline double evaluar(const std::string &s,double x=0){return Expresion(s).evaluar(x);}

class Sesion {
    std::ofstream archivo;
public:
    std::string carpeta, ruta;
    std::ostringstream hoja;
    explicit Sesion(const char *ejecutable){
        std::string p=ejecutable; auto barra=p.find_last_of("/\\");
        carpeta=barra==std::string::npos?".":p.substr(0,barra);
        std::string nombre=barra==std::string::npos?p:p.substr(barra+1);
        if(nombre.size()>4 && nombre.substr(nombre.size()-4)==".exe") nombre.resize(nombre.size()-4);
        ruta=carpeta+"/salida_"+nombre+".txt";
        archivo.open(ruta.c_str(),std::ios::app);
        if(!archivo) std::cout<<"AVISO: no se pudo abrir "<<ruta<<". La salida sigue en consola.\n";
        else archivo<<"\n================ NUEVO EJERCICIO ================\n";
        hoja<<std::setprecision(15);
        const std::string nombres[][2]={
            {"Biseccion","BISECCION"},{"RegulaFalsi","REGULA FALSI"},
            {"Newton-Raphson","NEWTON-RAPHSON"},{"Secante","SECANTE"},{"Punto-Fijo","PUNTO FIJO"},
            {"Eliminacion-Gaussiana","ELIMINACION GAUSSIANA"},
            {"JacobiGaussSeidelRelax","JACOBI / GAUSS-SEIDEL / SOR"},
            {"interpolacion","INTERPOLACION"},{"Spline","SPLINES"},
            {"regresionLineal","REGRESION LINEAL"},{"polynomialRegression","REGRESION POLINOMICA Y OTROS AJUSTES"}};
        std::string nombreVisible=nombre;
        for(const auto &par:nombres)if(par[0]==nombre)nombreVisible=par[1];
        titulo(std::cout,"RECUPERATORIO 1 | "+nombreVisible);
        textoConsola(std::cout,
            "Funciones: x^3, exp(-x), ln(x), sqrt(x), sen(x), pi. Angulos en radianes.\n"
            "Decimales: punto o coma. Listas: valores separados por espacios.\n"
            "ENTER acepta el valor sugerido. Escribi salir para terminar.\n"
            "El desarrollo y los resultados se guardan junto al ejecutable.");
        titulo(std::cout,"DATOS DE ENTRADA",'-');
    }
    std::string leer(const std::string &mensaje){
        std::cout<<'\n';
        textoConsola(std::cout,opcionesConsola(mensaje));
        std::cout<<"  > "<<std::flush;
        std::string s; if(!std::getline(std::cin,s)) throw FinEntrada{};
        s=trim(s); if(s=="salir" || s=="SALIR") throw FinEntrada{};
        if(archivo) {archivo<<mensaje<<s<<'\n'; archivo.flush();}
        return s;
    }
    double num(const std::string &msg, const std::string &def=""){
        for(;;){
            std::string s=leer(msg+(def.empty()?"": " [ENTER="+def+"]")+": ");
            if(s.empty()) s=def;
            try{return Expresion(s).evaluar(0,false,false);}
            catch(const std::exception &e){std::cout<<"Entrada invalida: "<<e.what()<<'\n';}
        }
    }
    int entero(const std::string &msg,int lo,int hi,const std::string &def=""){
        for(;;){double v=num(msg,def); if(v>=lo && v<=hi && std::floor(v)==v) return static_cast<int>(v);
            std::cout<<"Escribi un entero entre "<<lo<<" y "<<hi<<".\n";}
    }
    double positivo(const std::string &msg,const std::string &def=""){
        for(;;){double v=num(msg,def); if(v>0) return v; std::cout<<"Debe ser mayor que cero.\n";}
    }
    std::string funcion(const std::string &msg,bool opcional=false){
        for(;;){std::string s=leer(msg+": "); if(s.empty() && opcional) return s;
            try {Expresion(s).evaluar(0,true); return s;}
            catch(const std::exception &e){std::cout<<"Expresion invalida: "<<e.what()<<'\n';}}
    }
    void mostrar(){
        std::ostringstream informe;
        titulo(informe,"TEXTO PARA COPIAR EN LA HOJA");
        informe<<hoja.str();
        titulo(informe,"FIN DEL TEXTO PARA COPIAR");
        const std::string b=informe.str();
        std::cout<<b; if(archivo){archivo<<b; archivo.flush(); std::cout<<"Guardado en: "<<ruta<<'\n';}
        hoja.str(""); hoja.clear();
    }
    void error(const std::string &m){std::cout<<"\nERROR: "<<m<<'\n'; if(archivo){archivo<<"ERROR: "<<m<<'\n'; archivo.flush();}}
};
inline int ejecutar(const char *argv0, const std::function<void(Sesion&)> &tarea){
    Sesion s(argv0);
    try {tarea(s); return 0;}
    catch(const FinEntrada&){std::cout<<"\nEntrada finalizada.\n"; return 0;}
    catch(const std::exception &e){
        if(!s.hoja.str().empty()) {s.hoja<<"\nCALCULO INTERRUMPIDO: "<<e.what()<<"\nNo hay un resultado final validado.\n";s.mostrar();}
        s.error(e.what()); return 1;
    }
}
inline Vec lista(const std::string &linea){
    std::string s=linea; for(char &c:s) if(c==';' || c=='|') c=' ';
    std::istringstream in(s); std::string t; Vec v;
    while(in>>t){v.push_back(Expresion(t).evaluar(0,false,false)); if(v.size()>101) throw std::runtime_error("Demasiados valores.");}
    return v;
}
inline Vec leerLista(Sesion &s,const std::string &msg,int cantidad=0){
    for(;;){try{Vec v=lista(s.leer(msg+": ")); if(!v.empty() && (!cantidad || static_cast<int>(v.size())==cantidad)) return v;
        std::cout<<"Cantidad incorrecta"<<(cantidad?"; se esperan "+std::to_string(cantidad):"")<<".\n";}
        catch(const std::exception &e){std::cout<<e.what()<<'\n';}}
}
inline Mat archivoFilas(const std::string &ruta){
    std::ifstream in(ruta.c_str()); if(!in) throw std::runtime_error("No se pudo abrir: "+ruta);
    Mat filas; std::string linea;
    while(std::getline(in,linea)){
        auto p=linea.find('#'); if(p!=std::string::npos) linea.resize(p);
        if(trim(linea).empty()) continue;
        Vec v=lista(linea); if(v.empty()) continue; filas.push_back(v);
        if(filas.size()>100) throw std::runtime_error("Maximo 100 filas por archivo.");
    }
    if(filas.empty()) throw std::runtime_error("Archivo sin datos: "+ruta);
    return filas;
}
inline std::string rutaDatos(Sesion &s){
    std::string r=s.leer("Ruta del archivo [ENTER=data.txt junto al ejecutable]: ");
    return r.empty()?s.carpeta+"/data.txt":r;
}
struct Puntos {Vec x,y;};
inline Puntos puntos(Sesion &s,bool distintos){
    Puntos p;
    int modo=s.entero("Datos: 1=pegar listas x e y, 2=archivo de filas x y",1,2,"1");
    if(modo==1){
        p.x=leerLista(s,"Valores de x separados por espacios (ej.: 0 1 2)");
        if(p.x.size()<2 || p.x.size()>100) throw std::runtime_error("Se necesitan de 2 a 100 puntos.");
        p.y=leerLista(s,"Valores de y en el mismo orden",static_cast<int>(p.x.size()));
    }else{
        Mat m=archivoFilas(rutaDatos(s));
        if(m.size()<2) throw std::runtime_error("Se necesitan al menos 2 puntos.");
        for(const Vec &f:m){if(f.size()!=2) throw std::runtime_error("Cada fila debe tener exactamente x y.");p.x.push_back(f[0]);p.y.push_back(f[1]);}
    }
    if(distintos){
        std::vector<size_t> orden(p.x.size()); for(size_t i=0;i<orden.size();++i) orden[i]=i;
        std::sort(orden.begin(),orden.end(),[&](size_t a,size_t b){return p.x[a]<p.x[b];});
        Puntos q; for(size_t i:orden){q.x.push_back(p.x[i]);q.y.push_back(p.y[i]);} p=q;
        for(size_t i=1;i<p.x.size();++i) if(p.x[i]==p.x[i-1]) throw std::runtime_error("Hay abscisas repetidas; no existe interpolacion unica con estos nodos.");
    }
    s.hoja<<"DATOS ("<<p.x.size()<<" puntos"<<(distintos?", ordenados por x":"")<<")\n";
    Tabla tabla({"i","x_i","y_i"});
    for(size_t i=0;i<p.x.size();++i)tabla.fila({i,p.x[i],p.y[i]});
    tabla.mostrar(s.hoja);
    return p;
}
inline void matriz(std::ostream &out,const Mat &a,const Vec &b){
    std::vector<std::string> columnas={"Fila"};
    for(size_t j=0;j<a[0].size();++j)columnas.push_back("a"+std::to_string(j+1));
    columnas.push_back("b");Tabla tabla(columnas);
    for(size_t i=0;i<a.size();++i){
        std::vector<Celda> fila={i+1};for(double v:a[i])fila.push_back(v);fila.push_back(b[i]);tabla.fila(fila);
    }
    tabla.mostrar(out);
}
struct Solucion {Vec x; long double det;};
// Pivoteo parcial escalado; el umbral se compara con la escala de cada fila.
// Se conservan A y b originales para verificar el residuo despues.
inline Solucion gauss(Mat a,Vec b,std::ostream *out=nullptr){
    const int n=static_cast<int>(a.size()); Vec escala(n),x(n); long double det=1;
    for(int i=0;i<n;++i){for(double v:a[i]) escala[i]=std::max(escala[i],std::fabs(v));
        if(escala[i]==0) throw std::runtime_error("Sistema singular: una fila de A es nula.");}
    if(out) {*out<<"Matriz aumentada inicial:\n";matriz(*out,a,b);}
    for(int k=0;k<n;++k){
        int p=k; for(int i=k+1;i<n;++i) if(std::fabs(a[i][k])/escala[i]>std::fabs(a[p][k])/escala[p]) p=i;
        if(std::fabs(a[p][k])/escala[p] <= 64*std::numeric_limits<double>::epsilon())
            throw std::runtime_error("Sistema singular o numericamente mal condicionado: sin solucion unica confiable.");
        if(p!=k){std::swap(a[p],a[k]);std::swap(b[p],b[k]);std::swap(escala[p],escala[k]);det=-det;
            if(out) *out<<"F"<<k+1<<" <-> F"<<p+1<<" (cambia el signo del determinante).\n";}
        det*=a[k][k];
        for(int i=k+1;i<n;++i){
            double m=a[i][k]/a[k][k];
            if(out) *out<<"m("<<i+1<<","<<k+1<<") = "<<a[i][k]<<"/"<<a[k][k]<<" = "<<m
                        <<"; F"<<i+1<<" <- F"<<i+1<<" - ("<<m<<") F"<<k+1<<'\n';
            a[i][k]=0; for(int j=k+1;j<n;++j) a[i][j]=finito(a[i][j]-m*a[k][j]); b[i]=finito(b[i]-m*b[k]);
        }
        if(out){*out<<"Luego del paso "<<k+1<<":\n";matriz(*out,a,b);}
    }
    if(out) *out<<"Retrosustitucion: x_i = (b_i - suma_(j>i) U_ij*x_j)/U_ii\n";
    for(int i=n-1;i>=0;--i){
        double suma=0; for(int j=i+1;j<n;++j) suma+=a[i][j]*x[j];
        x[i]=finito((b[i]-suma)/a[i][i]);
        if(out) *out<<"x"<<i+1<<" = ("<<b[i]<<" - ("<<suma<<"))/("<<a[i][i]<<") = "<<x[i]<<'\n';
    }
    return {x,det};
}
inline double residuo(const Mat &a,const Vec &b,const Vec &x){
    double r=0; for(size_t i=0;i<a.size();++i){double t=-b[i]; for(size_t j=0;j<x.size();++j) t+=a[i][j]*x[j]; r=std::max(r,std::fabs(finito(t)));} return r;
}
inline double poli(const Vec &c,double x){double y=0;for(auto i=c.rbegin();i!=c.rend();++i) y=finito(y*x+*i);return y;}
inline void polinomio(std::ostream &out,const Vec &c,const std::string &nombre="P(x)"){
    out<<nombre<<" = "<<c[0]; for(size_t j=1;j<c.size();++j) out<<" + ("<<c[j]<<")*x"<<(j>1?"^"+std::to_string(j):""); out<<'\n';
}
inline void errorExacto(std::ostream &out,double aprox,double exacto){
    double ea=finito(std::fabs(aprox-exacto));out<<"Valor de referencia = "<<exacto<<"\nError absoluto verdadero = |aproximado - referencia| = "<<ea<<'\n';
    if(exacto!=0) out<<"Error relativo verdadero porcentual = "<<finito(100*(ea/std::fabs(exacto)))<<" %\n";
    else out<<"Error relativo verdadero: no definido porque la referencia es cero.\n";
}
} // namespace recup
#endif
