@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo   Building To-Do List Manager (C++17)
echo ========================================================

:: Locate MSVC environment
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if not exist "!VCVARS!" (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
)
if not exist "!VCVARS!" (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
)
if not exist "!VCVARS!" (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
)

if exist "!VCVARS!" (
    echo Initializing Visual Studio Build Tools...
    call "!VCVARS!" >nul 2>&1
)

:: Terminate running processes to release locks
taskkill /f /im todo_manager.exe >nul 2>&1
taskkill /f /im todo_tests.exe >nul 2>&1

if not exist bin mkdir bin

echo Compiling Main Application (todo_manager.exe)...
cl /nologo /std:c++17 /EHsc /O2 /W3 /utf-8 /I src ^
   src\main.cpp src\Storage.cpp src\TaskManager.cpp src\UI.cpp ^
   /Fe:bin\todo_manager.exe /Fo:bin\ ^
   /link /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Main application compilation failed!
    exit /b %ERRORLEVEL%
)

echo Compiling Test Suite (todo_tests.exe)...
cl /nologo /std:c++17 /EHsc /O2 /W3 /utf-8 /I src ^
   tests\test_runner.cpp src\Storage.cpp src\TaskManager.cpp ^
   /Fe:bin\todo_tests.exe /Fo:bin\ ^
   /link /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Test suite compilation failed!
    exit /b %ERRORLEVEL%
)

del /q bin\*.obj >nul 2>&1

echo.
echo [SUCCESS] Build completed successfully!
echo Executables created:
echo   - bin\todo_manager.exe
echo   - bin\todo_tests.exe
echo.
