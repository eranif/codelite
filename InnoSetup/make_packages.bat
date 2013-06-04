@ECHO OFF

:: Check command arguments
IF "%1"=="/?" GOTO USAGE
IF "%1"=="all" GOTO ALL

:: Preare a development installer
SET CODELITE_INSTALLER=codelite-devel
"D:\software\Inno Setup 5\iscc" lite_editor.iss /F%CODELITE_INSTALLER%

GOTO END

:ALL

IF "%2"=="" GOTO USAGE

:: Installer name for IDE + MinGW
SET CODELITE_INSTALLER_MINGW=codelite-5.2.%2-mingw4.7.1

"D:\software\Inno Setup 5\iscc" codelite_mingw.iss /F%CODELITE_INSTALLER_MINGW%

GOTO END

:USAGE

echo Usage:
echo       make_packages.bat all [revision_number] - prepare a package for release
echo       make_packages.bat                       - prepare a developement installer (used to test the installer)

:END
