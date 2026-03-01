@echo off
chcp 65001 >nul 2>&1

echo ============================================================
echo  C++ API Server - Database Initialize
echo ============================================================
echo.

set "SCRIPT_DIR=%~dp0"
set "DB_DIR=%SCRIPT_DIR%..\CppApiServer\bin\Debug\x64\data"
set "DB_FILE=%DB_DIR%\mobile_app.db"

if not exist "%DB_DIR%" mkdir "%DB_DIR%"

if exist "%DB_FILE%" (
    echo [WARN] Database already exists: %DB_FILE%
    set /p CONFIRM="Delete and recreate? [y/N]: "
    if /i not "!CONFIRM!"=="y" (
        echo [INFO] Cancelled.
        exit /b 0
    )
    del "%DB_FILE%"
    echo [INFO] Database deleted.
)

echo [INFO] Database will be auto-initialized on first server startup.
echo [INFO] Start the server with start-api.bat to create the database.
echo.
echo [INFO] Done.
exit /b 0
