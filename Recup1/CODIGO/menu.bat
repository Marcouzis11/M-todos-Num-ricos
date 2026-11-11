@echo off
chcp 65001 >nul
setlocal
call "%~dp0compilar.bat"
if errorlevel 1 exit /b 1
:menu
cls
echo ================================================================
echo  RECUPERATORIO 1 ^| METODOS NUMERICOS
echo ================================================================
echo  LOCALIZACION DE RAICES
echo    1) Biseccion
echo    2) Regula Falsi
echo    3) Newton-Raphson
echo    4) Secante
echo    5) Punto fijo
echo.
echo  SISTEMAS DE ECUACIONES LINEALES
echo    6) Eliminacion Gaussiana
echo    7) Jacobi / Gauss-Seidel / SOR
echo.
echo  INTERPOLACION Y AJUSTE DE CURVAS
echo    8) Interpolacion
echo    9) Splines
echo   10) Regresion lineal
echo   11) Regresion polinomica y otros ajustes
echo ----------------------------------------------------------------
echo    0) Salir
echo  Los informes se guardan junto al ejecutable de cada metodo.
echo ----------------------------------------------------------------
set "op="
set /p "op=Elegi una opcion [0-11]: "
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
if not defined prog (
    echo Elegi una opcion del 0 al 11.
    pause
    goto menu
)
"%~dp0%prog%.exe"
pause
goto menu
