@ECHO OFF

:: Check command arguments
IF "%1"=="/?" GOTO USAGE
IF "%1"=="all" GOTO ALL

:: Preare a development installer
SET CODELITE_INSTALLER=codelite-devel
"%ProgramFiles%\Inno Setup 5\iscc" lite_editor.iss /F%CODELITE_INSTALLER%

GOTO END

:ALL

IF "%2"=="" GOTO USAGE

:: Installer name for IDE only
SET CODELITE_INSTALLER=codelite-2.7.0.%2

:: Installer name for IDE + MinGW
SET CODELITE_INSTALLER_MINGW=codelite-2.7.0.%2-mingw4.4.1

:: Installer name for IDE + MinGW + WX2.8.10
SET CODELITE_INSTALLER_MINGW_WX=codelite-2.7.0.%2-mingw4.4.1-wx2.8.10

"%ProgramFiles%\Inno Setup 5\iscc" lite_editor.iss /F%CODELITE_INSTALLER%

"%ProgramFiles%\Inno Setup 5\iscc" codelite_mingw.iss /F%CODELITE_INSTALLER_MINGW%

"%ProgramFiles%\Inno Setup 5\iscc" codelite_mingw_wx.iss /F%CODELITE_INSTALLER_MINGW_WX%

GOTO END

:USAGE

echo Usage:
echo       make_packages.bat all [revision_number] - prepare a package for release
echo       make_packages.bat                       - prepare a developement installer (used to test the installer)

:END
