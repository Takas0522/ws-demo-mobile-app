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
REM  [1/4] SQLiteデータベースの初期化
REM =========================================
set "DB_DIR=%PROJECT_ROOT%\src\web-api\data"
set "DB_FILE=%DB_DIR%\mobile_app.db"
echo [1/4] Checking SQLite database...

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
REM  [2/4] Web APIを起動
REM =========================================
echo [2/4] Starting Web API (port 8080)...
cd /d "%PROJECT_ROOT%\src\web-api"
set "JAR_FILE=target\web-api-1.0.0-SNAPSHOT.jar"
if not exist "%JAR_FILE%" (
    echo   JAR not found. Building...
    where mvn >nul 2>&1
    if errorlevel 1 (
        if exist "mvnw.cmd" (
            call mvnw.cmd clean package -q -DskipTests
        ) else (
            echo [ERROR] mvn / mvnw.cmd not found!
            exit /b 1
        )
    ) else (
        call mvn clean package -q -DskipTests
    )
)
start "Web API" /b cmd /c "java -jar "%JAR_FILE%" > "%PROJECT_ROOT%\logs\web-api.log" 2>&1"
echo [OK] Web API starting...
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
REM  [3/4] Mobile BFFを起動
REM =========================================
echo [3/4] Starting Mobile BFF (port 8081)...
cd /d "%PROJECT_ROOT%\src\mobile-bff"
set "JAR_FILE=target\mobile-bff-1.0.0-SNAPSHOT.jar"
if not exist "%JAR_FILE%" (
    echo   JAR not found. Building...
    where mvn >nul 2>&1
    if errorlevel 1 (
        if exist "mvnw.cmd" (
            call mvnw.cmd clean package -q -DskipTests
        ) else (
            echo [ERROR] mvn / mvnw.cmd not found!
            exit /b 1
        )
    ) else (
        call mvn clean package -q -DskipTests
    )
)
start "Mobile BFF" /b cmd /c "java -jar "%JAR_FILE%" > "%PROJECT_ROOT%\logs\mobile-bff.log" 2>&1"
echo [OK] Mobile BFF starting...
echo.

REM [MIGRATED TO STRUTS2] Admin BFF + Admin Web（旧構成 - Struts2 移行により無効化）
REM Admin BFF (port 8082) と Admin Web (port 3000) は
REM Struts2 管理画面アプリに統合されました。

REM =========================================
REM  [4/4] Struts2 管理画面アプリを起動
REM =========================================
echo [4/4] Starting Struts2 Admin Application (port 8082)...
call "%SCRIPT_DIR%start-admin-struts.bat"
echo [OK] Struts2 Admin Application started
echo.

REM =========================================
REM  完了
REM =========================================
echo =========================================
echo   All Services Started!
echo =========================================
echo.
echo Services:
echo   - SQLite DB:     %DB_FILE%
echo   - Web API:       http://localhost:8080
echo   - Mobile BFF:    http://localhost:8081
echo   - Struts2 Admin: http://localhost:8082/admin-struts/admin/login
echo.
echo Logs:
echo   - logs\web-api.log
echo   - logs\mobile-bff.log
echo   - logs\admin-struts.log
echo.
echo To stop all services, run:
echo   scripts\stop-all-services.bat
echo.

cd /d "%PROJECT_ROOT%"
endlocal
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
