@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo   Building CLI Calculator ^& Unit Converter (C++17)
echo ========================================================

:: Locate MSVC environment
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if not exist "!VCVARS!" (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
)
if not exist "!VCVARS!" (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
)

if exist "!VCVARS!" (
    echo Initializing Visual Studio Build Tools...
    call "!VCVARS!" >nul 2>&1
)

:: Create bin directory
if not exist bin mkdir bin

echo Compiling Main Application (calculator.exe)...
cl /nologo /std:c++17 /EHsc /O2 /W3 /utf-8 /I src ^
   src\main.cpp src\UI.cpp src\Calculator.cpp src\UnitConverter.cpp src\ProgrammerCalc.cpp src\FinanceHealthCalc.cpp src\History.cpp ^
   /Fe:bin\calculator.exe /Fo:bin\

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Main application compilation failed!
    exit /b %ERRORLEVEL%
)

echo Compiling Test Suite (test_runner.exe)...
cl /nologo /std:c++17 /EHsc /O2 /W3 /utf-8 /I src ^
   tests\test_runner.cpp src\Calculator.cpp src\UnitConverter.cpp src\ProgrammerCalc.cpp src\FinanceHealthCalc.cpp ^
   /Fe:bin\test_runner.exe /Fo:bin\

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Test suite compilation failed!
    exit /b %ERRORLEVEL%
)

echo.
echo [SUCCESS] Build completed successfully!
echo Executables created:
echo   - bin\calculator.exe
echo   - bin\test_runner.exe
echo.
