@echo off
chcp 65001 >nul 2>&1
REM =========================================
REM  reset-database.bat
REM  データベースをリセットして初期データを再投入するスクリプト (Windows)
REM =========================================

setlocal enabledelayedexpansion

echo =========================================
echo   Resetting Database (SQLite)
echo =========================================
echo.

REM プロジェクトルートの取得
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."

REM SQLiteデータベースファイルのパス
set "DB_DIR=%PROJECT_ROOT%\data"
set "DB_FILE=%DB_DIR%\mobile_app.db"

REM dataディレクトリを作成
if not exist "%DB_DIR%" (
    mkdir "%DB_DIR%"
)

REM 既存のデータベースファイルを削除
if exist "%DB_FILE%" (
    echo Removing existing database...
    del /f "%DB_FILE%"
    echo [OK] Old database removed
    echo.
)

REM sqlite3コマンドの確認
where sqlite3 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] sqlite3 command not found!
    echo.
    echo   Please install SQLite3:
    echo     1. Download from https://www.sqlite.org/download.html
    echo        sqlite-tools-win-x64-XXXXXXX.zip
    echo     2. Extract sqlite3.exe to a directory in your PATH
    echo        e.g., C:\tools or C:\Windows\System32
    echo.
    echo   Or install via winget:
    echo     winget install SQLite.SQLite
    echo.
    echo   Or install via scoop:
    echo     scoop install sqlite
    exit /b 1
)

REM スキーマ作成
echo Creating schema...
set "SCHEMA_FILE=%PROJECT_ROOT%\src\database\schema\01_create_tables.sql"
if exist "%SCHEMA_FILE%" (
    sqlite3 "%DB_FILE%" < "%SCHEMA_FILE%"
    if errorlevel 1 (
        echo [ERROR] Failed to create schema!
        exit /b 1
    )
    echo [OK] Schema created
) else (
    echo [ERROR] 01_create_tables.sql not found!
    echo   Expected: %SCHEMA_FILE%
    exit /b 1
)
echo.

REM 初期データを投入
echo Inserting seed data...
set "SEED_FILE=%PROJECT_ROOT%\src\database\seeds\02_seed_data.sql"
if exist "%SEED_FILE%" (
    sqlite3 "%DB_FILE%" < "%SEED_FILE%"
    if errorlevel 1 (
        echo [ERROR] Failed to insert seed data!
        exit /b 1
    )
    echo [OK] Seed data inserted
) else (
    echo [ERROR] 02_seed_data.sql not found!
    echo   Expected: %SEED_FILE%
    exit /b 1
)
echo.

REM データ確認
echo Verifying data...
sqlite3 -header -column "%DB_FILE%" "SELECT 'users' as table_name, COUNT(*) as count FROM users UNION ALL SELECT 'products', COUNT(*) FROM products UNION ALL SELECT 'feature_flags', COUNT(*) FROM feature_flags UNION ALL SELECT 'purchases', COUNT(*) FROM purchases UNION ALL SELECT 'favorites', COUNT(*) FROM favorites UNION ALL SELECT 'user_feature_flags', COUNT(*) FROM user_feature_flags UNION ALL SELECT 'product_price_history', COUNT(*) FROM product_price_history;"
echo.

echo =========================================
echo   Database Reset Complete!
echo =========================================
echo.
echo Database file: %DB_FILE%
echo.

endlocal
