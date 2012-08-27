@ECHO OFF

:: Check command arguments
IF "%1"=="/?" GOTO USAGE
IF "%1"=="all" GOTO ALL
IF "%1"=="win7" GOTO WIN7

:: Preare a development installer
SET CODELITE_INSTALLER=codelite-devel
"%ProgramFiles(x86)%\Inno Setup 5\iscc" lite_editor.iss /F%CODELITE_INSTALLER%

GOTO END

:WIN7

:: Preare a development installer for Windows 7
SET CODELITE_INSTALLER=codelite-devel
"%ProgramFiles(x86)%\Inno Setup 5\iscc" lite_editor_win7.iss /F%CODELITE_INSTALLER%

GOTO END

:ALL

IF "%2"=="" GOTO USAGE

:: Installer name for IDE only
SET CODELITE_INSTALLER=codelite-4.1.%2

:: Installer name for IDE + MinGW
SET CODELITE_INSTALLER_MINGW=codelite-4.1.%2-mingw4.6.1

:: Installer name for IDE + MinGW + WX2.9.4
SET CODELITE_INSTALLER_MINGW_WX=codelite-4.1.%2-mingw4.6.1-wx2.9.4

"%ProgramFiles(x86)%\Inno Setup 5\iscc" lite_editor.iss /F%CODELITE_INSTALLER%

"%ProgramFiles(x86)%\Inno Setup 5\iscc" codelite_mingw.iss /F%CODELITE_INSTALLER_MINGW%

"%ProgramFiles(x86)%\Inno Setup 5\iscc" codelite_mingw_wx.iss /F%CODELITE_INSTALLER_MINGW_WX%

GOTO END

:USAGE

echo Usage:
echo       make_packages.bat all [revision_number] - prepare a package for release
echo       make_packages.bat                       - prepare a developement installer (used to test the installer)

:END
