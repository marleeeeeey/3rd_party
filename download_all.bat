@echo off
setlocal

@REM Get the directory where the script is located
set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%"

echo Running cross-platform library download and build script...

@REM Execute the CMake script
cmake -P download_all.cmake

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Library installation failed!
@REM     pause
    exit /b %ERRORLEVEL%
)

echo.
echo [SUCCESS] All libraries have been downloaded and installed to external_install/
@REM pause