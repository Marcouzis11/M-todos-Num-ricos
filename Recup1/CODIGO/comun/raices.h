#ifndef RECUP1_RAICES_H
#define RECUP1_RAICES_H
#include "base.h"
namespace recup {
enum MetodoRaiz {BISECCION, FALSA_POSICION, NEWTON, SECANTE, PUNTO_FIJO};
inline double derivadaNumerica(const std::string &f,double x){
    const double h=std::cbrt(std::numeric_limits<double>::epsilon())*std::max(1.0,std::fabs(x));
    return finito((evaluar(f,x+h)-evaluar(f,x-h))/(2*h));
}
inline double errorRel(double nuevo,double viejo,bool simetrico=false){
    double d=simetrico?std::fabs(nuevo/2+viejo/2):std::fabs(nuevo);
    if(d==0) return nuevo==viejo?0:std::numeric_limits<double>::infinity();
    return 100*(std::fabs(nuevo-viejo)/d);
}
inline void raiz(Sesion &s,MetodoRaiz metodo){
    const char *nombres[]={"BISECCION","REGULA FALSI (clasica)","NEWTON-RAPHSON","SECANTE","PUNTO FIJO"};
    const bool cerrado=metodo==BISECCION || metodo==FALSA_POSICION;
    const bool fijo=metodo==PUNTO_FIJO;
    std::cout<<"\n"<<nombres[metodo]<<"\n";
    std::string f=s.funcion(fijo?"g(x), despejada para iterar x_(k+1)=g(x_k)":"f(x), ecuacion f(x)=0");
    std::string df;
    if(metodo==NEWTON) df=s.funcion("f'(x) exacta [ENTER=derivada numerica central]",true);
    if(fijo) df=s.funcion("g'(x) [ENTER=estimacion numerica para diagnostico local]",true);
    double a=0,b=0,viejo=0,previo=0;
    if(cerrado){a=s.num("Extremo a");b=s.num("Extremo b");if(a>b)std::swap(a,b);if(a==b)throw std::runtime_error("El intervalo debe tener longitud positiva.");}
    else if(metodo==SECANTE){previo=s.num("x0");viejo=s.num("x1");}
    else viejo=s.num("x0");
    std::cout<<"\nCriterio de error:\n 1) |x_n - x_(n-1)| (absoluto)\n"
        <<" 2) 100*|x_n - x_(n-1)|/|x_n| (porcentual)\n"
        <<" 3) 100*|x_n - x_(n-1)|/(|x_n+x_(n-1)|/2) (IUA, porcentual)\n"
        <<" 4) Residuo |f(x_n)| (en punto fijo: |g(x_n)-x_n|)\n";
    if(metodo==BISECCION)std::cout<<" 5) Cota (b-a)/2 del intervalo ANTES de partirlo\n";
    int criterio=s.entero("Criterio",1,metodo==BISECCION?5:4,metodo==BISECCION?"5":"1");
    int modo=s.entero("Corte: 1=tolerancia, 2=cantidad fija de iteraciones",1,2,"1");
    double tol=modo==1?s.positivo("Tolerancia (si elegiste porcentaje, 0.1 significa 0.1%)","1e-6"):0;
    int maxit=s.entero(modo==1?"Maximo de iteraciones":"Cantidad de iteraciones",1,10000,modo==1?"100":"3");
    std::string ref=s.leer("Raiz de referencia para error verdadero [ENTER=sin referencia]: ");
    double exacto=0; if(!ref.empty()) exacto=Expresion(ref).evaluar(0,false,false);
    auto valor=[&](double x){double y=evaluar(f,x);return fijo?finito(y-x):y;};
    s.hoja<<nombres[metodo]<<"\n"<<(fijo?"g(x) = ":"f(x) = ")<<f<<'\n';
    const char *formulas[]={"x_n = a + (b-a)/2", "x_n = a - f(a)*(b-a)/(f(b)-f(a))",
        "x_n = x_(n-1) - f(x_(n-1))/f'(x_(n-1))",
        "x_n = x_(n-1) - f(x_(n-1))*(x_(n-1)-x_(n-2))/(f(x_(n-1))-f(x_(n-2)))",
        "x_n = g(x_(n-1))"};
    s.hoja<<"Formula: "<<formulas[metodo]<<'\n';
    const char *errores[]={"", "|x_n-x_(n-1)|", "100*|x_n-x_(n-1)|/|x_n| (%)",
        "100*|x_n-x_(n-1)|/(|x_n+x_(n-1)|/2) (%)", "residuo absoluto", "(b-a)/2 antes de actualizar el intervalo"};
    s.hoja<<"Criterio elegido E = "<<errores[criterio]<<'\n';
    if(modo==1)s.hoja<<"Corte: E <= "<<tol<<"; maximo "<<maxit<<" iteraciones.\n";
    else s.hoja<<"Se solicitan "<<maxit<<" iteraciones; no se afirma convergencia por cantidad de pasos.\n";
    if(!df.empty())s.hoja<<(fijo?"g'(x) = ":"f'(x) = ")<<df<<'\n';
    else if(metodo==NEWTON)s.hoja<<"f' aproximada por diferencia central; este resultado usa Newton con derivada numerica.\n";
    double fa=0,fb=0,raiz=viejo,fr=0,elegido=std::numeric_limits<double>::infinity();
    bool terminada=false,convergio=false,exacta=false; int kfin=0;
    std::string motivo;
    if(cerrado){
        fa=valor(a);fb=valor(b);raiz=a;fr=fa;
        s.hoja<<"Intervalo inicial ["<<a<<", "<<b<<"]; f(a)="<<fa<<"; f(b)="<<fb<<".\n"
              <<"Si f es continua y hay cambio de signo, Bolzano garantiza al menos una raiz.\n"
              <<"La continuidad debe justificarse con el dominio de f; el programa no la demuestra.\n";
        if(fa==0 || fb==0){raiz=fa==0?a:b;fr=0;exacta=true;terminada=true;convergio=true;motivo="Raiz en un extremo (evaluacion cero en aritmetica de maquina).";}
        else if(std::signbit(fa)==std::signbit(fb)) throw std::runtime_error("No hay cambio de signo. Este intervalo no garantiza una raiz por Bolzano; elegi otro.");
    }else{
        fr=valor(viejo);s.hoja<<"x inicial = "<<viejo<<"; residuo = "<<std::fabs(fr)<<'\n';
        if(metodo==SECANTE)s.hoja<<"x anterior = "<<previo<<"; f(x anterior) = "<<valor(previo)<<'\n';
        if(fr==0){raiz=viejo;exacta=true;terminada=true;convergio=true;motivo="El punto inicial satisface la ecuacion en aritmetica de maquina.";}
    }
    s.hoja<<"\nTABLA DE ITERACIONES (sin redondeo interno)\n";
    if(cerrado)s.hoja<<"n | a | b | f(a) | f(b) | x_n | f(x_n) | E_abs | E_rel% | E_IUA% | cota_biseccion\n";
    else if(metodo==SECANTE)s.hoja<<"n | x_(n-2) | x_(n-1) | f_anterior | f_actual | x_n | f(x_n) | E_abs | E_rel% | E_IUA%\n";
    else s.hoja<<"n | x_(n-1) | "<<(fijo?"g(x_(n-1))":"f(x_(n-1))")<<" | "<<(fijo?"g'(x_(n-1))":"f'(x_(n-1))")<<" | x_n | residuo | E_abs | E_rel% | E_IUA%\n";
    for(int k=1;k<=maxit && !terminada;++k){
        double xn=0,d=0,cota=0,fp=0,fv=0;
        if(cerrado){
            cota=finito(b/2-a/2);
            if(metodo==BISECCION)xn=finito(a/2+b/2);
            else {double den=finito(fb-fa); if(den==0){motivo="Denominador nulo en regula falsi.";break;} xn=finito(a-fa*(finito(b-a)/den));}
        }else if(fijo){xn=evaluar(f,viejo);try{d=df.empty()?derivadaNumerica(f,viejo):evaluar(df,viejo);}catch(const std::exception &){d=std::numeric_limits<double>::quiet_NaN();}}
        else if(metodo==NEWTON){
            fv=valor(viejo);d=df.empty()?derivadaNumerica(f,viejo):evaluar(df,viejo);
            if(d==0){motivo="Derivada nula: no se puede realizar el paso de Newton.";break;}
            xn=finito(viejo-fv/d);
        }else{
            fv=valor(viejo);fp=valor(previo);double den=finito(fv-fp);
            if(den==0){motivo="Denominador nulo en secante: f(x anterior)=f(x actual).";break;}
            xn=finito(viejo-fv*(finito(viejo-previo)/den));
        }
        double fn=valor(xn),ea=std::fabs(xn-viejo),er=errorRel(xn,viejo),ei=errorRel(xn,viejo,true);
        bool tieneAnterior=!cerrado || k>1;
        elegido=criterio==1?ea:criterio==2?er:criterio==3?ei:criterio==4?std::fabs(fn):cota;
        if(cerrado){
            s.hoja<<k<<" | "<<a<<" | "<<b<<" | "<<fa<<" | "<<fb<<" | "<<xn<<" | "<<fn<<" | ";
        }else if(metodo==SECANTE){s.hoja<<k<<" | "<<previo<<" | "<<viejo<<" | "<<fp<<" | "<<fv<<" | "<<xn<<" | "<<fn<<" | ";}
        else {s.hoja<<k<<" | "<<viejo<<" | "<<(fijo?evaluar(f,viejo):fv)<<" | "<<d<<" | "<<xn<<" | "<<fn<<" | ";}
        if(tieneAnterior)s.hoja<<ea<<" | "<<er<<" | "<<ei;
        else s.hoja<<"-- | -- | --";
        if(cerrado) {s.hoja<<" | ";if(metodo==BISECCION)s.hoja<<cota;else s.hoja<<"--";}
        s.hoja<<'\n';
        if(k==1){
            s.hoja<<"Primer reemplazo: ";
            if(metodo==BISECCION)s.hoja<<"("<<a<<" + "<<b<<")/2 = "<<xn;
            else if(metodo==FALSA_POSICION)s.hoja<<a<<" - ("<<fa<<")*("<<b<<"-"<<a<<")/("<<fb<<"-("<<fa<<")) = "<<xn;
            else if(metodo==NEWTON)s.hoja<<viejo<<" - ("<<fv<<")/("<<d<<") = "<<xn;
            else if(metodo==SECANTE)s.hoja<<viejo<<" - ("<<fv<<")*("<<viejo<<"-("<<previo<<"))/("<<fv<<"-("<<fp<<")) = "<<xn;
            else s.hoja<<"g("<<viejo<<") = "<<xn;
            s.hoja<<'\n';
        }
        raiz=xn;fr=fn;kfin=k;
        if(fn==0){exacta=true;convergio=true;terminada=true;motivo="Residuo cero en aritmetica de maquina.";}
        else if(modo==1 && (criterio>=4 || tieneAnterior) && elegido<=tol){convergio=true;terminada=true;motivo="Se alcanzo la tolerancia del criterio elegido.";}
        // No continuar miles de veces con el mismo valor por redondeo.
        else if((cerrado && (xn==a || xn==b)) || (tieneAnterior && xn==viejo)){
            terminada=true;motivo="Estancamiento por precision de maquina; no se confirma la tolerancia pedida.";
        }
        if(cerrado){if(std::signbit(fa)==std::signbit(fn)){a=xn;fa=fn;}else{b=xn;fb=fn;}}
        else if(metodo==SECANTE)previo=viejo;
        viejo=xn;
    }
    if(motivo.empty())motivo=modo==2?"Se completo la cantidad solicitada de iteraciones.":"Se alcanzo el maximo de iteraciones sin la tolerancia pedida.";
    s.hoja<<"\nESTADO: "<<motivo<<'\n';
    s.hoja<<(convergio?"RESULTADO: x = ":"ULTIMA APROXIMACION (sin convergencia certificada): x = ")<<raiz
          <<"\nIteraciones realizadas = "<<kfin<<"\nResiduo absoluto = "<<std::fabs(fr)<<'\n';
    if(exacta)s.hoja<<"El cero del residuo es una comprobacion numerica, sujeta al redondeo.\n";
    else if(kfin>0)s.hoja<<"Ultimo E elegido = "<<elegido<<"\n";
    if(cerrado)s.hoja<<"Intervalo final que conserva el cambio de signo: ["<<a<<", "<<b<<"].\n";
    if(criterio<=3)s.hoja<<"La diferencia entre iterados es un error aproximado, no una cota del error verdadero.\n";
    if(fijo){
        try{double d=df.empty()?derivadaNumerica(f,raiz):evaluar(df,raiz);
            s.hoja<<"|g'(ultima aproximacion)| = "<<std::fabs(d)<<" ("<<(df.empty()?"estimacion numerica":"derivada ingresada")<<").\n";}
        catch(const std::exception &){s.hoja<<"No se pudo evaluar g' en el ultimo punto.\n";}
        s.hoja<<"Para garantizar convergencia por contraccion se requiere un intervalo I con g(I) incluido en I\n"
              <<"y |g'(x)| <= q < 1 en TODO I. Un valor puntual no demuestra ni descarta esa condicion.\n";
    }
    if(!ref.empty())errorExacto(s.hoja,raiz,exacto);
    s.mostrar();
}
}
#endif
