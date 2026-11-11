#ifndef RECUP1_SISTEMAS_H
#define RECUP1_SISTEMAS_H
#include "base.h"
namespace recup {
struct Sistema {Mat a;Vec b;};
inline void informarSistema(Sesion &s,const Mat &a,const Vec &b){
    s.hoja<<"Sistema A*x=b, orden "<<a.size()<<". Incognitas x1..x"<<a.size()<<".\n";
    matriz(s.hoja,a,b);
}
inline void cargarSistema(Sesion &s,Mat &a,Vec &b,const std::string &predeterminado="data.txt"){
    int modo=s.entero("Datos: 1=pegar matriz aumentada por filas, 2=archivo",1,2,"1");
    Mat filas;
    if(modo==1){int n=s.entero("Cantidad de incognitas",1,20,"3");
        for(int i=0;i<n;++i) filas.push_back(leerLista(s,"Fila "+std::to_string(i+1)+" (coeficientes y termino independiente)",n+1));
    }else filas=archivoFilas(rutaDatos(s,predeterminado));
    int n=static_cast<int>(filas.size());if(n>20)throw std::runtime_error("Maximo 20 incognitas.");
    for(Vec f:filas){if(static_cast<int>(f.size())!=n+1)throw std::runtime_error("Matriz aumentada: n filas, n+1 valores por fila.");
        b.push_back(f.back());f.pop_back();a.push_back(f);}
    informarSistema(s,a,b);
}
inline void verificarSistema(Sesion &s,const Mat &a,const Vec &b,const Vec &x){
    s.hoja<<"\nCOMPROBACION EN EL SISTEMA ORIGINAL\n";
    Tabla tabla({"i","(A*x)_i","b_i","(A*x-b)_i"});
    for(size_t i=0;i<a.size();++i){double v=0;for(size_t j=0;j<x.size();++j)v+=a[i][j]*x[j];
        tabla.fila({i+1,finito(v),b[i],finito(v-b[i])});}
    tabla.mostrar(s.hoja);
    s.hoja<<"Norma infinito del residuo ||A*x-b||inf = "<<residuo(a,b,x)<<'\n';
}
inline void eliminacion(Sesion &s,const Sistema *original=nullptr){
    s.hoja<<"ELIMINACION GAUSSIANA CON PIVOTEO PARCIAL ESCALADO\n";
    Mat a;Vec b;
    if(original){a=original->a;b=original->b;informarSistema(s,a,b);}
    else cargarSistema(s,a,b);
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
inline int elegirErrorSistema(Sesion &s){
    textoConsola(std::cout,
        "Error estimado: compara la iteracion nueva con la anterior.\n"
        "Error exacto o verdadero: compara con la solucion de referencia del enunciado.");
    int op=s.entero("Tipo de error: 1=absoluto estimado, 2=absoluto exacto o verdadero, 3=relativo porcentual estimado, 4=relativo porcentual exacto o verdadero, 5=porcentual IUA estimado, 6=criterios avanzados",1,6,"1");
    const int basicos[]={0,1,8,3,10,4};
    if(op<6)return basicos[op];
    int avanzado=s.entero("Criterio avanzado: 1=absoluto estimado euclideo, 2=residuo del sistema, 3=absoluto estimado Y residuo (maximos), 4=absoluto estimado Y residuo (euclideos), 5=absoluto verdadero euclideo",1,5,"1");
    const int avanzados[]={0,2,5,6,7,9};return avanzados[avanzado];
}
inline Vec pedirReferenciaSistema(Sesion &s,const Mat &a,const Vec &b,bool opcional){
    for(;;){
        std::string t=s.leer(opcional?
            "Vector de referencia [ENTER=sin referencia; gauss=referencia numerica por Gauss]: ":
            "Solucion exacta de referencia, separada por espacios (o gauss para referencia numerica): ");
        if(t.empty() && opcional)return {};
        if(t=="gauss"){
            s.hoja<<"\nCALCULO DE REFERENCIA POR GAUSS: se detalla su pivoteo parcial escalado.\n";
            Vec ref=gauss(a,b,&s.hoja).x;
            s.hoja<<"Referencia calculada por Gauss: sujeta al redondeo, no una solucion simbolica exacta.\n";
            return ref;
        }
        try{
            Vec ref=lista(t);
            if(ref.size()==b.size())return ref;
            std::cout<<"La referencia requiere "<<b.size()<<" componentes, una por incognita.\n";
        }catch(const std::exception &e){std::cout<<"Referencia invalida: "<<e.what()<<'\n';}
    }
}
inline void iterativos(Sesion &s,const Sistema *original=nullptr,int seleccionado=0){
    int metodo=seleccionado?seleccionado:s.entero("Metodo: 1=Jacobi, 2=Gauss-Seidel, 3=Gauss-Seidel con relajacion (SOR)",1,3,"2");
    const char *nombre[]={"","JACOBI","GAUSS-SEIDEL","SOR"};s.hoja<<nombre[metodo]<<'\n';
    Mat a;Vec b;
    if(original){a=original->a;b=original->b;informarSistema(s,a,b);}
    else cargarSistema(s,a,b);
    int n=static_cast<int>(b.size());
    s.hoja<<"Pivoteo parcial durante las iteraciones: no se aplica.\n";
    double omega=1;
    if(metodo==3){for(;;){omega=s.num("Factor omega (0 < omega < 2)","1.1");if(omega>0 && omega<2)break;std::cout<<"Se requiere 0 < omega < 2.\n";}}
    Vec x(n,0);
    std::cout<<'\n';
    textoConsola(std::cout,metodo==1?
        "Jacobi necesita un punto de partida X^(0): cada paso usa todas las componentes del vector anterior.":
        "Gauss-Seidel y SOR necesitan un punto de partida X^(0); en cada paso usan tambien componentes recien calculadas.");
    std::string unos;for(int i=0;i<n;++i)unos+=(i?" 1":"1");
    textoConsola(std::cout,"Si el ejercicio indica X0j=1 para todo j, ingresa: "+unos);
    std::string inicio=s.leer("Vector inicial, separado por espacios [ENTER=todos ceros]: ");
    if(!inicio.empty()){x=lista(inicio);if(static_cast<int>(x.size())!=n)throw std::runtime_error("El vector inicial debe tener n componentes.");}
    s.hoja<<"Vector de partida X^(0) = (";
    for(int i=0;i<n;++i){s.hoja<<(i?", ":"")<<x[i];}
    s.hoja<<")\n";
    int criterio=elegirErrorSistema(s);
    Vec referencia;
    if(criterio>=8)referencia=pedirReferenciaSistema(s,a,b,false);
    if(criterio==10 && normaInf(referencia)==0)
        throw std::runtime_error("El error porcentual verdadero no esta definido para una referencia nula. Usa el error absoluto verdadero.");
    const bool porcentual=criterio==3 || criterio==4 || criterio==10;
    int modo=s.entero("Corte: 1=tolerancia, 2=cantidad fija de iteraciones",1,2,"1");
    double tol=modo==1?s.positivo(porcentual?"Tolerancia porcentual (0.1 significa 0.1%)":"Tolerancia","1e-6"):0;
    int maxit=s.entero(modo==1?"Maximo de iteraciones":"Cantidad de iteraciones",1,10000,modo==1?"100":"3");
    double tolR=modo==1 && (criterio==6 || criterio==7)?s.positivo("Tolerancia del residuo (simultanea)","1e-5"):0;
    bool estricto=true;
    int extras=s.entero(referencia.empty()?"Opciones adicionales: 0=continuar, 1=reordenar/referencia/comparacion":"Opciones adicionales: 0=continuar, 1=reordenar/comparacion",0,1,"0");
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
        if(referencia.empty())referencia=pedirReferenciaSistema(s,a,b,true);
        if(modo==1)estricto=s.entero("Comparacion: 1=error < tolerancia, 2=error <= tolerancia",1,2,"1")==1;
    }
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
    const char *criterios[]={"","max |delta_i|","sqrt(suma delta_i^2)","100*||delta||inf/||x_nuevo||inf (%)","100*||delta||inf/(||x_nuevo+x_viejo||inf/2) (%)","||A*x-b||inf","||paso||inf Y ||residuo||inf","||paso||2 Y ||residuo||2","||x-referencia||inf","||x-referencia||2","100*||x-referencia||inf/||referencia||inf (%)"};
    const char *tipos[]={"","Absoluto estimado","Absoluto estimado euclideo","Relativo porcentual estimado","Porcentual IUA estimado","Residuo del sistema","Absoluto estimado y residuo (maximos)","Absoluto estimado y residuo (euclideos)","Absoluto exacto o verdadero","Absoluto verdadero euclideo","Relativo porcentual exacto o verdadero"};
    s.hoja<<"Tipo de error elegido: "<<tipos[criterio]<<"\nCriterio E = "<<criterios[criterio]<<'\n';
    if(criterio==1 || criterio==8)s.hoja<<"Se toma la mayor diferencia absoluta entre las componentes del vector.\n";
    if(porcentual)s.hoja<<"Se usa el cociente de normas indicado arriba, multiplicado por 100. E y tolerancia se expresan en %.\n";
    if(modo==1){s.hoja<<"Corte: E "<<(estricto?"< ":"<= ")<<tol;
        if(criterio==6 || criterio==7)s.hoja<<" Y residuo "<<(estricto?"< ":"<= ")<<tolR;
        s.hoja<<"; maximo "<<maxit<<" iteraciones.\n";}
    if(!referencia.empty()){s.hoja<<"Vector de referencia: ";for(double z:referencia)s.hoja<<z<<' ';s.hoja<<'\n';}
    if(modo==2)s.hoja<<"Se pidieron "<<maxit<<" iteraciones fijas.\n";
    s.hoja<<"\nTABLA DE ITERACIONES\n";
    std::vector<std::string> columnas={"k"};for(int i=0;i<n;++i)columnas.push_back("x"+std::to_string(i+1));
    columnas.push_back("E");columnas.push_back("||A*x-b||inf");Tabla tabla(columnas);
    std::vector<Celda> inicial={0};for(double v:x)inicial.push_back(v);
    inicial.push_back("--");inicial.push_back(residuo(a,b,x));tabla.fila(inicial);
    std::ostringstream detalle;detalle<<std::setprecision(15);
    int kfin=0;bool converge=false;double e=0;std::string estado;
    if(criterio<8 && residuo(a,b,x)==0){converge=true;estado="El vector inicial resuelve el sistema.";}
    try {for(int k=1;k<=maxit && !converge;++k){
        Vec nuevo=x,delta(n),sumaVect(n);
        for(int i=0;i<n;++i){double suma=0;for(int j=0;j<n;++j)if(j!=i)suma+=a[i][j]*((metodo!=1 && j<i)?nuevo[j]:x[j]);
            double candidato=finito((b[i]-suma)/a[i][i]);
            nuevo[i]=finito((1-omega)*x[i]+omega*candidato);
            if(k==1)detalle<<"Primer paso, componente "<<i+1<<": candidato=("<<b[i]<<"-("<<suma<<"))/("<<a[i][i]<<")="<<candidato<<"; x nuevo="<<nuevo[i]<<'\n';
            delta[i]=finito(nuevo[i]-x[i]);sumaVect[i]=finito(nuevo[i]/2+x[i]/2);}
        double r=residuo(a,b,nuevo),num=normaInf(delta),den=criterio==4?normaInf(sumaVect):normaInf(nuevo);
        Vec rv(n),verdadero(n);
        for(int i=0;i<n;++i){double v=-b[i];for(int j=0;j<n;++j)v+=a[i][j]*nuevo[j];rv[i]=finito(v);
            if(!referencia.empty())verdadero[i]=finito(nuevo[i]-referencia[i]);}
        double rCorte=criterio==7?norma2(rv):r;
        e=criterio==10?100*(normaInf(verdadero)/normaInf(referencia)):criterio>=8?(criterio==8?normaInf(verdadero):norma2(verdadero)):criterio==6?num:criterio==7?norma2(delta):criterio==1?num:criterio==2?norma2(delta):criterio==5?r:den==0?(num==0?0:std::numeric_limits<double>::infinity()):100*(num/den);
        std::vector<Celda> fila={k};for(double v:nuevo)fila.push_back(v);fila.push_back(e);fila.push_back(r);tabla.fila(fila);
        if(criterio==7)detalle<<"Residuo euclideo iteracion "<<k<<" = "<<rCorte<<'\n';
        if(!referencia.empty())detalle<<"Error verdadero iteracion "<<k<<": inf="<<normaInf(verdadero)<<"; euclideo="<<norma2(verdadero)<<'\n';
        bool estancado=normaInf(delta)==0;x=nuevo;kfin=k;
        bool cumple=estricto?e<tol:e<=tol;
        bool combinado=criterio==6 || criterio==7;
        if(combinado)cumple=cumple && (estricto?rCorte<tolR:rCorte<=tolR);
        if((r==0 && !combinado && criterio<8) || (modo==1 && cumple)){converge=true;estado=r==0?"Residuo cero en aritmetica de maquina.":"Se alcanzo la tolerancia del criterio elegido.";}
        else if(estancado){estado="Estancamiento por precision de maquina.";break;}
    }}catch(...){tabla.mostrar(s.hoja);s.hoja<<detalle.str();throw;}
    tabla.mostrar(s.hoja);
    s.hoja<<"E: criterio elegido. --: no hay paso previo para calcular el error inicial.\n";
    if(!detalle.str().empty())s.hoja<<"\nDETALLE DE ITERACIONES\n"<<detalle.str();
    if(estado.empty())estado=modo==1?"Maximo de iteraciones sin alcanzar tolerancia.":"Se completo la cantidad de iteraciones solicitada.";
    titulo(s.hoja,"RESUMEN DEL METODO",'-');
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
