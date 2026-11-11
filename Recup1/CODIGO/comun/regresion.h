#ifndef RECUP1_REGRESION_H
#define RECUP1_REGRESION_H
#include "graficos.h"
namespace recup {
// Minimos cuadrados por Householder con escalado de columnas.
// Se evita usar las ecuaciones normales para la solucion numerica principal.
inline Vec minimosCuadrados(Mat a,Vec b){
    const int n=static_cast<int>(a.size()),m=static_cast<int>(a[0].size());
    if(n<m)throw std::runtime_error("Faltan datos para la cantidad de coeficientes.");
    Vec escala(m,0);
    for(int j=0;j<m;++j){for(int i=0;i<n;++i)escala[j]=std::hypot(escala[j],a[i][j]);
        if(escala[j]==0 || !std::isfinite(escala[j]))throw std::runtime_error("Base singular o fuera de escala.");
        for(int i=0;i<n;++i)a[i][j]/=escala[j];}
    for(int k=0;k<m;++k){
        double norm=0;for(int i=k;i<n;++i)norm=std::hypot(norm,a[i][k]);
        if(norm<=256*std::numeric_limits<double>::epsilon()*std::max(n,m))
            throw std::runtime_error("Base sin rango suficiente: datos repetidos, funciones dependientes o mal condicionamiento.");
        double alpha=-std::copysign(norm,a[k][k]);Vec v(n-k);
        for(int i=k;i<n;++i)v[i-k]=a[i][k];
        v[0]-=alpha;
        double vn=0;for(double z:v)vn=std::hypot(vn,z);for(double &z:v)z/=vn;
        for(int j=k;j<m;++j){double dot=0;for(int i=k;i<n;++i)dot+=v[i-k]*a[i][j];
            for(int i=k;i<n;++i)a[i][j]=finito(a[i][j]-2*v[i-k]*dot);}
        double dot=0;for(int i=k;i<n;++i)dot+=v[i-k]*b[i];
        for(int i=k;i<n;++i)b[i]=finito(b[i]-2*v[i-k]*dot);
    }
    Vec z(m,0);for(int i=m-1;i>=0;--i){double suma=0;for(int j=i+1;j<m;++j)suma+=a[i][j]*z[j];z[i]=finito((b[i]-suma)/a[i][i]);}
    for(int j=0;j<m;++j)z[j]=finito(z[j]/escala[j]);
    return z;
}
inline void normales(std::ostream &out,const Mat &phi,const Vec &y,bool pasos){
    int n=static_cast<int>(phi.size()),m=static_cast<int>(phi[0].size());Mat a(m,Vec(m,0));Vec b(m,0);
    for(int j=0;j<m;++j)for(int i=0;i<n;++i){b[j]=finito(b[j]+phi[i][j]*y[i]);
        for(int k=0;k<m;++k)a[j][k]=finito(a[j][k]+phi[i][j]*phi[i][k]);}
    out<<"\nECUACIONES NORMALES: (Phi^T*Phi)*c=Phi^T*y\n"
       <<"S(c)=suma_i (y_i - suma_j c_j*phi_j(x_i))^2.\n"
       <<"dS/dc_j=-2*suma_i phi_j(x_i)*(y_i-suma_k c_k*phi_k(x_i))=0.\n"
       <<"Por lo tanto: suma_k [suma_i phi_j(x_i)*phi_k(x_i)] c_k = suma_i phi_j(x_i)*y_i.\n";
    matriz(out,a,b);
    if(pasos){
        out<<"\nDesarrollo por Gauss del sistema normal (para escribir en la hoja):\n";
        try{Vec c=gauss(a,b,&out).x;for(int j=0;j<m;++j)out<<"c"<<j<<" = "<<c[j]<<'\n';}
        catch(const std::exception &e){out<<"No se usa este sistema para calcular el resultado: "<<e.what()<<'\n';}
    }
}
inline Vec convertirPolinomio(const Vec &ct,double centro,double escala){
    // Horner simbolico en t=(x-centro)/escala, acumulando coeficientes en x.
    Vec c(1,ct.back());
    for(int j=static_cast<int>(ct.size())-2;j>=0;--j){Vec d(c.size()+1,0);
        for(size_t k=0;k<c.size();++k){d[k]=finito(d[k]-c[k]*(centro/escala));d[k+1]=finito(d[k+1]+c[k]/escala);}
        d[0]=finito(d[0]+ct[j]);c=d;}
    return c;
}
inline void regresion(Sesion &s,bool soloRecta){
    Puntos p=puntos(s,false);int n=static_cast<int>(p.x.size());
    int tipo=soloRecta?1:s.entero("Ajuste: 1=polinomico, 2=base de funciones, 3=exponencial a*exp(b*x), 4=potencial a*x^b, 5=logaritmico a+b*ln(x), 6=saturacion a*x/(b+x)",1,6,"1");
    int grado=soloRecta?1:tipo==1?s.entero("Grado del polinomio (0=constante, 1=recta, 2=parabola)",0,std::min(10,n-1),std::to_string(std::min(2,n-1))):1;
    int m=grado+1;Vec u=p.x,v=p.y;std::vector<std::string> base;
    double centro=0,escala=1;
    if(tipo==2){m=s.entero("Cantidad de funciones de base",1,std::min(10,n),"2");
        for(int j=0;j<m;++j)base.push_back(s.funcion("phi_"+std::to_string(j)+"(x) (ej.: 1, x, sin(x), exp(x))"));}
    else {base={"1","x"};if(tipo==1){base.clear();for(int j=0;j<m;++j)base.push_back(j==0?"1":j==1?"x":"x^"+std::to_string(j));}}
    s.hoja<<"\nREGRESION POR MINIMOS CUADRADOS\n";
    if(tipo==3 || tipo==4){for(int i=0;i<n;++i){if(p.y[i]<=0 || (tipo==4 && p.x[i]<=0))throw std::runtime_error("Para linealizar este modelo se necesitan y>0 y, en potencial, x>0.");v[i]=std::log(p.y[i]);if(tipo==4)u[i]=std::log(p.x[i]);}
        s.hoja<<"Transformacion: v=ln(y); u="<<(tipo==3?"x":"ln(x)")<<"; v=c0+c1*u.\n"
              <<"Se minimizan residuos al cuadrado en ln(y), no en y original. a=exp(c0), b=c1.\n";}
    if(tipo==5){for(int i=0;i<n;++i){if(p.x[i]<=0)throw std::runtime_error("El ajuste logaritmico requiere x>0.");u[i]=std::log(p.x[i]);}
        s.hoja<<"Transformacion: u=ln(x); y=c0+c1*u.\n";}
    if(tipo==6){for(int i=0;i<n;++i){if(p.x[i]==0 || p.y[i]==0)throw std::runtime_error("La saturacion linealizada requiere x e y distintos de cero.");u[i]=1/p.x[i];v[i]=1/p.y[i];}
        s.hoja<<"Transformacion: v=1/y; u=1/x; v=c0+c1*u. a=1/c0, b=c1/c0.\n"
              <<"Se minimizan residuos al cuadrado en 1/y, no en y original.\n";}
    Mat phi(n,Vec(m));
    for(int i=0;i<n;++i)for(int j=0;j<m;++j)phi[i][j]=tipo==2?evaluar(base[j],p.x[i]):finito(std::pow(u[i],j));
    s.hoja<<"Base del sistema: ";for(int j=0;j<m;++j)s.hoja<<"phi"<<j<<"="<<base[j]<<"; ";s.hoja<<"\n";
    if(tipo>=3)s.hoja<<"En el sistema linealizado, la variable x de la base representa u.\n";
    if(tipo!=2){
        s.hoja<<"\nTABLA DE SUMATORIAS (u=x, v=y si no hay transformacion)\n i | u | v";
        for(int k=2;k<=2*grado;++k)s.hoja<<" | u^"<<k;
        for(int k=1;k<=grado;++k)s.hoja<<" | u^"<<k<<"*v";
        s.hoja<<" | v^2\n";
        Vec sumas(2*grado+1,0),productos(grado+1,0);double v2=0;
        for(int i=0;i<n;++i){s.hoja<<i<<" | "<<u[i]<<" | "<<v[i];
            for(int k=0;k<=2*grado;++k){double t=finito(std::pow(u[i],k));sumas[k]=finito(sumas[k]+t);if(k>=2)s.hoja<<" | "<<t;}
            for(int k=0;k<=grado;++k){double t=finito(std::pow(u[i],k)*v[i]);productos[k]=finito(productos[k]+t);if(k>=1)s.hoja<<" | "<<t;}
            double t=finito(v[i]*v[i]);v2=finito(v2+t);s.hoja<<" | "<<t<<'\n';}
        s.hoja<<"SUMAS: n="<<n;for(int k=0;k<=2*grado;++k)s.hoja<<"; suma u^"<<k<<"="<<sumas[k];
        for(int k=0;k<=grado;++k)s.hoja<<"; suma u^"<<k<<"*v="<<productos[k];
        s.hoja<<"; suma v^2="<<v2<<'\n';
    }else{
        s.hoja<<"\n i | y | phi0 | phi1 | ...\n";for(int i=0;i<n;++i){s.hoja<<i<<" | "<<v[i];for(double z:phi[i])s.hoja<<" | "<<z;s.hoja<<'\n';}
    }
    normales(s.hoja,phi,v,true);
    Mat estable=phi;
    // Para polinomios y linealizaciones afines se centra la variable antes de QR.
    if(tipo!=2 && grado>0){
        auto extremos=std::minmax_element(u.begin(),u.end());centro=*extremos.first/2+*extremos.second/2;
        escala=std::max(std::fabs(*extremos.second-centro),std::fabs(*extremos.first-centro));
        if(escala==0)throw std::runtime_error("Todas las abscisas del ajuste son iguales.");
        for(int i=0;i<n;++i)for(int j=0;j<m;++j)estable[i][j]=std::pow((u[i]-centro)/escala,j);
    }
    Vec ct=minimosCuadrados(estable,v),c=tipo==2?ct:convertirPolinomio(ct,centro,escala);
    s.hoja<<"\nRESULTADO (calculado por QR para evitar perdida de precision de las ecuaciones normales)\n";
    for(int j=0;j<m;++j)s.hoja<<"c"<<j<<" = "<<c[j]<<'\n';
    if(tipo!=2){
        s.hoja<<"Coordenada de calculo estable: t=(u-("<<centro<<"))/("<<escala<<").\nCoeficientes en t:\n";
        for(int j=0;j<m;++j)s.hoja<<"d"<<j<<" = "<<ct[j]<<'\n';
    }
    double aa=0,bb=0;
    if(tipo==1)polinomio(s.hoja,c,"y ajustado");
    else if(tipo==2){s.hoja<<"y ajustado = ";for(int j=0;j<m;++j)s.hoja<<(j?" + ":"")<<"("<<c[j]<<")*("<<base[j]<<")";s.hoja<<'\n';}
    else if(tipo==3 || tipo==4){aa=finito(std::exp(c[0]));bb=c[1];s.hoja<<"MODELO ORIGINAL: y = "<<aa<<(tipo==3?"*exp((":"*x^(")<<bb<<(tipo==3?")*x)":")")<<'\n';}
    else if(tipo==5)s.hoja<<"MODELO ORIGINAL: y = "<<c[0]<<" + ("<<c[1]<<")*ln(x)\n";
    else {if(c[0]==0)throw std::runtime_error("No se puede recuperar la saturacion: c0=0.");aa=finito(1/c[0]);bb=finito(c[1]/c[0]);s.hoja<<"MODELO ORIGINAL: y = "<<aa<<"*x/("<<bb<<"+x)\n";}
    auto predecir=[&](double q){
        double z=q;
        if(tipo==2){double pred=0;for(int j=0;j<m;++j)pred=finito(pred+c[j]*evaluar(base[j],q));return pred;}
        if(tipo==4 || tipo==5){if(q<=0)throw std::runtime_error("El modelo requiere x>0.");z=std::log(q);}
        if(tipo==6){if(q==0)throw std::runtime_error("El modelo linealizado no se evalua en x=0.");z=1/q;}
        double pred=poli(ct,(z-centro)/escala);
        if(tipo==3 || tipo==4)return finito(std::exp(pred));
        if(tipo==6){if(pred==0)throw std::runtime_error("Polo del modelo de saturacion.");return finito(1/pred);}
        return pred;
    };
    double media=0;for(double y:p.y)media+=y/n;double sr=0,st=0,srt=0,maxexp=0;
    s.hoja<<"\nRESIDUOS EN LA ESCALA ORIGINAL\n i | x_i | y_i | y ajustado | e_i=y_i-y ajustado | e_i^2\n";
    for(int i=0;i<n;++i){double pred=predecir(p.x[i]),err=finito(p.y[i]-pred);sr=finito(sr+err*err);st=finito(st+(p.y[i]-media)*(p.y[i]-media));
        double errt=v[i]-(tipo==2?pred:poli(ct,(u[i]-centro)/escala));srt=finito(srt+errt*errt);
        if(tipo==1)maxexp=std::max(maxexp,std::fabs(poli(c,p.x[i])-pred));
        s.hoja<<i<<" | "<<p.x[i]<<" | "<<p.y[i]<<" | "<<pred<<" | "<<err<<" | "<<err*err<<'\n';}
    s.hoja<<"Media y = "<<media<<"\nSr = suma e_i^2 = "<<sr<<"\nSt = suma (y_i-media)^2 = "<<st
          <<"\nECM (error cuadratico medio, sin raiz) = Sr/n = "<<sr/n
          <<"\nRMSE (raiz del ECM) = sqrt(Sr/n) = "<<std::sqrt(sr/n)<<'\n';
    if(n>m)s.hoja<<"Error estandar estimado s_y/x = sqrt(Sr/(n-m)) = "<<std::sqrt(sr/(n-m))<<"; m="<<m<<" coeficientes.\n";
    else s.hoja<<"Error estandar estimado: no definido, n-m=0 (sin grados de libertad residuales).\n";
    if(st>0){double r2=1-sr/st;s.hoja<<"R^2 = 1-Sr/St = "<<r2<<'\n';
        if(((tipo==1 && grado>1) || tipo==2) && r2>=0)s.hoja<<"Indice de correlacion multiple R=sqrt(R^2) = "<<std::sqrt(r2)<<" (no es el r de Pearson).\n";
        if(r2<0)s.hoja<<"sqrt(R^2) no es real: el ajuste es peor que usar la media. No se toma valor absoluto.\n";
        if(tipo==2)s.hoja<<"Para bases sin constante, sqrt(1-Sr/St) es el indice solicitado por esta convencion; no necesariamente coincide con la correlacion de Pearson entre y e y ajustado.\n";
    }else s.hoja<<"R^2: no definido, St=0 (todos los y son iguales).\n";
    if(tipo==1 && grado==1){
        double mx=0;for(double x:p.x)mx+=x/n;double sxx=0,syy=0,sxy=0;
        for(int i=0;i<n;++i){double dx=p.x[i]-mx,dy=p.y[i]-media;sxx+=dx*dx;syy+=dy*dy;sxy+=dx*dy;}
        if(sxx>0 && syy>0)s.hoja<<"r de Pearson (con signo) = "<<finito((sxy/std::sqrt(sxx))/std::sqrt(syy))<<'\n';
        else s.hoja<<"r de Pearson: no definido por varianza nula.\n";
    }
    if(tipo==3 || tipo==4 || tipo==6)s.hoja<<"Suma de residuos cuadrados en escala transformada (objetivo minimizado) = "<<srt<<'\n';
    if(tipo==1){s.hoja<<"Diferencia maxima entre forma expandida y evaluacion estable en los datos = "<<maxexp<<'\n';
        if(maxexp>1e-8*std::max(1.0,std::fabs(media)))s.hoja<<"AVISO: evaluar en t conserva mas precision que usar los coeficientes expandidos en x.\n";}
    s.mostrar();
    for(;;){int op=s.entero("1=predecir en un x, 2=raiz del modelo, 3=grafico SVG/CSV, 4=sistema normal redondeado por Gauss, 0=salir",0,4,"1");if(!op)break;
        if(op==4){
            int dec=s.entero("Decimales del sistema normal (se redondean matriz y vector)",0,12,"3");
            double factor=std::pow(10.0,dec);Mat ar(m,Vec(m,0));Vec br(m,0);
            for(int j=0;j<m;++j)for(int i=0;i<n;++i){br[j]=finito(br[j]+phi[i][j]*v[i]);
                for(int k=0;k<m;++k)ar[j][k]=finito(ar[j][k]+phi[i][j]*phi[i][k]);}
            for(int j=0;j<m;++j){br[j]=finito(std::round(finito(br[j]*factor))/factor);
                for(int k=0;k<m;++k)ar[j][k]=finito(std::round(finito(ar[j][k]*factor))/factor);}
            s.hoja<<"SISTEMA NORMAL REDONDEADO A "<<dec<<" DECIMALES\n";matriz(s.hoja,ar,br);
            try{Vec cr=gauss(ar,br,&s.hoja).x;for(int j=0;j<m;++j)s.hoja<<"Coeficiente redondeado c"<<j<<" = "<<cr[j]<<'\n';}
            catch(const std::exception &e){s.hoja<<"El sistema redondeado no se pudo resolver: "<<e.what()<<'\n';}
            s.hoja<<"Este resultado corresponde al sistema redondeado. El modelo principal conserva los datos sin redondear.\n";
            s.mostrar();continue;
        }
        if(op==2){raizModelo(s,predecir);continue;}
        if(op==3){graficoFuncion(s,predecir,"ajuste","Modelo ajustado",&p);continue;}
        double q=s.num("x para predecir"),pred=predecir(q);
        s.hoja<<"PREDICCION CON EL MODELO AJUSTADO\nRESULTADO: y("<<q<<") = "<<pred<<'\n';
        auto mm=std::minmax_element(p.x.begin(),p.x.end());if(q<*mm.first || q>*mm.second)s.hoja<<"La consulta esta fuera del rango observado: EXTRAPOLACION.\n";
        std::string ref=s.leer("Valor real de y para comparar [ENTER=sin referencia]: ");
        if(!ref.empty())errorExacto(s.hoja,pred,Expresion(ref).evaluar(0,false,false));
        s.mostrar();}
}
}
#endif
