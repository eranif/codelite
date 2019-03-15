@echo OFF

set TARGET_DIR=%ProgramFiles%
set COPY_WXC_RESOURCES=0
::IF EXIST D:\software\NUL (set TARGET_DIR=D:\software)
IF EXIST ..\wxcrafter\wxcrafter.accelerators (set COPY_WXC_RESOURCES=1)

xcopy config\*.default "%TARGET_DIR%\CodeLite\config\" /E /I /H /Y /EXCLUDE:excludes
xcopy config\build_settings.xml.default.win "%TARGET_DIR%\CodeLite\config\build_settings.xml.default" /E /I /H /Y /EXCLUDE:excludes
xcopy debuggers\*.dll "%TARGET_DIR%\CodeLite\debuggers\" /E /I /H /Y /EXCLUDE:excludes
xcopy images\* "%TARGET_DIR%\CodeLite\images\" /E /I /H /Y /EXCLUDE:excludes
xcopy lexers\*.json "%TARGET_DIR%\CodeLite\lexers\" /E /I /H /Y /EXCLUDE:excludes
xcopy plugins\*.dll "%TARGET_DIR%\CodeLite\plugins\" /E /I /H /Y /EXCLUDE:excludes
xcopy ..\SpellChecker\dics\* "%TARGET_DIR%\CodeLite\dics\" /E /I /H /Y /EXCLUDE:excludes
xcopy lib*.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy plugins\resources\*.* "%TARGET_DIR%\CodeLite\plugins\resources\" /E /I /H /Y /EXCLUDE:excludes
xcopy rc\*.xrc "%TARGET_DIR%\CodeLite\rc\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\ "%TARGET_DIR%\CodeLite\templates\" /E /I /H /Y /EXCLUDE:excludes
xcopy gdb_printers\*.py "%TARGET_DIR%\CodeLite\gdb_printers\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\formbuilder\* "%TARGET_DIR%\CodeLite\templates\formbuilder\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\projects\* "%TARGET_DIR%\CodeLite\templates\projects\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\gizmos\* "%TARGET_DIR%\CodeLite\templates\gizmos\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\codedesigner\* "%TARGET_DIR%\CodeLite\templates\codedesigner\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\databaselayer\* "%TARGET_DIR%\CodeLite\templates\databaselayer\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\qmake\* "%TARGET_DIR%\CodeLite\templates\qmake\" /E /I /H /Y /EXCLUDE:excludes
xcopy *.html "%TARGET_DIR%\CodeLite\" /H /Y /EXCLUDE:excludes
xcopy codelite-bitmaps-dark.zip "%TARGET_DIR%\CodeLite\" /H /Y /EXCLUDE:excludes
xcopy codelite-bitmaps-light.zip "%TARGET_DIR%\CodeLite\" /H /Y /EXCLUDE:excludes
xcopy locale\* "%TARGET_DIR%\CodeLite\locale\" /E /I /H /Y /EXCLUDE:excludes
xcopy ..\lib\gcc_lib\libdatabaselayersqlite*.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy ..\lib\gcc_lib\libwxshapeframework*.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
IF EXIST wxgui.zip ( copy wxgui.zip "%TARGET_DIR%\CodeLite\" )
IF EXIST PHP.zip ( copy PHP.zip "%TARGET_DIR%\CodeLite\" )

if "%WXWIN%" == "" GOTO OTHERS
xcopy %WXWIN%\lib\gcc_dll\wxmsw*u_*gcc_cl.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy %WXWIN%\lib\gcc_dll\wxbase*u_*gcc_cl.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
:: xcopy %WXWIN%\lib\gcc_dll\wxrc.exe "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes

if "%COPY_WXC_RESOURCES%" == "1" (copy ..\wxcrafter\wxcrafter.accelerators  "%TARGET_DIR%\CodeLite\plugins\resources" )

:OTHERS
:: Copy the misc files
copy codelite_indexer.exe "%TARGET_DIR%\CodeLite\" /Y
copy ..\LanguageServer\codelite-lsp\codelite-lsp-helper "%TARGET_DIR%\CodeLite\" /Y
copy codelite-cc.exe "%TARGET_DIR%\CodeLite\" /Y
copy codelite_cppcheck.exe "%TARGET_DIR%\CodeLite\" /Y
copy codelite_launcher.exe "%TARGET_DIR%\CodeLite\" /Y
copy codelite-echo.exe "%TARGET_DIR%\CodeLite\" /Y
copy ..\sdk\clang\lib\clang-format-64.exe "%TARGET_DIR%\CodeLite\codelite-clang-format.exe" /Y
copy ..\sdk\libssh\lib\libssh64.dll "%TARGET_DIR%\CodeLite\libssh.dll" /Y
copy makedir.exe "%TARGET_DIR%\CodeLite\" /Y
copy le_exec.exe "%TARGET_DIR%\CodeLite\" /Y
copy CodeLite.exe "%TARGET_DIR%\CodeLite\" /Y
copy codelite-make.exe "%TARGET_DIR%\CodeLite\" /Y
copy codelite-terminal.exe "%TARGET_DIR%\CodeLite\" /Y
copy patch.exe "%TARGET_DIR%\CodeLite\" /Y
copy rm.exe "%TARGET_DIR%\CodeLite\" /Y
copy astyle.sample "%TARGET_DIR%\CodeLite\" /Y
copy php.sample "%TARGET_DIR%\CodeLite\" /Y
copy pthreadGC2.dll "%TARGET_DIR%\CodeLite\" /Y
copy wx-config.exe "%TARGET_DIR%\CodeLite\" /Y

:END

echo CodeLite was updated into %TARGET_DIR%\CodeLite
