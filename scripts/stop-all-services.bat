@echo off
chcp 65001 >nul 2>&1
REM =========================================
REM  stop-all-services.bat
REM  すべてのサービスを停止するスクリプト (Windows)
REM =========================================

setlocal enabledelayedexpansion

echo =========================================
echo   Stopping All Services
echo =========================================
echo.

REM プロジェクトルートの取得
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."

REM =========================================
REM  Javaプロセス（Web API / Mobile BFF / Struts2 Admin）を停止
REM =========================================
echo Stopping Java services...

REM ポートベースでプロセスを検索・停止
REM   8080: Web API
REM   8081: Mobile BFF
REM   8082: Struts2 Admin Application
for %%P in (8080 8081 8082) do (
    for /f "tokens=5" %%A in ('netstat -ano 2^>nul ^| findstr "LISTENING" ^| findstr ":%%P "') do (
        set "PID=%%A"
        if not "!PID!"=="0" (
            echo   Stopping process on port %%P ^(PID: !PID!^)...
            taskkill /PID !PID! /F >nul 2>&1
            if not errorlevel 1 (
                echo   [OK] Process on port %%P stopped
            ) else (
                echo   [WARN] Could not stop process on port %%P
            )
        )
    )
)
echo.

REM =========================================
REM  SQLite
REM =========================================
echo SQLite database: no shutdown required (file-based)
echo.

echo =========================================
echo   All Services Stopped!
echo =========================================
echo.

cd /d "%PROJECT_ROOT%"
endlocal
