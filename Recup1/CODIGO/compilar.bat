@echo off
chcp 65001 >nul
setlocal
pushd "%~dp0"
where g++ >nul 2>nul
if errorlevel 1 (
    echo Falta g++ en PATH. Configura MinGW o el compilador de Code::Blocks.
    popd
    exit /b 1
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
