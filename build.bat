@echo off
setlocal

:: ------------------------------------------------------------------------------
:: Common
:: ------------------------------------------------------------------------------
set SRC=enniolimpiadi.c
set BUILD_DIR=build
set DEBUG_OUT=enniolimpiadi-debug.exe
set RELEASE_OUT=enniolimpiadi-release.exe

:: ------------------------------------------------------------------------------
:: Raylib paths - adjust if installed elsewhere
:: ------------------------------------------------------------------------------
set RAYLIB_PATH=C:\raylib
set RAYLIB_INCLUDE=-I%RAYLIB_PATH%\include
set RAYLIB_LIB=%RAYLIB_PATH%\lib\libraylib.a

:: Windows system libraries required by raylib
set WIN_LIBS=-lopengl32 -lgdi32 -lwinmm -luser32 -lshell32

:: ------------------------------------------------------------------------------
:: Compiler flags
:: ------------------------------------------------------------------------------
set DEBUG_CFLAGS=-std=c99 -Wall -Wconversion -pedantic -g -O0
set RELEASE_CFLAGS=-std=c99 -Wall -Wconversion -pedantic -O2

:: ------------------------------------------------------------------------------
:: Parse command
:: ------------------------------------------------------------------------------
if "%1"=="" goto :no_command
if "%1"=="debug" goto :debug
if "%1"=="release" goto :release
if "%1"=="clean" goto :clean
goto :unknown

:: ------------------------------------------------------------------------------
:: Commands
:: ------------------------------------------------------------------------------
:no_command
echo No command provided.
goto :help

:unknown
echo Unknown command: %1
goto :help

:help
echo Usage: build.bat [debug^|release^|clean]
echo.
echo   debug     Compile %SRC% -^> %DEBUG_OUT% (with debug symbols)
echo   release   Compile %SRC% -^> %RELEASE_OUT% (optimized)
echo   clean     Remove the build directory
echo.
exit /b 1

:clean
echo Cleaning build directory...
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
echo Cleaned.
exit /b 0

:debug
call :ensure_build_dir
echo Compiling %SRC% -^> %DEBUG_OUT%
gcc %DEBUG_CFLAGS% %SRC% -o %BUILD_DIR%\%DEBUG_OUT% %RAYLIB_INCLUDE% %RAYLIB_LIB% %WIN_LIBS%
goto :check_result

:release
call :ensure_build_dir
echo Compiling %SRC% -^> %RELEASE_OUT% (release build)
gcc %RELEASE_CFLAGS% %SRC% -o %BUILD_DIR%\%RELEASE_OUT% %RAYLIB_INCLUDE% %RAYLIB_LIB% %WIN_LIBS%
goto :check_result

:: ------------------------------------------------------------------------------
:: Helpers
:: ------------------------------------------------------------------------------
:ensure_build_dir
if not exist %BUILD_DIR% (
    echo Creating build directory...
    mkdir %BUILD_DIR%
)
exit /b 0

:check_result
if %errorlevel%==0 (
    echo Build successful.
    exit /b 0
) else (
    echo Build failed.
    exit /b 1
)
