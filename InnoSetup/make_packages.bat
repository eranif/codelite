@ECHO OFF

:: Check command arguments
IF "%1"=="" GOTO USAGE

:: Installer name for IDE only
SET CODELITE_INSTALLER=codelite-2.0.%1

:: Installer name for IDE + MinGW
SET CODELITE_INSTALLER_MINGW=codelite-2.0.%1-mingw4.4.0

:: Installer name for IDE + MinGW + WX2.8.10
SET CODELITE_INSTALLER_MINGW_WX=codelite-2.0.%1-mingw4.4.0-wx2.8.10

::"%ProgramFiles%\Inno Setup 5\iscc" lite_editor.iss /F%CODELITE_INSTALLER%

"%ProgramFiles%\Inno Setup 5\iscc" codelite_mingw.iss /F%CODELITE_INSTALLER_MINGW%

::"%ProgramFiles%\Inno Setup 5\iscc" codelite_mingw_wx.iss /F%CODELITE_INSTALLER_MINGW_WX%

GOTO END

:USAGE
echo Usage: make_packages.bat [revision_number]

:END