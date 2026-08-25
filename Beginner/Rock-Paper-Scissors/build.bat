@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo   Building Rock-Paper-Scissors Tournament Arena (C++17)
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

:: Terminate any running instances to release file locks
taskkill /f /im rps_game.exe >nul 2>&1
taskkill /f /im rps_tests.exe >nul 2>&1

:: Create bin directory
if not exist bin mkdir bin

echo Compiling Main Application (rps_game.exe)...
cl /nologo /std:c++17 /EHsc /O2 /W3 /utf-8 /guard:cf /MD /I src ^
   src\main.cpp src\Terminal.cpp src\InputValidator.cpp src\RandomGenerator.cpp src\RulesEngine.cpp src\AiPredictor.cpp src\GameTheoryAnalyzer.cpp src\StatsManager.cpp src\AsciiArt.cpp src\GameEngine.cpp ^
   /Fe:bin\rps_game.exe /Fo:bin\ ^
   /link /DYNAMICBASE /NXCOMPAT /GUARD:CF /MANIFEST /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Main application compilation failed!
    exit /b %ERRORLEVEL%
)

echo Compiling Test Suite (rps_tests.exe)...
cl /nologo /std:c++17 /EHsc /O2 /W3 /utf-8 /guard:cf /MD /I src ^
   tests\test_runner.cpp src\Terminal.cpp src\InputValidator.cpp src\RandomGenerator.cpp src\RulesEngine.cpp src\AiPredictor.cpp src\GameTheoryAnalyzer.cpp src\StatsManager.cpp src\AsciiArt.cpp ^
   /Fe:bin\rps_tests.exe /Fo:bin\ ^
   /link /DYNAMICBASE /NXCOMPAT /GUARD:CF /MANIFEST /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Test suite compilation failed!
    exit /b %ERRORLEVEL%
)

:: Clean up temporary intermediate compiler artifacts
del /q bin\*.obj bin\*.manifest >nul 2>&1

:: Unblock binaries if tagged by OneDrive/Internet zones
powershell -Command "Get-Item 'bin\*.exe' -ErrorAction SilentlyContinue | ForEach-Object { Unblock-File -Path $_.FullName -ErrorAction SilentlyContinue }" >nul 2>&1

echo.
echo [SUCCESS] Build completed successfully!
echo Executables created:
echo   - bin\rps_game.exe
echo   - bin\rps_tests.exe
echo.
