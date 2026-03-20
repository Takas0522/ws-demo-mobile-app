@echo off
chcp 65001 >nul 2>&1
REM =========================================
REM  start-admin-struts.bat
REM  Struts2 管理画面アプリケーションの起動スクリプト (Windows)
REM  Maven Tomcat プラグインで組込み Tomcat を使用（外部 Tomcat 不要）
REM =========================================

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
set "ADMIN_STRUTS_DIR=%PROJECT_ROOT%\src\admin-struts"
set "LOG_DIR=%PROJECT_ROOT%\logs"
set "PID_DIR=%PROJECT_ROOT%\pids"

if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"
if not exist "%PID_DIR%" mkdir "%PID_DIR%"

echo =========================================
echo  Struts2 Admin Application - Start
echo =========================================

REM WAR ビルド
echo.
echo [1/2] Building WAR...
cd /d "%ADMIN_STRUTS_DIR%"

where mvn >nul 2>&1
if errorlevel 1 (
    if exist "mvnw.cmd" (
        call mvnw.cmd clean package -q -DskipTests
    ) else (
        echo [ERROR] mvn command not found and mvnw.cmd does not exist!
        echo   Please install Apache Maven and add it to PATH.
        exit /b 1
    )
) else (
    call mvn clean package -q -DskipTests
)
if errorlevel 1 (
    echo [ERROR] Build failed!
    exit /b 1
)
echo   Build complete

REM Maven Tomcat プラグインで起動
echo.
echo [2/2] Starting embedded Tomcat on port 8082...

where mvn >nul 2>&1
if errorlevel 1 (
    start "Struts2 Admin" /b cmd /c "mvnw.cmd tomcat7:run -DskipTests > "%LOG_DIR%\admin-struts.log" 2>&1"
) else (
    start "Struts2 Admin" /b cmd /c "mvn tomcat7:run -DskipTests > "%LOG_DIR%\admin-struts.log" 2>&1"
)

REM 起動待ち
echo   Waiting for startup...
set "WAIT_COUNT=0"
:wait_startup
set /a WAIT_COUNT+=1
if !WAIT_COUNT! gtr 30 (
    echo.
    echo   Application may still be starting. Check logs: %LOG_DIR%\admin-struts.log
    goto :done
)
timeout /t 2 /nobreak >nul
curl -s -o nul -w "" http://localhost:8082/admin-struts/admin/login >nul 2>&1
if errorlevel 1 (
    echo   Waiting... ^(!WAIT_COUNT!/30^)
    goto :wait_startup
)

echo.
echo =========================================
echo  Struts2 Admin Application Started
echo =========================================
echo   URL:   http://localhost:8082/admin-struts/admin/login
echo   Logs:  %LOG_DIR%\admin-struts.log
echo   Login: admin001 / admin123
echo =========================================

:done
cd /d "%PROJECT_ROOT%"
endlocal
