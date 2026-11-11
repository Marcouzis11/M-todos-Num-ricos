@echo off
chcp 65001 >nul
setlocal
call "%~dp0compilar.bat"
if errorlevel 1 exit /b 1
"%~dp0menu.exe"
exit /b %errorlevel%
