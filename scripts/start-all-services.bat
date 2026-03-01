@echo off
chcp 65001 >nul 2>&1
REM =========================================
REM  start-all-services.bat
REM  すべてのサービスを起動するスクリプト (Windows)
REM =========================================

setlocal enabledelayedexpansion

echo =========================================
echo   Starting All Services
echo =========================================
echo.

REM プロジェクトルートの取得
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."

REM ログとPIDディレクトリを作成
if not exist "%PROJECT_ROOT%\logs" mkdir "%PROJECT_ROOT%\logs"
if not exist "%PROJECT_ROOT%\pids" mkdir "%PROJECT_ROOT%\pids"

REM =========================================
REM  [1/5] SQLiteデータベースの初期化
REM =========================================
set "DB_DIR=%PROJECT_ROOT%\data"
set "DB_FILE=%DB_DIR%\mobile_app.db"
echo [1/5] Checking SQLite database...

if not exist "%DB_DIR%" mkdir "%DB_DIR%"

if not exist "%DB_FILE%" (
    echo   Database not found. Initializing...

    where sqlite3 >nul 2>&1
    if errorlevel 1 (
        echo [ERROR] sqlite3 command not found!
        echo   Please install SQLite3 and add it to PATH.
        echo   Download: https://www.sqlite.org/download.html
        exit /b 1
    )

    sqlite3 "%DB_FILE%" < "%PROJECT_ROOT%\src\database\schema\01_create_tables.sql"
    if errorlevel 1 (
        echo [ERROR] Failed to create schema!
        exit /b 1
    )

    sqlite3 "%DB_FILE%" < "%PROJECT_ROOT%\src\database\seeds\02_seed_data.sql"
    if errorlevel 1 (
        echo [ERROR] Failed to insert seed data!
        exit /b 1
    )

    echo [OK] SQLite database initialized
) else (
    echo [OK] SQLite database exists
)
echo.

REM =========================================
REM  [2/5] Web APIを起動
REM =========================================
echo [2/5] Starting Web API (port 8080)...
cd /d "%PROJECT_ROOT%\src\web-api"
if exist "mvnw.cmd" (
    start "Web API" /b cmd /c "mvnw.cmd spring-boot:run > "%PROJECT_ROOT%\logs\web-api.log" 2>&1"
    echo [OK] Web API starting...
) else if exist "gradlew.bat" (
    start "Web API" /b cmd /c "gradlew.bat bootRun > "%PROJECT_ROOT%\logs\web-api.log" 2>&1"
    echo [OK] Web API starting...
) else (
    echo [ERROR] mvnw.cmd / gradlew.bat not found in src\web-api\
    exit /b 1
)
echo.

REM Web APIの起動を待機
echo Waiting for Web API to be ready...
set "WAIT_COUNT=0"
:wait_webapi
set /a WAIT_COUNT+=1
if !WAIT_COUNT! gtr 30 (
    echo [WARN] Web API may not be ready yet. Continuing...
    goto :webapi_done
)
timeout /t 2 /nobreak >nul
curl -s -o nul -w "" http://localhost:8080/actuator/health >nul 2>&1
if errorlevel 1 (
    echo   Waiting for Web API... ^(!WAIT_COUNT!/30^)
    goto :wait_webapi
)
echo [OK] Web API is ready
:webapi_done
echo.

REM =========================================
REM  [3/5] Mobile BFFを起動
REM =========================================
echo [3/5] Starting Mobile BFF (port 8081)...
cd /d "%PROJECT_ROOT%\src\mobile-bff"
if exist "mvnw.cmd" (
    start "Mobile BFF" /b cmd /c "mvnw.cmd spring-boot:run > "%PROJECT_ROOT%\logs\mobile-bff.log" 2>&1"
    echo [OK] Mobile BFF starting...
) else if exist "gradlew.bat" (
    start "Mobile BFF" /b cmd /c "gradlew.bat bootRun > "%PROJECT_ROOT%\logs\mobile-bff.log" 2>&1"
    echo [OK] Mobile BFF starting...
) else (
    echo [ERROR] mvnw.cmd / gradlew.bat not found in src\mobile-bff\
    exit /b 1
)
echo.

REM =========================================
REM  [4/5] Admin BFFを起動
REM =========================================
echo [4/5] Starting Admin BFF (port 8082)...
cd /d "%PROJECT_ROOT%\src\admin-bff"
if exist "mvnw.cmd" (
    start "Admin BFF" /b cmd /c "mvnw.cmd spring-boot:run > "%PROJECT_ROOT%\logs\admin-bff.log" 2>&1"
    echo [OK] Admin BFF starting...
) else if exist "gradlew.bat" (
    start "Admin BFF" /b cmd /c "gradlew.bat bootRun > "%PROJECT_ROOT%\logs\admin-bff.log" 2>&1"
    echo [OK] Admin BFF starting...
) else (
    echo [ERROR] mvnw.cmd / gradlew.bat not found in src\admin-bff\
    exit /b 1
)
echo.

REM =========================================
REM  [5/5] Admin Webを起動
REM =========================================
echo [5/5] Starting Admin Web (port 3000)...
cd /d "%PROJECT_ROOT%\src\admin-web"
if exist "package.json" (
    start "Admin Web" /b cmd /c "npm run dev > "%PROJECT_ROOT%\logs\admin-web.log" 2>&1"
    echo [OK] Admin Web starting...
) else (
    echo [ERROR] package.json not found in src\admin-web\
    exit /b 1
)
echo.

REM =========================================
REM  完了
REM =========================================
echo =========================================
echo   All Services Started!
echo =========================================
echo.
echo Services:
echo   - SQLite DB:   %DB_FILE%
echo   - Web API:     http://localhost:8080
echo   - Mobile BFF:  http://localhost:8081
echo   - Admin BFF:   http://localhost:8082
echo   - Admin Web:   http://localhost:3000
echo.
echo Logs:
echo   - logs\web-api.log
echo   - logs\mobile-bff.log
echo   - logs\admin-bff.log
echo   - logs\admin-web.log
echo.
echo To stop all services, run:
echo   scripts\stop-all-services.bat
echo.

cd /d "%PROJECT_ROOT%"
endlocal
