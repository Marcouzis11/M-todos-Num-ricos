@echo off
chcp 65001 >nul
setlocal
pushd "%~dp0"
if exist "..\compilador.local.bat" call "..\compilador.local.bat"
where g++ >nul 2>nul
if errorlevel 1 (
    for /r %%F in (*.cpp) do (
        if not exist "%%~dpnF.exe" (
            echo Falta g++ y no existe %%~dpnF.exe. Configura MinGW o Code::Blocks.
            popd
            exit /b 1
        )
    )
    echo No hay g++ disponible: se usaran los ejecutables existentes sin recompilar.
    echo Si modificaste el codigo, configura un compilador antes de ejecutarlo.
    popd
    exit /b 0
)
for /r %%F in (*.cpp) do (
    echo Compilando %%~nxF
    g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic "%%F" -o "%%~dpnF.exe"
    if errorlevel 1 (
        popd
        exit /b 1
    )
)
echo Los 11 programas estan listos.
popd
exit /b 0
