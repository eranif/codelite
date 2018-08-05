::
:: Usage (64 bit):
::
:: make-weekly --upload --with-php
:: make-weekly --upload
:: make-weekly
::
:: Usage (32 bit):
:: make-weekly --upload X86
:: make-weekly --no-upload X86
::
@echo OFF
IF "%2"=="32" GOTO MAKE_X86
IF "%2"=="--with-php" GOTO MAKE_AMD64_PHP
GOTO MAKE_AMD64

:MAKE_AMD64
:: 64 bit version
set PATH=C:\Program Files\CodeLite;D:\software\Inno Setup 5;%PATH%
set WXCFG=gcc_dll/mswu
set WXWIN=D:\src\wx-src
set CL_CONFIG_NAME=Win_x64_Release
set WXC_CONFIG_NAME=Win_x64_Release
set UTILS_CONFIG_NAME=Win_x64_Release
set ISCC_FILE=codelite64_mingw.iss
set CYGWIN_BIN=D:\cygwin64\bin
GOTO COMMON

:MAKE_AMD64_PHP
:: 64 bit version
set PATH=C:\Program Files\CodeLite;D:\software\Inno Setup 5;%PATH%
set WXCFG=gcc_dll/mswu
set WXWIN=D:\src\wx-src
set CL_CONFIG_NAME=Win_x64_Release_PHP
set WXC_CONFIG_NAME=Win_x64_Release
set UTILS_CONFIG_NAME=Win_x64_Release
set ISCC_FILE=codelite64_php_mingw.iss
set CYGWIN_BIN=D:\cygwin64\bin
GOTO COMMON

:MAKE_X86
set PATH=C:\Program Files (x86)\CodeLite;C:\Program Files (x86)\Inno Setup 5;%PATH%
set WXCFG=gcc_dll/mswu
set WXWIN=C:/src/wxWidgets.git
set CL_CONFIG_NAME=Win_x86_Release
set WXC_CONFIG_NAME=Win_x64_Release
set UTILS_CONFIG_NAME=Win_x64_Release
set ISCC_FILE=codelite_mingw.iss
set CYGWIN_BIN=C:\cygwin\bin
GOTO COMMON

:COMMON
echo Building CodeLite (%CL_CONFIG_NAME%)
cd C:\src\codelite
codelite-make.exe --workspace=LiteEditor.workspace --project=CodeLiteIDE --config=%CL_CONFIG_NAME% --execute

echo Building wxCrafter (%WXC_CONFIG_NAME%)
cd C:\src\codelite\wxcrafter
codelite-make.exe --workspace=wxcrafter.workspace --project=wxcrafter --config=%WXC_CONFIG_NAME% --execute

echo Building Utils (%UTILS_CONFIG_NAME%)
cd C:\src\codelite\codelite_utils
codelite-make.exe  --workspace=codelite_utils.workspace --project=build_all --config=%UTILS_CONFIG_NAME% --execute

echo "Packaging..."
cd C:\src\codelite\InnoSetup
iscc %ISCC_FILE%

cd C:\src\codelite\InnoSetup\output
%CYGWIN_BIN%\bash.exe --login /cygdrive/c/src/codelite/InnoSetup/output/finalize-upload.sh "%1"
cd C:\src\codelite

:: pause
