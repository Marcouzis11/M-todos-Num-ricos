#ifndef RECUP1_CONSOLA_H
#define RECUP1_CONSOLA_H
// Presentacion compartida. No modifica valores ni precision de los calculos.
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#ifndef _WIN32
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace recup {
inline size_t anchoConsola(){
    const char *config=std::getenv("RECUP_ANCHO");
    if(!config)config=std::getenv("COLUMNS");
    if(config){
        char *fin=nullptr;long n=std::strtol(config,&fin,10);
        if(fin!=config && *fin=='\0' && n>=40 && n<=240)return static_cast<size_t>(n);
    }
#ifndef _WIN32
    struct winsize tam = {};
    if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&tam)==0 && tam.ws_col>=40)
        return std::min<size_t>(tam.ws_col,240);
#endif
    return 100;
}
inline void textoConsola(std::ostream &out,const std::string &texto,size_t ancho=anchoConsola()){
    std::istringstream lineas(texto);std::string linea;
    while(std::getline(lineas,linea)){
        // Cortar solo en espacios: nunca partir numeros, rutas ni expresiones.
        while(linea.size()>ancho){
            size_t corte=linea.rfind(' ',ancho);
            if(corte==std::string::npos || corte==0)break;
            out<<linea.substr(0,corte)<<'\n';
            linea.erase(0,corte+1);
        }
        out<<linea<<'\n';
    }
}
inline void titulo(std::ostream &out,const std::string &nombre,char borde='='){
    out<<'\n'<<std::string(anchoConsola(),borde)<<'\n';
    textoConsola(out,nombre);
    out<<std::string(anchoConsola(),borde)<<'\n';
}
struct Celda {
    std::string texto;
    Celda(const char *v):texto(v){}
    Celda(const std::string &v):texto(v){}
    template<class T> Celda(T v){
        std::ostringstream out;out<<std::setprecision(15)<<v;texto=out.str();
    }
};
// Las tablas anchas se separan en paneles, repitiendo la primera columna.
// Se conservan las 15 cifras significativas del informe y todos los registros.
class Tabla {
    std::vector<std::string> encabezados;
    std::vector<std::vector<Celda>> filas;
public:
    explicit Tabla(const std::vector<std::string> &cabecera):encabezados(cabecera){
        if(encabezados.empty())throw std::logic_error("Tabla sin columnas.");
    }
    void fila(const std::vector<Celda> &valores){
        if(valores.size()>encabezados.size())throw std::logic_error("Tabla: sobran columnas.");
        filas.push_back(valores);
        while(filas.back().size()<encabezados.size())filas.back().push_back("--");
    }
    void mostrar(std::ostream &out) const {
        std::vector<size_t> anchos;
        for(const auto &e:encabezados)anchos.push_back(e.size());
        for(const auto &f:filas)for(size_t j=0;j<f.size();++j)
            anchos[j]=std::max(anchos[j],f[j].texto.size());
        std::vector<std::vector<size_t>> paneles(1,std::vector<size_t>(1,0));
        size_t usado=anchos[0]+4;
        for(size_t j=1;j<encabezados.size();++j){
            if(paneles.back().size()>1 && usado+anchos[j]+3>anchoConsola()){
                paneles.push_back(std::vector<size_t>(1,0));usado=anchos[0]+4;
            }
            paneles.back().push_back(j);usado+=anchos[j]+3;
        }
        for(size_t p=0;p<paneles.size();++p){
            if(paneles.size()>1)out<<"\nColumnas - bloque "<<p+1<<" de "<<paneles.size()<<" ("<<encabezados[0]<<" identifica la fila)\n";
            const auto &cols=paneles[p];
            auto borde=[&](){out<<'+';for(size_t j:cols)out<<std::string(anchos[j]+2,'-')<<'+';out<<'\n';};
            auto linea=[&](const std::vector<std::string> &celdas,bool cabecera){
                out<<'|';for(size_t j:cols){
                    const std::string &v=celdas[j];out<<' ';
                    if(!cabecera)out<<std::string(anchos[j]-v.size(),' ');
                    out<<v;
                    if(cabecera)out<<std::string(anchos[j]-v.size(),' ');
                    out<<" |";
                }out<<'\n';
            };
            borde();linea(encabezados,true);borde();
            for(const auto &f:filas){std::vector<std::string> c;for(const auto &v:f)c.push_back(v.texto);linea(c,false);}
            if(filas.empty())out<<"(sin filas: no se realizaron pasos)\n";
            borde();
        }
    }
};
inline std::string opcionesConsola(const std::string &mensaje){
    // Las preguntas conservan su texto en el registro; las opciones se leen en vertical.
    std::string resultado;
    for(size_t i=0;i<mensaje.size();++i){
        if(mensaje[i]==',' && i+2<mensaje.size() && mensaje[i+1]==' '){
            size_t j=i+2;while(j<mensaje.size() && mensaje[j]>='0' && mensaje[j]<='9')++j;
            if(j>i+2 && j<mensaje.size() && mensaje[j]=='='){
                resultado+="\n  ";++i;continue;
            }
        }
        resultado+=mensaje[i];
    }
    return resultado;
}
} // namespace recup
#endif
