#ifndef RECUP1_INTERPOLACION_H
#define RECUP1_INTERPOLACION_H
#include "graficos.h"
namespace recup {
// Multiplica un polinomio en potencias ascendentes por (x-r).
inline Vec porBinomio(const Vec &c,double r){
    Vec d(c.size()+1,0);for(size_t i=0;i<c.size();++i){d[i]=finito(d[i]-r*c[i]);d[i+1]=finito(d[i+1]+c[i]);}return d;
}
inline Vec coefLagrange(const Puntos &p,std::ostream &out){
    size_t n=p.x.size();Vec c(n,0);
    out<<"L_i(x) = producto_(j!=i) (x-x_j)/(x_i-x_j); P(x) = suma y_i*L_i(x).\n";
    for(size_t i=0;i<n;++i){
        Vec base(1,1);double den=1;
        out<<"L"<<i<<"(x) = ";
        for(size_t j=0;j<n;++j)if(i!=j){out<<"(x-("<<p.x[j]<<"))";base=porBinomio(base,p.x[j]);den=finito(den*(p.x[i]-p.x[j]));}
        if(den==0)throw std::runtime_error("Denominador de Lagrange nulo o demasiado pequeno.");
        out<<" / ("<<den<<")\n";
        for(size_t j=0;j<n;++j)c[j]=finito(c[j]+p.y[i]*(base[j]/den));
    }
    return c;
}
inline Vec coefNewton(const Puntos &p,std::ostream &out){
    int n=static_cast<int>(p.x.size());Mat d(n,Vec(n,0));for(int i=0;i<n;++i)d[i][0]=p.y[i];
    out<<"Diferencias divididas: D[i,j]=(D[i+1,j-1]-D[i,j-1])/(x_(i+j)-x_i).\n";
    for(int j=1;j<n;++j)for(int i=0;i<n-j;++i)d[i][j]=finito((d[i+1][j-1]-d[i][j-1])/(p.x[i+j]-p.x[i]));
    std::vector<std::string> columnas={"i","x_i"};
    for(int j=0;j<n;++j)columnas.push_back("D[i,"+std::to_string(j)+"]");
    Tabla tabla(columnas);
    for(int i=0;i<n;++i){std::vector<Celda> fila={i,p.x[i]};for(int j=0;j<n-i;++j)fila.push_back(d[i][j]);tabla.fila(fila);}
    tabla.mostrar(out);out<<"--: diferencia dividida no necesaria para esta fila.\n";
    out<<"Forma de Newton:\nP(x) = "<<d[0][0];
    Vec c(n,0),base(1,1);
    for(int j=0;j<n;++j){
        if(j){out<<" + ("<<d[0][j]<<")";for(int k=0;k<j;++k)out<<"*(x-("<<p.x[k]<<"))";}
        for(size_t k=0;k<base.size();++k)c[k]=finito(c[k]+d[0][j]*base[k]);
        if(j+1<n)base=porBinomio(base,p.x[j]);
    }
    out<<'\n';return c;
}
// Evaluacion baricentrica: evita expandir el polinomio para obtener P(q).
inline double lagrangeValor(const Puntos &p,double q){
    long double numerador=0,denominador=0;
    for(size_t i=0;i<p.x.size();++i){if(q==p.x[i])return p.y[i];
        long double w=1;for(size_t j=0;j<p.x.size();++j)if(i!=j)w/=static_cast<long double>(p.x[i])-p.x[j];
        long double t=w/(static_cast<long double>(q)-p.x[i]);numerador+=t*p.y[i];denominador+=t;}
    return finito(static_cast<double>(numerador/denominador));
}
inline void interpolacion(Sesion &s,const Puntos *original=nullptr,int seleccionado=0){
    Puntos p=puntos(s,true,original);int n=static_cast<int>(p.x.size());
    if(n>20)throw std::runtime_error("Para el polinomio unico usa hasta 20 puntos; para mas puntos usa Spline.cpp.");
    int metodo=seleccionado?seleccionado:s.entero("Metodo: 1=Lagrange, 2=polinomica (Vandermonde + Gauss), 3=Newton (diferencias divididas)",1,3,"1");
    std::string f=s.funcion("f(x) de referencia [ENTER=solo datos, sin error verdadero]",true);
    std::string cotaTxt=s.leer("Cota M de |f^(n)(x)| (n=cantidad de puntos) [ENTER=sin cota teorica]: ");
    double m=0;if(!cotaTxt.empty()){m=Expresion(cotaTxt).evaluar(0,false,false);if(m<0)throw std::runtime_error("La cota M no puede ser negativa.");}
    const char *nombres[]={"","LAGRANGE","POLINOMICA (VANDERMONDE)","NEWTON CON DIFERENCIAS DIVIDIDAS"};
    s.hoja<<"\nINTERPOLACION: "<<nombres[metodo]<<"\nCon "<<n<<" nodos distintos hay un unico polinomio de grado <= "<<n-1<<".\n";
    Vec c;
    if(metodo==1)c=coefLagrange(p,s.hoja);
    else if(metodo==3)c=coefNewton(p,s.hoja);
    else {
        Mat a(n,Vec(n,1));for(int i=0;i<n;++i)for(int j=1;j<n;++j)a[i][j]=finito(a[i][j-1]*p.x[i]);
        s.hoja<<"P(x)=a0+a1*x+...; se impone P(x_i)=y_i.\nV[i,j]=x_i^j; V*a=y.\n";
        c=gauss(a,p.y,&s.hoja).x;
    }
    s.hoja<<"\nRESULTADO: POLINOMIO EXPANDIDO\n";polinomio(s.hoja,c);
    s.hoja<<"\nCONTROL DE LOS COEFICIENTES EXPANDIDOS EN LOS NODOS\n";
    Tabla control({"i","x_i","P(x_i)","y_i","diferencia"});
    double maxerr=0,yscale=0;
    for(int i=0;i<n;++i){double v=poli(c,p.x[i]);maxerr=std::max(maxerr,std::fabs(v-p.y[i]));yscale=std::max(yscale,std::fabs(p.y[i]));
        control.fila({i,p.x[i],v,p.y[i],v-p.y[i]});}
    control.mostrar(s.hoja);
    s.hoja<<"Maximo error en nodos de la forma expandida = "<<maxerr<<'\n';
    if(maxerr>1e-8*std::max(1.0,yscale))s.hoja<<"AVISO: la expansion tiene perdida de precision. Usar la forma factorizada/tabla y revisar escala o grado.\n";
    s.mostrar();
    auto consulta=[&](double q){
        double v=lagrangeValor(p,q);
        s.hoja<<"EVALUACION DEL POLINOMIO INTERPOLANTE EN x="<<q<<'\n';
        if(q<p.x.front() || q>p.x.back())s.hoja<<"El punto queda fuera de los datos: es EXTRAPOLACION.\n";
        if(metodo==1){
            s.hoja<<"num_i=producto(q-x_j); den_i=producto(x_i-x_j), para j distinto de i.\n";
            Tabla aportes({"i","num_i","den_i","L_i(q)","y_i*L_i(q)"});
            for(int i=0;i<n;++i){double num=1,den=1;for(int j=0;j<n;++j)if(i!=j){num=finito(num*(q-p.x[j]));den=finito(den*(p.x[i]-p.x[j]));}
                double li=finito(num/den);aportes.fila({i,num,den,li,finito(p.y[i]*li)});}
            aportes.mostrar(s.hoja);
        }
        if(metodo==2){
            s.hoja<<"Reemplazo en el polinomio expandido:\nP("<<q<<") = "<<c[0];
            for(int j=1;j<n;++j)s.hoja<<" + ("<<c[j]<<")*("<<q<<")^"<<j;
            s.hoja<<" = "<<poli(c,q)<<'\n';
        }else if(metodo==3){
            Vec dd=p.y;double suma=dd[0],producto=1;
            s.hoja<<"Reemplazo en la forma de Newton:\nP("<<q<<") = "<<dd[0];
            for(int j=1;j<n;++j){
                for(int i=0;i<n-j;++i)dd[i]=finito((dd[i+1]-dd[i])/(p.x[i+j]-p.x[i]));
                producto=finito(producto*(q-p.x[j-1]));
                suma=finito(suma+dd[0]*producto);
                s.hoja<<" + ("<<dd[0]<<")*("<<producto<<")";
            }
            s.hoja<<" = "<<suma<<'\n';
        }
        s.hoja<<"RESULTADO: P("<<q<<") = "<<v<<"\nValor calculado por evaluacion baricentrica de los mismos nodos.\n";
        if(!f.empty()){s.hoja<<"Referencia: f(x)="<<f<<'\n';errorExacto(s.hoja,v,evaluar(f,q));}
        if(!cotaTxt.empty()){
            long double producto=1,factorial=1;for(int i=0;i<n;++i){producto*=static_cast<long double>(q)-p.x[i];factorial*=i+1;}
            s.hoja<<"Resto teorico: f(q)-P(q)=f^("<<n<<")(xi)/"<<n<<"! * producto(q-x_i).\n"
                  <<"Si |f^("<<n<<")(x)| <= M="<<m<<" en el intervalo que contiene los nodos y q,\n"
                  <<"|f(q)-P(q)| <= M/"<<n<<"! * |producto(q-x_i)| = "<<finito(static_cast<double>(m*std::fabs(producto)/factorial))<<'\n';
        }
    };
    for(;;){
        int op=s.entero("1=Evaluar en un x, 2=generar tabla, 3=grafico SVG/CSV, 0=salir",0,3,"1");if(!op)break;
        if(op==1){consulta(s.num("x a evaluar"));}
        else if(op==3){
            graficoFuncion(s,[&](double q){return lagrangeValor(p,q);},"interpolacion","Polinomio interpolador",&p,f);
            continue;
        }else {
            double a=s.num("Inicio de tabla",numero(p.x.front())),b=s.num("Fin de tabla",numero(p.x.back()));
            if(b<=a)throw std::runtime_error("La tabla requiere inicio < fin.");
            int intervalos=s.entero("Cantidad de subintervalos",1,1000,"10");
            s.hoja<<"TABLA DEL POLINOMIO: h="<<(b-a)/intervalos<<'\n';
            std::vector<std::string> columnas={"i","x_i","P(x_i)"};
            if(!f.empty()){columnas.push_back("f(x_i)");columnas.push_back("error absoluto");}
            Tabla tabla(columnas);
            for(int i=0;i<=intervalos;++i){double q=i==intervalos?b:a+(b-a)*i/intervalos;double v=lagrangeValor(p,q);
                std::vector<Celda> fila={i,q,v};
                if(!f.empty()){double real=evaluar(f,q);fila.push_back(real);fila.push_back(std::fabs(v-real));}
                tabla.fila(fila);}
            tabla.mostrar(s.hoja);
        }
        s.mostrar();
    }
}
}
#endif
