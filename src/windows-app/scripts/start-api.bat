@echo off
chcp 65001 >nul 2>&1

echo ============================================================
echo  C++ API Server - Startup
echo ============================================================
echo.

set "SCRIPT_DIR=%~dp0"
set "BIN_DIR=%SCRIPT_DIR%..\CppApiServer\bin\Debug\x64"
set "EXE=%BIN_DIR%\CppApiServer.exe"

if not exist "%EXE%" (
    echo [ERROR] CppApiServer.exe not found at: %EXE%
    echo         Please build the project first with build.bat
    exit /b 1
)

rem Create data directory if it doesn't exist
if not exist "%BIN_DIR%\data" mkdir "%BIN_DIR%\data"

echo [INFO] Starting C++ API Server...
echo [INFO] Executable: %EXE%
echo.

"%EXE%" --port 8081 --db "%BIN_DIR%\data\mobile_app.db"
