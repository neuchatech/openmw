@echo off
if defined VCToolsInstallDir goto :build

echo Searching for vcvars64.bat...

:: Check Community Edition
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "vcvars=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    goto :found
)

:: Check Enterprise Edition
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "vcvars=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    goto :found
)

:: Check Professional Edition
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "vcvars=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    goto :found
)

:: Check Build Tools
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
    set "vcvars=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    goto :found
)

echo WARNING: vcvars64.bat not found. Ensure you are running from a Developer Command Prompt.
goto :build

:found
echo Found "%vcvars%"
call "%vcvars%"

:build
echo Building OpenMW from MSVC2022_64_Ninja...
cmake --build MSVC2022_64_Ninja --parallel
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ****************************************
    echo *           BUILD SUCCESSFUL           *
    echo ****************************************
) else (
    echo.
    echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    echo !             BUILD FAILED             !
    echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)
pause
