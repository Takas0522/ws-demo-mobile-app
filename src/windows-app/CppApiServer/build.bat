@echo off
chcp 65001 >nul 2>&1
setlocal enabledelayedexpansion

echo ============================================================
echo  CppApiServer Build Script
echo ============================================================
echo.

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found
    exit /b 1
)

for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe"`) do (
    set "MSBUILD=%%i"
)

if not defined MSBUILD (
    echo [ERROR] MSBuild.exe not found
    exit /b 1
)

echo [INFO] MSBuild: %MSBUILD%

set "PROJECT_FILE=%~dp0CppApiServer.vcxproj"
set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"

echo [INFO] Building CppApiServer ^(%CONFIG%^|x64^)...
echo.

"%MSBUILD%" "%PROJECT_FILE%" /p:Configuration=%CONFIG% /p:Platform=x64 /p:ResolveNuGetPackages=false /verbosity:minimal
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] BUILD FAILED
    exit /b 1
)

echo.
echo [INFO] BUILD SUCCEEDED
echo [INFO] Output: %~dp0bin\%CONFIG%\x64\
exit /b 0
