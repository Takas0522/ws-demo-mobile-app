@echo off
chcp 65001 >nul 2>&1
REM =========================================
REM  stop-admin-struts.bat
REM  Struts2 管理画面アプリケーションの停止スクリプト (Windows)
REM =========================================

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."

echo Stopping Struts2 Admin Application...

REM ポート8082で動作しているプロセスを検索して停止
set "FOUND=0"
for /f "tokens=5" %%A in ('netstat -ano 2^>nul ^| findstr "LISTENING" ^| findstr ":8082 "') do (
    set "PID=%%A"
    if not "!PID!"=="0" (
        echo   Stopping process on port 8082 ^(PID: !PID!^)...
        taskkill /PID !PID! /F >nul 2>&1
        if not errorlevel 1 (
            echo   [OK] Process stopped
            set "FOUND=1"
        ) else (
            echo   [WARN] Could not stop process ^(PID: !PID!^)
        )
    )
)

if "!FOUND!"=="0" (
    echo   No process found on port 8082. Application may not be running.
)

echo   Stopped.

cd /d "%PROJECT_ROOT%"
endlocal
