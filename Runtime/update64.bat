@echo OFF

set TARGET_DIR=%ProgramFiles%
set COPY_WXC_RESOURCES=0
set CODELITE_SRC_DIR=C:\src\codelite
set BIN_DIR=%CODELITE_SRC_DIR%\build-Win_x64_Release\bin
set LIB_DIR=%CODELITE_SRC_DIR%\build-Win_x64_Release\lib
set SDK_DIR==%CODELITE_SRC_DIR%\sdk
set RUNTIME_DIR=%CODELITE_SRC_DIR%\Runtime

::IF EXIST D:\software\NUL (set TARGET_DIR=D:\software)
IF EXIST ..\wxcrafter\wxcrafter.accelerators (set COPY_WXC_RESOURCES=1)

xcopy %RUNTIME_DIR%\config\*.default "%TARGET_DIR%\CodeLite\config\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\config\build_settings.xml.default.win "%TARGET_DIR%\CodeLite\config\build_settings.xml.default" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\debuggers\*.dll "%TARGET_DIR%\CodeLite\debuggers\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\images\* "%TARGET_DIR%\CodeLite\images\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\lexers\*.json "%TARGET_DIR%\CodeLite\lexers\" /E /I /H /Y /EXCLUDE:excludes
xcopy %CODELITE_SRC_DIR%\SpellChecker\dics\* "%TARGET_DIR%\CodeLite\dics\" /E /I /H /Y /EXCLUDE:excludes
xcopy %LIB_DIR%\lib*.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\plugins\resources\*.* "%TARGET_DIR%\CodeLite\plugins\resources\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\rc\*.xrc "%TARGET_DIR%\CodeLite\rc\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\templates\ "%TARGET_DIR%\CodeLite\templates\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\gdb_printers\*.py "%TARGET_DIR%\CodeLite\gdb_printers\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\templates\formbuilder\* "%TARGET_DIR%\CodeLite\templates\formbuilder\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\templates\projects\* "%TARGET_DIR%\CodeLite\templates\projects\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\templates\gizmos\* "%TARGET_DIR%\CodeLite\templates\gizmos\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\templates\codedesigner\* "%TARGET_DIR%\CodeLite\templates\codedesigner\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\templates\databaselayer\* "%TARGET_DIR%\CodeLite\templates\databaselayer\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\templates\qmake\* "%TARGET_DIR%\CodeLite\templates\qmake\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\*.html "%TARGET_DIR%\CodeLite\" /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\codelite-bitmaps-dark.zip "%TARGET_DIR%\CodeLite\" /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\codelite-bitmaps-light.zip "%TARGET_DIR%\CodeLite\" /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\locale\* "%TARGET_DIR%\CodeLite\locale\" /E /I /H /Y /EXCLUDE:excludes
xcopy %LIB_DIR%\libdatabaselayersqlite*.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy %LIB_DIR%\libwxshapeframework*.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
IF EXIST %RUNTIME_DIR%\wxgui.zip ( copy wxgui.zip "%TARGET_DIR%\CodeLite\" )
IF EXIST %RUNTIME_DIR%\PHP.zip ( copy PHP.zip "%TARGET_DIR%\CodeLite\" )
xcopy %LIB_DIR%\*.dll "%TARGET_DIR%\CodeLite\plugins\" /E /I /H /Y /EXCLUDE:excludes

if "%WXWIN%" == "" GOTO OTHERS
xcopy %WXWIN%\lib\gcc_dll\wxmsw*u_*gcc_cl.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy %WXWIN%\lib\gcc_dll\wxbase*u_*gcc_cl.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
:: xcopy %WXWIN%\lib\gcc_dll\wxrc.exe "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes

if "%COPY_WXC_RESOURCES%" == "1" (copy ..\wxcrafter\wxcrafter.accelerators  "%TARGET_DIR%\CodeLite\plugins\resources" )

:OTHERS
:: Copy the misc files
copy %RUNTIME_DIR%\codelite_indexer.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite-cc.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite_cppcheck.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite_launcher.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite-echo.exe "%TARGET_DIR%\CodeLite\" /Y
copy %SDK_DIR%\clang\lib\clang-format-64.exe "%TARGET_DIR%\CodeLite\codelite-clang-format.exe" /Y
copy %SDK_DIR%\libssh\lib\libssh.dll "%TARGET_DIR%\CodeLite\libssh.dll" /Y
copy %RUNTIME_DIR%\le_exec.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite-make.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite-terminal.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\rm.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\astyle.sample "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\php.sample "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\pthreadGC2.dll "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\wx-config.exe "%TARGET_DIR%\CodeLite\" /Y

:END

echo CodeLite was updated into %TARGET_DIR%\CodeLite
