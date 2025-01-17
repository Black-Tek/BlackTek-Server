@echo off
setlocal enabledelayedexpansion

set projectDir=%CD%

echo ::::::::::::::::::::::::::::::::::::::
echo ::::::::::::::::::::::::::::::::::::::
echo ::::: Black Tek Server Bootstrap :::::
echo :::::      (for windows)         :::::
echo ::::::::::::::::::::::::::::::::::::::
echo ::::::::::::::::::::::::::::::::::::::


:: Check if vcpkg is installed first.
where vcpkg >nul 2>&1
if %ERRORLEVEL%==0 (
    echo Vcpkg installation has been found!
    echo skipping vcpkg installation..
    goto premake_input
) else (
    echo Vcpkg installation has not been detected!
    echo moving forward with vpckg installation...
)

:vcpkg_input
echo Please provide the path to the directory you wish to store your vcpgk installation!
echo Warning!! This folder MUST ALREADY EXIST, or it won't be accepted.
echo Example input: C:\Packages\vcpkg
set /p VCPKG_INSTALL_PATH=Path: 

:: Remove quotes if user included them
set VCPKG_INSTALL_PATH=!VCPKG_INSTALL_PATH:"=!

:: Check if path exists
if not exist "!VCPKG_INSTALL_PATH!" (
    echo Error: The specified path does not exist.
    echo Please enter a valid path.
    goto vcpkg_input
)

cd "!VCPKG_INSTALL_PATH!"

:: Download and extract vcpkg
echo Downloading vcpkg...
curl -L "https://github.com/microsoft/vcpkg/archive/refs/heads/master.zip" -o vcpkg.zip || (
    echo Failed to download vcpkg.
    exit /b
)
tar -xf vcpkg.zip
del vcpkg.zip
ren "vcpkg-master" "vcpkg"

:: Setup vcpkg
echo Setting up vcpkg...
cd "!VCPKG_INSTALL_PATH!\vcpkg"
call bootstrap-vcpkg.bat || (
    echo Failed to bootstrap vcpkg.
    exit /b
)
vcpkg integrate install || (
    echo Failed to integrate vcpkg.
    exit /b
)
cd "!projectDir!"

:premake_input
echo Please enter a path to a folder for storing premake
echo Warning: This folder must already exist, or it won't be accepted.
echo Example: C:\premake
set /p PREMAKE_PATH=Path:

set PREMAKE_PATH=!PREMAKE_PATH:"=!

:: Check if path exists
if not exist "!PREMAKE_PATH!" (
    echo Error: The specified path does not exist.
    echo Please enter a valid path.
    goto premake_input
)

cd "!PREMAKE_PATH!"

:: Download and extract premake.
:: We are currently locked into a version of premake
:: which does not fail to build using its bootstrap.bat
:: but also supports our needed features.
echo Downloading premake...
curl -L "https://github.com/premake/premake-core/archive/490686ceb24b29f16c1ec817ed05c07c5cce89c6.zip" -o premake.zip || (
    echo Failed to download premake.
    exit /b
)
tar -xf premake.zip
del premake.zip
ren "premake-core-490686ceb24b29f16c1ec817ed05c07c5cce89c6" "premake-core"
set premakeBin=!PREMAKE_PATH!\premake-core\bin\release\premake5.exe

:: Build premake
echo Building premake...
cd premake-core
cmd /c Bootstrap.bat || (
    echo Premake build failed.
    exit /b
)
cls

:: Copy premake5.exe to BlackTek-Server
echo Copying premake5.exe...
if not exist "!premakeBin!" (
    echo Source file does not exist: "!premakeBin!"
    exit /b
)
if not exist "!projectDir!" (
    echo Destination directory does not exist: "!projectDir!"
    exit /b
)
copy "!premakeBin!" "!projectDir!" 
if errorlevel 1 (
    echo Failed to copy premake5.exe.
    echo Error level: %errorlevel%
    echo From: "!premakeBin!"
    echo To: "!projectDir!"
    exit /b
)

:select_vs_version
cls
echo Select Visual Studio version:
echo [1] Visual Studio 2022
echo [2] Visual Studio 2019
echo [3] Visual Studio 2015
echo [4] Visual Studio 2013
echo [5] Visual Studio 2012
echo.

set /p choice="Enter number (1-5): "

:: Set the vs_version variable based on user choice
if "%choice%"=="1" (
    set "vs_version=vs2022"
) else if "%choice%"=="2" (
    set "vs_version=vs2019"
) else if "%choice%"=="3" (
    set "vs_version=vs2015"
) else if "%choice%"=="4" (
    set "vs_version=vs2013"
) else if "%choice%"=="5" (
    set "vs_version=vs2012"
) else (
    echo Invalid selection. Please try again.
    timeout /t 2 >nul
    goto select_vs_version
)

:: Confirm selection
echo.
echo You selected Visual Studio %vs_version%
echo.
set /p confirm="Is this correct? (Y/N): "
if /i not "%confirm%"=="Y" goto select_vs_version

:: Run premake with selected version
echo.
echo Generating solution files for %vs_version%...
cd !projectDir!
cmd /c premake5.exe %vs_version%

if errorlevel 1 (
    echo Failed to generate solution files.
    echo Error level: %errorlevel%
    pause
    exit /b 1
)

echo Solution files generated successfully!
echo Setup complete. BlackTek has been successfully installed!
pause
exit
endlocal