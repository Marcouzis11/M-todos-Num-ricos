#ifndef RECUP1_GRAFICOS_H
#define RECUP1_GRAFICOS_H
#include "base.h"
namespace recup {
inline std::string xml(const std::string &s){
    std::string r;for(char c:s){if(c=='&')r+="&amp;";else if(c=='<')r+="&lt;";
        else if(c=='>')r+="&gt;";else if(c=='\"')r+="&quot;";else r+=c;}return r;
}
inline void escribirArchivo(const std::string &ruta,const std::string &texto){
    std::ofstream out(ruta.c_str(),std::ios::binary);if(!out)throw std::runtime_error("No se pudo crear: "+ruta);
    out<<texto;if(!out)throw std::runtime_error("No se pudo completar: "+ruta);
}
inline std::string rutaGrafico(Sesion &s,const std::string &tipo){
    for(int i=1;i<=10000;++i){std::string r=s.carpeta+"/grafico_"+tipo+"_"+std::to_string(i);
        if(!std::ifstream((r+".svg").c_str()).good() && !std::ifstream((r+".csv").c_str()).good())return r;}
    throw std::runtime_error("No quedan nombres libres para graficos.");
}
inline std::string svgCurvas(const Vec &xs,const std::vector<Vec> &series,
                             const std::vector<std::string> &etiquetas,const std::string &titulo,
                             const Puntos *datos=nullptr){
    double ymin=std::numeric_limits<double>::infinity(),ymax=-ymin;
    for(const Vec &ys:series)for(double y:ys)if(std::isfinite(y)){ymin=std::min(ymin,y);ymax=std::max(ymax,y);}
    if(datos)for(size_t i=0;i<datos->x.size();++i)if(datos->x[i]>=xs.front() && datos->x[i]<=xs.back()){
        ymin=std::min(ymin,datos->y[i]);ymax=std::max(ymax,datos->y[i]);}
    if(!std::isfinite(ymin))throw std::runtime_error("No hay valores validos en el intervalo del grafico.");
    long double lo=ymin,hi=ymax;
    long double margen=hi==lo?std::max(1.0L,std::fabs(lo)*.05L):(hi-lo)*.07L;
    lo-=margen;hi+=margen;
    auto px=[&](double x){return 90+780*static_cast<double>((static_cast<long double>(x)-xs.front())/(static_cast<long double>(xs.back())-xs.front()));};
    auto py=[&](double y){return 430-340*static_cast<double>((static_cast<long double>(y)-lo)/(hi-lo));};
    const char *colores[]={"#1769aa","#df6b16","#7d3c98"};
    std::ostringstream out;out<<std::setprecision(12);
    out<<"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"960\" height=\"540\" viewBox=\"0 0 960 540\">\n"
       <<"<rect width=\"960\" height=\"540\" fill=\"white\"/><g font-family=\"Arial,sans-serif\" fill=\"#202b36\">"
       <<"<text x=\"90\" y=\"32\" font-size=\"19\">"<<xml(titulo)<<"</text>\n";
    for(int i=0;i<=5;++i){double x=xs.front()*(1-i/5.0)+xs.back()*(i/5.0);
        long double y=lo+(hi-lo)*i/5;double yy=430-340*i/5.0;
        out<<"<path d=\"M90 "<<yy<<" H870\" stroke=\"#e3e8ee\"/>"
           <<"<text x=\"8\" y=\""<<yy+4<<"\" font-size=\"11\">"<<std::setprecision(5)<<y<<std::setprecision(12)<<"</text>"
           <<"<text x=\""<<px(x)-16<<"\" y=\"453\" font-size=\"11\">"<<std::setprecision(5)<<x<<std::setprecision(12)<<"</text>\n";}
    out<<"<path d=\"M90 80 V430 H880\" fill=\"none\" stroke=\"#202b36\"/>"
       <<"<text x=\"890\" y=\"435\">x</text>";
    for(size_t j=0;j<series.size();++j){bool abierto=false;
        out<<"<path fill=\"none\" stroke=\""<<colores[j%3]<<"\" stroke-width=\"2\" d=\"";
        for(size_t i=0;i<xs.size();++i){double y=series[j][i];if(!std::isfinite(y)){abierto=false;continue;}
            out<<(abierto?" L":" M")<<px(xs[i])<<' '<<py(y);abierto=true;}
        out<<"\"/>";
        out<<"<text x=\""<<90+j*260<<"\" y=\"480\" font-size=\"13\" fill=\""<<colores[j%3]<<"\">"<<xml(etiquetas[j])<<"</text>\n";
    }
    if(datos)for(size_t i=0;i<datos->x.size();++i)if(datos->x[i]>=xs.front() && datos->x[i]<=xs.back())
        out<<"<circle cx=\""<<px(datos->x[i])<<"\" cy=\""<<py(datos->y[i])<<"\" r=\"3\" fill=\"#202b36\"/>";
    out<<"<text x=\"90\" y=\"515\" font-size=\"11\">Curvas muestreadas. Los huecos indican evaluaciones fuera de dominio.</text></g></svg>\n";
    return out.str();
}
inline void graficoFuncion(Sesion &s,const std::function<double(double)> &modelo,
                          const std::string &tipo,const std::string &titulo,
                          const Puntos *datos=nullptr,const std::string &ref=""){
    double a=s.num("Inicio del grafico",datos?numero(*std::min_element(datos->x.begin(),datos->x.end())):"1");
    double b=s.num("Fin del grafico",datos?numero(*std::max_element(datos->x.begin(),datos->x.end())):"2");
    if(!(a<b))throw std::runtime_error("Grafico: se requiere inicio < fin.");
    int n=s.entero("Cantidad de subintervalos para graficar",2,10000,"400");
    Vec xs,ys,real,errores;int invalidos=0;
    for(int i=0;i<=n;++i){double x=i==n?b:finito(a*(1-i/static_cast<double>(n))+b*(i/static_cast<double>(n)));
        double y=std::numeric_limits<double>::quiet_NaN(),r=y,e=y;
        try{y=finito(modelo(x));}catch(const std::exception &){}
        if(!ref.empty()){try{r=evaluar(ref,x);if(std::isfinite(y))e=finito(std::fabs(y-r));}catch(const std::exception &){}}
        if(!std::isfinite(y) || (!ref.empty() && !std::isfinite(e)))++invalidos;
        xs.push_back(x);ys.push_back(y);real.push_back(r);errores.push_back(e);
    }
    std::string ruta=rutaGrafico(s,tipo);
    std::vector<Vec> series{ys};std::vector<std::string> nombres{"Modelo"};
    if(!ref.empty()){series.push_back(real);nombres.push_back("Funcion de referencia");}
    std::string svg=svgCurvas(xs,series,nombres,titulo,datos);
    std::ostringstream csv;csv<<std::setprecision(17)<<"x,modelo";
    if(!ref.empty())csv<<",referencia,error_absoluto";
    csv<<'\n';
    for(size_t i=0;i<xs.size();++i){csv<<xs[i]<<',';if(std::isfinite(ys[i]))csv<<ys[i];
        if(!ref.empty()){csv<<',';if(std::isfinite(real[i]))csv<<real[i];csv<<',';if(std::isfinite(errores[i]))csv<<errores[i];}csv<<'\n';}
    // Preparar todas las curvas antes de escribir evita entregar una curva incompleta por dominio invalido.
    std::string errorSvg;if(!ref.empty())errorSvg=svgCurvas(xs,{errores},{"|modelo - referencia|"},"Error absoluto");
    escribirArchivo(ruta+".svg",svg);escribirArchivo(ruta+".csv",csv.str());
    s.hoja<<"GRAFICO: "<<ruta<<".svg\nTABLA CSV: "<<ruta<<".csv\n";
    if(!ref.empty()){escribirArchivo(ruta+"_error.svg",errorSvg);s.hoja<<"GRAFICO DEL ERROR: "<<ruta<<"_error.svg\n";}
    s.hoja<<"Muestras con evaluaciones invalidas: "<<invalidos<<". La grafica no demuestra continuidad entre muestras.\n";
    s.mostrar();
}
inline void raizModelo(Sesion &s,const std::function<double(double)> &modelo){
    double a=s.num("Extremo a para buscar raiz del modelo"),b=s.num("Extremo b para buscar raiz del modelo");
    if(a>b)std::swap(a,b);
    if(a==b)throw std::runtime_error("Intervalo de raiz sin longitud.");
    double tol=s.positivo("Tolerancia de cota de raiz","1e-10"),tr=s.positivo("Tolerancia de residuo del modelo","1e-10");
    int maxit=s.entero("Maximo de iteraciones",1,10000,"200");
    double fa=finito(modelo(a)),fb=finito(modelo(b)),x=a,fx=fa;int k=0;
    bool ok=fa==0 || fb==0;if(fb==0){x=b;fx=fb;}
    if(!ok && std::signbit(fa)==std::signbit(fb))throw std::runtime_error("El modelo no cambia de signo en los extremos elegidos.");
    s.hoja<<"RAIZ DEL MODELO POR BISECCION\nRequiere continuidad en el intervalo elegido.\nk | x | cota | residuo\n";
    for(int i=1;i<=maxit && !ok;++i){x=a/2+b/2;fx=finito(modelo(x));double cota=b/2-a/2;k=i;
        s.hoja<<i<<" | "<<x<<" | "<<cota<<" | "<<std::fabs(fx)<<'\n';
        ok=fx==0 || (cota<tol && std::fabs(fx)<tr);
        if(ok || x==a || x==b)break;
        if(std::signbit(fx)==std::signbit(fa)){a=x;fa=fx;}else{b=x;fb=fx;}
    }
    s.hoja<<(ok?"RESULTADO: raiz del modelo = ":"ULTIMA APROXIMACION sin tolerancia confirmada = ")<<x
          <<"\nA diez decimales: "<<fijo10(x)<<"\nIteraciones = "<<k<<"\nResiduo del modelo = "<<std::fabs(fx)<<'\n';
    std::string referencia=s.leer("Raiz real de referencia para error [ENTER=sin referencia]: ");
    if(!referencia.empty())errorExacto(s.hoja,x,Expresion(referencia).evaluar(0,false,false));
    s.mostrar();
}
}
#endif
