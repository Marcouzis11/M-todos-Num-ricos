#ifndef RECUP1_SPLINES_H
#define RECUP1_SPLINES_H
#include "graficos.h"
namespace recup {
struct Spline {
    Vec x,a,b,c,d;
    int grado;
    int tramo(double q) const {
        if(q<x.front() || q>x.back())throw std::runtime_error("El x debe estar dentro del dominio del spline.");
        return std::min(static_cast<int>(x.size())-2,static_cast<int>(std::upper_bound(x.begin(),x.end(),q)-x.begin())-1);
    }
    double valor(int i,double q,int deriv=0) const {
        double t=q-x[i];
        if(deriv==1)return finito(b[i]+2*c[i]*t+3*d[i]*t*t);
        if(deriv==2)return finito(2*c[i]+6*d[i]*t);
        return finito(a[i]+t*(b[i]+t*(c[i]+t*d[i])));
    }
    double primitiva(int i,double q) const {double t=q-x[i];return finito(t*(a[i]+t*(b[i]/2+t*(c[i]/3+t*d[i]/4))));}
};
inline void splines(Sesion &s){
    Puntos p=puntos(s,true);int n=static_cast<int>(p.x.size())-1;
    int tipo=s.entero("Spline: 1=lineal, 2=cuadratico, 3=cubico natural (normal), 4=cubico sujeto (pendientes dadas)",1,4,"3");
    std::string f=s.funcion("f(x) de referencia [ENTER=solo tabla]",true);
    Spline sp{p.x,p.y,Vec(n,0),Vec(n,0),Vec(n,0),tipo<3?tipo:3};
    Vec h(n),delta(n);for(int i=0;i<n;++i){h[i]=finito(p.x[i+1]-p.x[i]);delta[i]=finito((p.y[i+1]-p.y[i])/h[i]);}
    const char *nombres[]={"","LINEAL","CUADRATICO","CUBICO NATURAL","CUBICO SUJETO"};
    s.hoja<<"\nSPLINE "<<nombres[tipo]<<"\nEn cada tramo: t=x-x_i; S_i(x)=a_i+b_i*t+c_i*t^2+d_i*t^3.\n"
          <<"Condiciones: S_i(x_i)=y_i, S_i(x_(i+1))=y_(i+1).\n";
    if(tipo>=2)s.hoja<<"Se exige continuidad de S' en los nodos interiores.\n";
    if(tipo>=3)s.hoja<<"Se exige continuidad de S'' en los nodos interiores.\n";
    s.hoja<<"Anchos y pendientes: h_i=x_(i+1)-x_i; delta_i=(y_(i+1)-y_i)/h_i.\n";
    Tabla pendientes({"i","h_i","delta_i"});
    for(int i=0;i<n;++i)pendientes.fila({i,h[i],delta[i]});
    pendientes.mostrar(s.hoja);
    if(tipo==1){
        sp.b=delta;s.hoja<<"a_i=y_i; b_i=delta_i; c_i=d_i=0.\nSolo se garantiza continuidad de S.\n";
    }else if(tipo==2){
        double c0=s.num("Condicion extra: c_0 (ENTER=0, primer tramo recto)","0");sp.c[0]=c0;
        sp.b[0]=finito(delta[0]-c0*h[0]);
        s.hoja<<"Condicion extra: c_0="<<c0<<" (S'' del primer tramo = "<<2*c0<<").\n"
              <<"b_0=delta_0-c_0*h_0; b_i=b_(i-1)+2*c_(i-1)*h_(i-1); c_i=(delta_i-b_i)/h_i.\n";
        for(int i=1;i<n;++i){sp.b[i]=finito(sp.b[i-1]+2*sp.c[i-1]*h[i-1]);sp.c[i]=finito((delta[i]-sp.b[i])/h[i]);}
    }else {
        double izq=0,der=0;
        if(tipo==4){izq=s.num("Pendiente S'(x_0)");der=s.num("Pendiente S'(x_n)");}
        Vec sub(n+1,0),diag(n+1,0),sup(n+1,0),rhs(n+1,0);
        s.hoja<<"Incognitas M_i=S''(x_i). En cada nodo interior:\n"
              <<"h_(i-1)*M_(i-1)+2*(h_(i-1)+h_i)*M_i+h_i*M_(i+1)=6*(delta_i-delta_(i-1)).\n";
        if(tipo==3){diag[0]=diag[n]=1;s.hoja<<"Borde NATURAL: M_0=M_n=0.\n";}
        else {
            diag[0]=2*h[0];sup[0]=h[0];rhs[0]=6*(delta[0]-izq);
            sub[n]=h[n-1];diag[n]=2*h[n-1];rhs[n]=6*(der-delta[n-1]);
            s.hoja<<"Borde SUJETO: S'(x0)="<<izq<<"; S'(xn)="<<der<<".\n"
                  <<"2*h0*M0+h0*M1=6*(delta0-S'(x0)); h_(n-1)*M_(n-1)+2*h_(n-1)*Mn=6*(S'(xn)-delta_(n-1)).\n";
        }
        for(int i=1;i<n;++i){sub[i]=h[i-1];diag[i]=2*(h[i-1]+h[i]);sup[i]=h[i];rhs[i]=6*(delta[i]-delta[i-1]);}
        s.hoja<<"\nSISTEMA TRIDIAGONAL\n";
        Tabla sistema({"Fila","subdiagonal","diagonal","superdiagonal","b"});
        for(int i=0;i<=n;++i)sistema.fila({i,sub[i],diag[i],sup[i],rhs[i]});
        sistema.mostrar(s.hoja);
        s.hoja<<"\nEliminacion de Thomas:\n";
        for(int i=1;i<=n;++i){if(diag[i-1]==0)throw std::runtime_error("Pivote nulo en Thomas.");
            double mult=sub[i]/diag[i-1];diag[i]=finito(diag[i]-mult*sup[i-1]);rhs[i]=finito(rhs[i]-mult*rhs[i-1]);
            s.hoja<<"Fila "<<i<<": multiplicador="<<mult<<"; diagonal nueva="<<diag[i]<<"; termino nuevo="<<rhs[i]<<'\n';}
        Vec m(n+1);if(diag[n]==0)throw std::runtime_error("Pivote final nulo en Thomas.");m[n]=finito(rhs[n]/diag[n]);
        for(int i=n-1;i>=0;--i)m[i]=finito((rhs[i]-sup[i]*m[i+1])/diag[i]);
        s.hoja<<"\nSolucion M_i=S''(x_i):\n";for(int i=0;i<=n;++i)s.hoja<<"M"<<i<<"="<<m[i]<<'\n';
        s.hoja<<"a_i=y_i; b_i=delta_i-h_i*(2*M_i+M_(i+1))/6; c_i=M_i/2; d_i=(M_(i+1)-M_i)/(6*h_i).\n";
        for(int i=0;i<n;++i){sp.b[i]=finito(delta[i]-h[i]*(2*m[i]+m[i+1])/6);sp.c[i]=m[i]/2;sp.d[i]=finito((m[i+1]-m[i])/(6*h[i]));}
        if(tipo==3)s.hoja<<"Control natural: S''(x0)="<<sp.valor(0,p.x[0],2)<<"; S''(xn)="<<sp.valor(n-1,p.x[n],2)<<'\n';
        else s.hoja<<"Control sujeto: S'(x0)="<<sp.valor(0,p.x[0],1)<<" (pedido "<<izq<<"); S'(xn)="<<sp.valor(n-1,p.x[n],1)<<" (pedido "<<der<<")\n";
    }
    s.hoja<<"\nRESULTADO: COEFICIENTES POR TRAMO\n";
    Tabla coeficientes({"i","a_i","b_i","c_i","d_i"});
    for(int i=0;i<n;++i)coeficientes.fila({i,sp.a[i],sp.b[i],sp.c[i],sp.d[i]});
    coeficientes.mostrar(s.hoja);
    s.hoja<<"\nPOLINOMIOS E INTERVALOS\n";
    for(int i=0;i<n;++i){
        s.hoja<<"Tramo "<<i<<": "<<p.x[i]<<" <= x <= "<<p.x[i+1]<<'\n';
        s.hoja<<"S"<<i<<"(x) = "<<sp.a[i]<<" + ("<<sp.b[i]<<")*(x-("<<p.x[i]<<")) + ("<<sp.c[i]<<")*(x-("<<p.x[i]<<"))^2 + ("<<sp.d[i]<<")*(x-("<<p.x[i]<<"))^3\n";
        double xi=p.x[i];Vec c={finito(sp.a[i]-sp.b[i]*xi+sp.c[i]*xi*xi-sp.d[i]*xi*xi*xi),finito(sp.b[i]-2*sp.c[i]*xi+3*sp.d[i]*xi*xi),finito(sp.c[i]-3*sp.d[i]*xi),sp.d[i]};
        polinomio(s.hoja,c,"Forma expandida S"+std::to_string(i)+"(x)");
    }
    s.hoja<<"\nCONTROL DE INTERPOLACION\n";
    Tabla control({"tramo","S_i(x_i)-y_i","S_i(x_(i+1))-y_(i+1)"});
    for(int i=0;i<n;++i)control.fila({i,sp.valor(i,p.x[i])-p.y[i],sp.valor(i,p.x[i+1])-p.y[i+1]});
    control.mostrar(s.hoja);
    s.hoja<<"\nCONTROL DE CONTINUIDAD (izquierda - derecha) EN NODOS INTERIORES\n";
    Tabla continuidad({"i","salto S","salto S'","salto S''"});
    for(int i=1;i<n;++i)continuidad.fila({i,sp.valor(i-1,p.x[i])-sp.valor(i,p.x[i]),sp.valor(i-1,p.x[i],1)-sp.valor(i,p.x[i],1),sp.valor(i-1,p.x[i],2)-sp.valor(i,p.x[i],2)});
    if(n>1)continuidad.mostrar(s.hoja);
    else s.hoja<<"No hay nodos interiores: el spline tiene un unico tramo.\n";
    s.hoja<<"El spline lineal solo exige salto S=0; el cuadratico tambien salto S'=0; el cubico tambien salto S''=0.\n";
    s.mostrar();
    for(;;){
        int op=s.entero("1=evaluar spline y derivadas, 2=tabla equiespaciada, 3=integral por tramos, 4=grafico SVG/CSV, 0=salir",0,4,"1");if(!op)break;
        if(op==1){double q=s.num("x a evaluar");int i=sp.tramo(q);double v=sp.valor(i,q);
            s.hoja<<"EVALUACION DEL SPLINE "<<nombres[tipo]<<"\n"<<p.x[i]<<" <= "<<q<<" <= "<<p.x[i+1]<<"; corresponde el tramo "<<i<<".\n"
                  <<"t="<<q<<"-("<<p.x[i]<<")="<<q-p.x[i]<<"\nS(x)="<<sp.a[i]<<"+t*("<<sp.b[i]<<"+t*("<<sp.c[i]<<"+t*("<<sp.d[i]<<")))\n"
                  <<"RESULTADO: S("<<q<<") = "<<v<<"\nS'("<<q<<") = "<<sp.valor(i,q,1)<<"\nS''("<<q<<") = "<<sp.valor(i,q,2)<<'\n';
            if((tipo==1 || tipo==2) && i>0 && q==p.x[i])s.hoja<<"En un nodo interior se informa la derivada del tramo derecho; si los limites laterales difieren, esa derivada no existe en el nodo.\n";
            if(!f.empty())errorExacto(s.hoja,v,evaluar(f,q));
        }else if(op==2){
            int modo=s.entero("Tabla: 1=dar cantidad de subintervalos, 2=dar paso h",1,2,"1");int cantidad;double paso;
            double ancho=p.x.back()-p.x.front();
            if(modo==1){cantidad=s.entero("Cantidad de subintervalos",1,1000,std::to_string(n));paso=ancho/cantidad;}
            else {paso=s.positivo("Paso h");double coc=ancho/paso;
                if(coc<1 || coc>1000 || std::fabs(coc-std::round(coc))>1e-9*std::max(1.0,std::fabs(coc)))
                    throw std::runtime_error("h debe dividir el intervalo en una cantidad entera de 1 a 1000 subintervalos. No se modifica h silenciosamente.");
                cantidad=static_cast<int>(std::round(coc));}
            s.hoja<<"TABLA EQUIESPACIADA DEL SPLINE\nIntervalo ["<<p.x.front()<<", "<<p.x.back()<<"]; h="<<paso<<"; subintervalos="<<cantidad<<'\n';
            std::vector<std::string> columnas={"i","x_i","S(x_i)","tramo"};
            if(!f.empty()){columnas.push_back("f(x_i)");columnas.push_back("error absoluto");}
            Tabla tabla(columnas);Vec xs,ys;
            for(int j=0;j<=cantidad;++j){double q=j==cantidad?p.x.back():p.x.front()+j*paso;int i=sp.tramo(q);double v=sp.valor(i,q);xs.push_back(q);ys.push_back(v);
                std::vector<Celda> fila={j,q,v,i};if(!f.empty()){double exacto=evaluar(f,q);fila.push_back(exacto);fila.push_back(std::fabs(v-exacto));}tabla.fila(fila);}
            tabla.mostrar(s.hoja);
            s.hoja<<"\nLISTAS PARA PEGAR EN OTRO PROGRAMA\nx:\n";for(double q:xs)s.hoja<<q<<' ';s.hoja<<"\ny:\n";for(double v:ys)s.hoja<<v<<' ';s.hoja<<'\n';
        }else if(op==4){
            graficoFuncion(s,[&](double q){return sp.valor(sp.tramo(q),q);},"spline","Spline",&p,f);
            continue;
        }else {
            double a=s.num("Limite inferior",numero(p.x.front())),b=s.num("Limite superior",numero(p.x.back()));
            if(a>b)throw std::runtime_error("Se requiere limite inferior <= superior.");
            sp.tramo(a);sp.tramo(b);
            s.hoja<<"INTEGRAL DEL SPLINE entre "<<a<<" y "<<b<<"\nF_i(t)=a_i*t+b_i*t^2/2+c_i*t^3/3+d_i*t^4/4.\n";double total=0;
            for(int i=0;i<n;++i){double izq=std::max(a,p.x[i]),der=std::min(b,p.x[i+1]);if(izq<der){double v=finito(sp.primitiva(i,der)-sp.primitiva(i,izq));total=finito(total+v);s.hoja<<"Tramo "<<i<<" ["<<izq<<", "<<der<<"]: F_i(der)-F_i(izq) = "<<v<<'\n';}}
            s.hoja<<"RESULTADO: integral del spline = "<<total<<"\nEsta integral es la del spline construido, no la de una funcion de referencia.\n";
        }
        s.mostrar();
    }
}
}
#endif
