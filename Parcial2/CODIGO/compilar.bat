@echo off
REM Compila los programas del PARCIAL 2 si la PC tiene g++ (MinGW) en el PATH.
REM Si no lo tiene: abri el .cpp con ZinjaI (F9), Code::Blocks o Dev-C++ (F11).
REM IMPORTANTE: los archivos .h tienen que estar en esta misma carpeta.
cd /d "%~dp0"
where g++ >nul 2>nul
if errorlevel 1 (
  echo No se encontro g++ en esta PC.
  echo Abri splines.cpp ^(o el que necesites^) con ZinjaI y apreta F9.
  pause
  exit /b
)
for %%p in (splines integracion derivacion edo edo_sistema2 edo_sistema3 raices sistemas interpolacion regresion) do (
  if exist %%p.cpp (
    echo Compilando %%p.cpp ...
    g++ -O2 %%p.cpp -o %%p.exe
  )
)
echo.
echo Listo. Hace doble click en el .exe que necesites.
pause
