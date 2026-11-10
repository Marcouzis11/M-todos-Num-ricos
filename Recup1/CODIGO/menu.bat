@echo off
chcp 65001 >nul
setlocal
call "%~dp0compilar.bat"
if errorlevel 1 exit /b 1
:menu
cls
echo ================ RECUPERATORIO 1 ================
echo  1 Biseccion                    2 Regula Falsi
echo  3 Newton-Raphson               4 Secante
echo  5 Punto fijo
echo  6 Gauss                        7 Jacobi / Seidel / SOR
echo  8 Interpolacion                9 Splines
echo 10 Regresion lineal            11 Regresion polinomica y otros
set "op="
set /p "op=Tema (0=salir): "
if "%op%"=="0" exit /b 0
set "prog="
if "%op%"=="1" set "prog=LocalizacionRaices\Cerrados\Biseccion"
if "%op%"=="2" set "prog=LocalizacionRaices\Cerrados\RegulaFalsi"
if "%op%"=="3" set "prog=LocalizacionRaices\Abiertos\Newton-Raphson"
if "%op%"=="4" set "prog=LocalizacionRaices\Abiertos\Secante"
if "%op%"=="5" set "prog=LocalizacionRaices\Abiertos\Punto-Fijo"
if "%op%"=="6" set "prog=SistEcuacionesLineales\Eliminacion-Gaussiana"
if "%op%"=="7" set "prog=SistEcuacionesLineales\JacobiGaussSeidelRelax"
if "%op%"=="8" set "prog=Interpolacion\interpolacion"
if "%op%"=="9" set "prog=Interpolacion\Spline"
if "%op%"=="10" set "prog=Regresión\regresionLineal"
if "%op%"=="11" set "prog=Regresión\polynomialRegression"
if not defined prog goto menu
"%~dp0%prog%.exe"
pause
goto menu
