@echo off
setlocal enabledelayedexpansion

echo ============================================================
echo  WsDemoMobileApp - Full Build Script
echo ============================================================
echo.

REM --- Set up build environment ---
REM Try vcvars64.bat from various VS installations, verify cl.exe is available
set "VCVARS="
for %%V in (
    "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
) do (
    if exist %%V (
        set "VCVARS=%%~V"
        goto :found_vcvars
    )
)
goto :no_vcvars

:found_vcvars
echo Setting up environment via: %VCVARS%
call "%VCVARS%" >NUL 2>&1
where cl.exe >NUL 2>&1
if %ERRORLEVEL% EQU 0 goto :env_ready

:no_vcvars
echo vcvars64.bat did not provide cl.exe. Searching for MSVC installations...
set "MSVC_ROOT="
for /d %%D in ("C:\Program Files\Microsoft Visual Studio\*") do (
    for /d %%E in ("%%D\*") do (
        if exist "%%E\VC\Tools\MSVC" (
            for /d %%M in ("%%E\VC\Tools\MSVC\*") do (
                if exist "%%M\bin\Hostx64\x64\cl.exe" if exist "%%M\include\string" set "MSVC_ROOT=%%M"
            )
        )
    )
)
if not defined MSVC_ROOT (
    echo ERROR: No MSVC installation with cl.exe found.
    exit /b 1
)
set "WINSDK_ROOT=C:\Program Files (x86)\Windows Kits\10"
for /d %%S in ("%WINSDK_ROOT%\Include\10.*") do set "WINSDK_VER=%%~nxS"
set "PATH=!MSVC_ROOT!\bin\Hostx64\x64;!WINSDK_ROOT!\bin\!WINSDK_VER!\x64;%PATH%"
set "INCLUDE=!MSVC_ROOT!\include;!WINSDK_ROOT!\Include\!WINSDK_VER!\ucrt;!WINSDK_ROOT!\Include\!WINSDK_VER!\um;!WINSDK_ROOT!\Include\!WINSDK_VER!\shared"
set "LIB=!MSVC_ROOT!\lib\x64;!WINSDK_ROOT!\Lib\!WINSDK_VER!\ucrt\x64;!WINSDK_ROOT!\Lib\!WINSDK_VER!\um\x64"

:env_ready
REM --- Set project root to the directory containing this script ---
set "PROJDIR=%~dp0"
cd /d "%PROJDIR%"

echo [1/4] Build environment configured.
for /f "delims=" %%C in ('where cl.exe') do echo       Compiler : %%C& goto :show_proj
:show_proj
echo       Project  : %PROJDIR%
echo.

REM --- Create output directories ---
echo [2/4] Creating output directories...
if not exist obj mkdir obj
if not exist bin mkdir bin

REM --- Compiler flags ---
set "CFLAGS=/std:c++latest /EHsc /W3 /c /nologo /utf-8 /MDd"
set "DEFINES=/DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0A00 /DWINVER=0x0A00"
set "PROJ_INCLUDES=/I"include" /I"src""
set "DEBUG=/Od /Zi /D_DEBUG"
set "OBJDIR=obj"

REM --- List of all .cpp source files ---
set SOURCES=^
 src\main.cpp^
 src\App.cpp^
 src\Services\HttpClient.cpp^
 src\Services\AuthService.cpp^
 src\Services\ProductService.cpp^
 src\Services\PurchaseService.cpp^
 src\Services\FavoriteService.cpp^
 src\Services\StatePollingService.cpp^
 src\Utils\CredentialManager.cpp^
 src\ViewModels\LoginViewModel.cpp^
 src\ViewModels\ProductListViewModel.cpp^
 src\ViewModels\ProductDetailViewModel.cpp^
 src\ViewModels\PurchaseViewModel.cpp^
 src\ViewModels\FavoriteViewModel.cpp^
 src\Views\LoginWindow.cpp^
 src\Views\ProductListWindow.cpp^
 src\Views\ProductDetailWindow.cpp^
 src\Views\PurchaseWindow.cpp^
 src\Views\FavoriteWindow.cpp

REM --- Compile each source file ---
echo.
echo [3/4] Compiling source files...
set COMPILE_ERRORS=0
set OBJ_FILES=

for %%F in (%SOURCES%) do (
    REM Extract just the filename without extension for .obj
    set "SRCFILE=%%F"
    for %%N in (%%~nF) do set "OBJNAME=%%N"

    echo   Compiling: %%F
    cl.exe %CFLAGS% %DEFINES% %PROJ_INCLUDES% %DEBUG% "%%F" /Fo"%OBJDIR%\!OBJNAME!.obj" /Fd"%OBJDIR%\!OBJNAME!.pdb" 2>&1
    if !ERRORLEVEL! NEQ 0 (
        echo   *** FAILED: %%F
        set /a COMPILE_ERRORS+=1
    )
    set "OBJ_FILES=!OBJ_FILES! %OBJDIR%\!OBJNAME!.obj"
)

echo.
if %COMPILE_ERRORS% GTR 0 (
    echo === COMPILATION FAILED: %COMPILE_ERRORS% files had errors ===
    exit /b 1
)
echo === All files compiled successfully ===

REM --- Compile resource file ---
echo.
echo   Compiling resource: resources\app.rc
rc.exe /nologo /I"resources" /fo "%OBJDIR%\app.res" "resources\app.rc" 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   WARNING: Resource compilation failed, continuing without resources.
)

REM --- Link ---
echo.
echo [4/4] Linking...
set "LIBS=winhttp.lib advapi32.lib comctl32.lib user32.lib gdi32.lib shell32.lib ole32.lib kernel32.lib"
set "LINK_FLAGS=/SUBSYSTEM:WINDOWS /DEBUG /NOLOGO"

set "RES_FILE="
if exist "%OBJDIR%\app.res" set "RES_FILE=%OBJDIR%\app.res"

link.exe %LINK_FLAGS% /OUT:"bin\WsDemoMobileApp.WindowsApp.exe" %OBJ_FILES% %RES_FILE% %LIBS% 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo === LINK FAILED ===
    exit /b 1
)

echo.
echo ============================================================
echo  BUILD SUCCEEDED
echo  Output: bin\WsDemoMobileApp.WindowsApp.exe
echo ============================================================

endlocal
exit /b 0
