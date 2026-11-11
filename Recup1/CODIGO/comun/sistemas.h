#ifndef RECUP1_SISTEMAS_H
#define RECUP1_SISTEMAS_H
#include "base.h"
namespace recup {
inline void cargarSistema(Sesion &s,Mat &a,Vec &b){
    int modo=s.entero("Datos: 1=pegar matriz aumentada por filas, 2=archivo",1,2,"1");
    Mat filas;
    if(modo==1){int n=s.entero("Cantidad de incognitas",1,20,"3");
        for(int i=0;i<n;++i) filas.push_back(leerLista(s,"Fila "+std::to_string(i+1)+" (coeficientes y termino independiente)",n+1));
    }else filas=archivoFilas(rutaDatos(s));
    int n=static_cast<int>(filas.size());if(n>20)throw std::runtime_error("Maximo 20 incognitas.");
    for(Vec f:filas){if(static_cast<int>(f.size())!=n+1)throw std::runtime_error("Matriz aumentada: n filas, n+1 valores por fila.");
        b.push_back(f.back());f.pop_back();a.push_back(f);}
    s.hoja<<"Sistema A*x=b, orden "<<n<<". Incognitas x1..x"<<n<<".\n";matriz(s.hoja,a,b);
}
inline void verificarSistema(Sesion &s,const Mat &a,const Vec &b,const Vec &x){
    s.hoja<<"\nCOMPROBACION EN EL SISTEMA ORIGINAL\n i | (A*x)_i | b_i | (A*x-b)_i\n";
    for(size_t i=0;i<a.size();++i){double v=0;for(size_t j=0;j<x.size();++j)v+=a[i][j]*x[j];
        s.hoja<<i+1<<" | "<<finito(v)<<" | "<<b[i]<<" | "<<finito(v-b[i])<<'\n';}
    s.hoja<<"Norma infinito del residuo ||A*x-b||inf = "<<residuo(a,b,x)<<'\n';
}
inline void eliminacion(Sesion &s){
    s.hoja<<"ELIMINACION GAUSSIANA CON PIVOTEO PARCIAL ESCALADO\n";
    Mat a;Vec b;cargarSistema(s,a,b);
    Mat prueba=a;bool sinPivoteo=true;int pasoFallo=0;
    for(size_t k=0;k<prueba.size();++k){
        double escala=0;for(double z:prueba[k])escala=std::max(escala,std::fabs(z));
        if(escala==0 || std::fabs(prueba[k][k])/escala<=64*std::numeric_limits<double>::epsilon()){
            sinPivoteo=false;pasoFallo=static_cast<int>(k)+1;break;}
        for(size_t i=k+1;i<prueba.size();++i){double mult=prueba[i][k]/prueba[k][k];
            for(size_t j=k+1;j<prueba.size();++j)prueba[i][j]-=mult*prueba[k][j];
            prueba[i][k]=0;}
    }
    if(sinPivoteo)s.hoja<<"Diagnostico sin intercambiar filas: no aparecen pivotes nulos o pequenos segun el umbral numerico; el pivoteo sigue siendo recomendable para estabilidad.\n";
    else s.hoja<<"Diagnostico sin intercambiar filas: pivote nulo o pequeno en el paso "<<pasoFallo<<"; se necesita revisar el orden/pivoteo.\n";
    Solucion sol=gauss(a,b,&s.hoja);
    s.hoja<<"\nRESULTADO\n";
    for(size_t i=0;i<sol.x.size();++i)s.hoja<<"x"<<i+1<<" = "<<sol.x[i]<<'\n';
    if(std::isfinite(sol.det) && sol.det!=0)
        s.hoja<<"det(A) = (-1)^intercambios * producto U_ii = "<<sol.det<<'\n';
    else s.hoja<<"det(A): magnitud fuera del rango representable; el sistema se resolvio verificando pivotes, no el producto.\n";
    s.hoja<<"El determinante no es una norma.\n";
    verificarSistema(s,a,b,sol.x);s.mostrar();
}
inline double norma2(const Vec &v){double n=0;for(double x:v)n=std::hypot(n,x);return n;}
inline double normaInf(const Vec &v){double n=0;for(double x:v)n=std::max(n,std::fabs(x));return n;}
// Matching de filas a columnas: encuentra dominancia estricta si existe por permutacion.
inline bool permutacionDominante(const Mat &a,std::vector<int> &orden){
    int n=static_cast<int>(a.size());std::vector<int> columnaFila(n,-1);
    std::function<bool(int,std::vector<bool>&)> buscar=[&](int j,std::vector<bool> &visitada){
        for(int i=0;i<n;++i){double resto=0;for(int k=0;k<n;++k)if(k!=j)resto+=std::fabs(a[i][k]);
            if(visitada[i] || !(std::fabs(a[i][j])>resto))continue;
            visitada[i]=true;
            if(columnaFila[i]<0 || buscar(columnaFila[i],visitada)){columnaFila[i]=j;return true;}
        }return false;
    };
    for(int j=0;j<n;++j){std::vector<bool> visitada(n,false);if(!buscar(j,visitada))return false;}
    orden.assign(n,0);for(int i=0;i<n;++i)orden[columnaFila[i]]=i;return true;
}
inline void errorVector(std::ostream &out,const Vec &x,const Vec &ref){
    Vec delta(x.size());out<<"Error verdadero por componente:\n";
    for(size_t i=0;i<x.size();++i){delta[i]=finito(x[i]-ref[i]);out<<"|x"<<i+1<<"-ref| = "<<std::fabs(delta[i])<<'\n';}
    out<<"Error verdadero infinito = "<<normaInf(delta)<<"\nError verdadero euclideo = "<<norma2(delta)<<'\n';
    if(normaInf(ref)>0)out<<"Error verdadero relativo infinito % = "<<100*(normaInf(delta)/normaInf(ref))<<'\n';
}
inline void iterativos(Sesion &s){
    int metodo=s.entero("Metodo: 1=Jacobi, 2=Gauss-Seidel, 3=Gauss-Seidel con relajacion (SOR)",1,3,"2");
    const char *nombre[]={"","JACOBI","GAUSS-SEIDEL","SOR"};s.hoja<<nombre[metodo]<<'\n';
    Mat a;Vec b;cargarSistema(s,a,b);int n=static_cast<int>(b.size());
    double omega=1;
    if(metodo==3){for(;;){omega=s.num("Factor omega (0 < omega < 2)","1.1");if(omega>0 && omega<2)break;std::cout<<"Se requiere 0 < omega < 2.\n";}}
    Vec x(n,0);
    std::string inicio=s.leer("Vector inicial, separado por espacios [ENTER=todos ceros]: ");
    if(!inicio.empty()){x=lista(inicio);if(static_cast<int>(x.size())!=n)throw std::runtime_error("El vector inicial debe tener n componentes.");}
    int criterio=s.entero("Error: 1=norma infinito del paso, 2=norma euclidea del paso, 3=relativo porcentual infinito, 4=IUA porcentual infinito, 5=norma infinito del residuo, 6=paso Y residuo infinito, 7=paso Y residuo euclideo, 8=error verdadero infinito, 9=error verdadero euclideo",1,9,"1");
    int modo=s.entero("Corte: 1=tolerancia, 2=cantidad fija de iteraciones",1,2,"1");
    double tol=modo==1?s.positivo("Tolerancia","1e-6"):0;
    int maxit=s.entero(modo==1?"Maximo de iteraciones":"Cantidad de iteraciones",1,10000,modo==1?"100":"3");
    double tolR=modo==1 && (criterio==6 || criterio==7)?s.positivo("Tolerancia del residuo (simultanea)","1e-5"):0;
    Vec referencia;
    bool estricto=true;
    int extras=s.entero("Opciones adicionales: 0=continuar, 1=reordenar/referencia/comparacion",0,1,"0");
    if(extras){
        int reorden=s.entero("Filas: 0=conservar orden, 1=permutacion manual, 2=buscar dominancia estricta",0,2,"0");
        std::vector<int> orden;
        if(reorden==1){Vec filas=leerLista(s,"Orden de filas (numeradas desde 1)",n);std::vector<bool> usada(n,false);
            for(double v:filas){if(v<1 || v>n || v!=std::floor(v) || usada[static_cast<int>(v)-1])throw std::runtime_error("Permutacion invalida: cada fila debe aparecer una vez.");
                int i=static_cast<int>(v)-1;usada[i]=true;orden.push_back(i);}}
        if(reorden==2 && !permutacionDominante(a,orden))s.hoja<<"No existe una permutacion de filas con dominancia estricta. No se altera el sistema; esto no demuestra divergencia.\n";
        if(!orden.empty()){Mat copia=a;Vec bb=b;s.hoja<<"Permutacion de ecuaciones: ";
            for(int i=0;i<n;++i){a[i]=copia[orden[i]];b[i]=bb[orden[i]];s.hoja<<orden[i]+1<<' ';}
            s.hoja<<"\nNo se permutan las variables ni el vector inicial.\n";matriz(s.hoja,a,b);}
        std::string t=s.leer("Vector de referencia [ENTER=sin referencia; gauss=referencia numerica por Gauss]: ");
        if(t=="gauss"){referencia=gauss(a,b).x;s.hoja<<"Referencia calculada por Gauss: sujeta al redondeo, no una solucion simbolica exacta.\n";}
        else if(!t.empty()){referencia=lista(t);if(static_cast<int>(referencia.size())!=n)throw std::runtime_error("Referencia: se necesitan n componentes.");}
        if(modo==1)estricto=s.entero("Comparacion: 1=error < tolerancia, 2=error <= tolerancia",1,2,"1")==1;
    }
    if(criterio>=8 && referencia.empty())throw std::runtime_error("El criterio de error verdadero requiere vector de referencia en opciones adicionales.");
    s.hoja<<"\nControl de dominancia diagonal estricta por filas:\n";bool dominante=true;
    for(int i=0;i<n;++i){double sum=0;for(int j=0;j<n;++j)if(i!=j)sum+=std::fabs(a[i][j]);
        if(a[i][i]==0)throw std::runtime_error("Diagonal nula: reordena las ecuaciones antes de usar el metodo iterativo.");
        bool ok=std::fabs(a[i][i])>sum;dominante=dominante && ok;
        s.hoja<<"Fila "<<i+1<<": |a_ii|="<<std::fabs(a[i][i])<<"; suma fuera de diagonal="<<sum<<" -> "<<(ok?"cumple":"no cumple")<<'\n';}
    s.hoja<<(dominante?"La dominancia estricta garantiza convergencia de Jacobi y Gauss-Seidel.\n":"No hay dominancia estricta: esto NO implica divergencia. Se verifica la iteracion numericamente.\n");
    if(metodo==3)s.hoja<<"Para SOR, 0<omega<2 por si solo no garantiza convergencia en una matriz general.\n";
    s.hoja<<"\nDespejes por ecuacion:\n";
    for(int i=0;i<n;++i){s.hoja<<"x"<<i+1<<" = ("<<b[i];for(int j=0;j<n;++j)if(i!=j)s.hoja<<" - ("<<a[i][j]<<")*x"<<j+1;s.hoja<<")/("<<a[i][i]<<")\n";}
    s.hoja<<(metodo==1?"Jacobi usa todas las componentes de la iteracion anterior.\n":"Gauss-Seidel usa componentes nuevas para j<i y anteriores para j>i.\n");
    if(metodo==3)s.hoja<<"x_i nuevo = (1-omega)*x_i anterior + omega*x_i candidato GS; omega="<<omega<<'\n';
    const char *criterios[]={"","max |delta_i|","sqrt(suma delta_i^2)","100*||delta||inf/||x_nuevo||inf (%)","100*||delta||inf/(||x_nuevo+x_viejo||inf/2) (%)","||A*x-b||inf","||paso||inf Y ||residuo||inf","||paso||2 Y ||residuo||2","||x-referencia||inf","||x-referencia||2"};
    s.hoja<<"Criterio E = "<<criterios[criterio]<<'\n';
    if(modo==1){s.hoja<<"Corte: E "<<(estricto?"< ":"<= ")<<tol;
        if(criterio==6 || criterio==7)s.hoja<<" Y residuo "<<(estricto?"< ":"<= ")<<tolR;
        s.hoja<<"; maximo "<<maxit<<" iteraciones.\n";}
    if(!referencia.empty()){s.hoja<<"Vector de referencia: ";for(double z:referencia)s.hoja<<z<<' ';s.hoja<<'\n';}
    if(modo==2)s.hoja<<"Se pidieron "<<maxit<<" iteraciones fijas.\n";
    s.hoja<<"\nk";for(int i=0;i<n;++i)s.hoja<<" | x"<<i+1;s.hoja<<" | E | ||A*x-b||inf\n0";
    for(double v:x)s.hoja<<" | "<<v;
    s.hoja<<" | -- | "<<residuo(a,b,x)<<'\n';
    int kfin=0;bool converge=false;double e=0;std::string estado;
    if(criterio<8 && residuo(a,b,x)==0){converge=true;estado="El vector inicial resuelve el sistema.";}
    for(int k=1;k<=maxit && !converge;++k){
        Vec nuevo=x,delta(n),sumaVect(n);
        for(int i=0;i<n;++i){double suma=0;for(int j=0;j<n;++j)if(j!=i)suma+=a[i][j]*((metodo!=1 && j<i)?nuevo[j]:x[j]);
            double candidato=finito((b[i]-suma)/a[i][i]);
            nuevo[i]=finito((1-omega)*x[i]+omega*candidato);
            if(k==1)s.hoja<<"Primer paso, componente "<<i+1<<": candidato=("<<b[i]<<"-("<<suma<<"))/("<<a[i][i]<<")="<<candidato<<"; x nuevo="<<nuevo[i]<<'\n';
            delta[i]=finito(nuevo[i]-x[i]);sumaVect[i]=finito(nuevo[i]/2+x[i]/2);}
        double r=residuo(a,b,nuevo),num=normaInf(delta),den=criterio==4?normaInf(sumaVect):normaInf(nuevo);
        Vec rv(n),verdadero(n);
        for(int i=0;i<n;++i){double v=-b[i];for(int j=0;j<n;++j)v+=a[i][j]*nuevo[j];rv[i]=finito(v);
            if(!referencia.empty())verdadero[i]=finito(nuevo[i]-referencia[i]);}
        double rCorte=criterio==7?norma2(rv):r;
        e=criterio>=8?(criterio==8?normaInf(verdadero):norma2(verdadero)):criterio==6?num:criterio==7?norma2(delta):criterio==1?num:criterio==2?norma2(delta):criterio==5?r:den==0?(num==0?0:std::numeric_limits<double>::infinity()):100*(num/den);
        s.hoja<<k;for(double v:nuevo)s.hoja<<" | "<<v;s.hoja<<" | "<<e<<" | "<<r<<'\n';
        if(criterio==7)s.hoja<<"Residuo euclideo iteracion "<<k<<" = "<<rCorte<<'\n';
        if(!referencia.empty())s.hoja<<"Error verdadero iteracion "<<k<<": inf="<<normaInf(verdadero)<<"; euclideo="<<norma2(verdadero)<<'\n';
        bool estancado=normaInf(delta)==0;x=nuevo;kfin=k;
        bool cumple=estricto?e<tol:e<=tol;
        bool combinado=criterio==6 || criterio==7;
        if(combinado)cumple=cumple && (estricto?rCorte<tolR:rCorte<=tolR);
        if((r==0 && !combinado && criterio<8) || (modo==1 && cumple)){converge=true;estado=r==0?"Residuo cero en aritmetica de maquina.":"Se alcanzo la tolerancia del criterio elegido.";}
        else if(estancado){estado="Estancamiento por precision de maquina.";break;}
    }
    if(estado.empty())estado=modo==1?"Maximo de iteraciones sin alcanzar tolerancia.":"Se completo la cantidad de iteraciones solicitada.";
    s.hoja<<"\nESTADO: "<<estado<<'\n'<<(converge?"RESULTADO\n":"ULTIMA APROXIMACION (sin convergencia certificada)\n");
    for(int i=0;i<n;++i)s.hoja<<"x"<<i+1<<" = "<<x[i]<<'\n';
    s.hoja<<"Iteraciones = "<<kfin<<"; ultimo E = "<<e<<'\n';
    s.hoja<<"Un paso pequeno no es por si solo una cota del error verdadero: se adjunta el residuo.\n";
    s.hoja<<"A diez decimales: ";for(double z:x)s.hoja<<fijo10(z)<<' ';s.hoja<<'\n';
    if(!referencia.empty())errorVector(s.hoja,x,referencia);
    verificarSistema(s,a,b,x);s.mostrar();
}
}
#endif
