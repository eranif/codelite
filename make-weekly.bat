@echo off

for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set "DEL=%%a"
)

:: Global environment variables
set CL_SRC_DIR=C:\src\codelite
set PATH=C:\Program Files\CodeLite;C:\Program Files (x86)\Inno Setup 6;%PATH%
set WXCFG=gcc_dll/mswu

:: Cleanup before we continue
del /Q %CL_SRC_DIR%\*.gch

:: Build the 32 bit version
call :ColorText 0A "Building CodeLite for i686"
call :MAKE_WEEKLY Win_x86_Release codelite_mingw.iss C:\src\wxWidgets32 C:\compilers\mingw64-i686\mingw32\bin

:: Build the 64 bit version
call :ColorText 0A "Building CodeLite for x86_64"
call :MAKE_WEEKLY Win_x64_Release codelite64_mingw.iss C:\src\wxWidgets C:\compilers\mingw64\bin

cd %CL_SRC_DIR%

call :ColorText 0A "Packaging Completed Successfuly!"
GOTO PACK_END

:: MAKE_WEEKLY method:
:: This function accepts 4 parameters:
:: @param configuration name
:: @param InnoSetup file name
:: @param wxWidgets directory 
:: @param the BIN folder for the compiler. We add this to the %PATH% environment variable
:MAKE_WEEKLY
    set CONFIG_NAME=%~1
    set ISCC_FILE=%~2
    set WXWIN=%~3
    set COMPILER_PATH=%~4
    set PATH=%WXWIN%\lib\gcc_dll;%COMPILER_PATH%;%PATH%
    cd %CL_SRC_DIR%
    
    if exist build.command del /Q build.command
    codelite-make.exe --workspace=CodeLiteIDE.workspace --config=%CONFIG_NAME% --project=CodeLiteIDE > build.command
    set /p BUILD_COMMAND=<build.command
    
    :: Execute the command
    echo Building CodeLite (config: %CONFIG_NAME%)
    %BUILD_COMMAND%
    
    echo Packging CodeLite (config: %CONFIG_NAME%)
    cd %CL_SRC_DIR%\InnoSetup
    if exist installer.txt del /Q installer.txt
    iscc %ISCC_FILE% > installer.txt
    
    :: Extract the installer file name from the output. It is the last line
    set INSTALLER_NAME=""
    FOR /f %%L IN (installer.txt) DO set INSTALLER_NAME=%%L
    
    echo Installer file: %INSTALLER_NAME%
    cd %CL_SRC_DIR%
    
    echo Uploading CodeLite (config: %CONFIG_NAME%)
    scp %INSTALLER_NAME% root@codelite.org:/var/www/html/downloads/codelite/wip
    exit /B 0

:ColorText
<nul set /p ".=%DEL%" > "%~2"
findstr /v /a:%1 /R "^$" "%~2" nul
del "%~2" > nul 2>&1
goto :eof

:PACK_END
    