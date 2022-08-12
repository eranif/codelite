echo OFF

set MSYS2_HOME=C:\msys64\home\eran
set TARGET_DIR=%ProgramFiles%
set COPY_WXC_RESOURCES=0
set CODELITE_SRC_DIR=C:\src\codelite
set LIB_DIR=%CODELITE_SRC_DIR%\build-Win_x64_Release\lib
set RUNTIME_DIR=%CODELITE_SRC_DIR%\Runtime
set WX_CONFIG_DIR=%MSYS2_HOME%\devl\wx-config-msys2\bin
set WXWIN=%MSYS2_HOME%\root
set LLVM_BIN=C:\LLVM\bin

::IF EXIST D:\software\NUL (set TARGET_DIR=D:\software)
IF EXIST ..\wxcrafter\wxcrafter.accelerators (set COPY_WXC_RESOURCES=1)

xcopy %RUNTIME_DIR%\config\*.default "%TARGET_DIR%\CodeLite\config\" /E /I /H /Y /EXCLUDE:excludes
xcopy %RUNTIME_DIR%\codelite-remote "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
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
IF EXIST %RUNTIME_DIR%\wxgui.zip ( copy wxgui.zip "%TARGET_DIR%\CodeLite\" )
IF EXIST %RUNTIME_DIR%\PHP.zip ( copy PHP.zip "%TARGET_DIR%\CodeLite\" )
xcopy %LIB_DIR%\*.dll "%TARGET_DIR%\CodeLite\plugins\" /E /I /H /Y /EXCLUDE:excludes

if "%WXWIN%" == "" GOTO OTHERS
xcopy %WXWIN%\lib\clang_x64_dll\wxmsw*u_*.dll "%TARGET_DIR%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes

if "%COPY_WXC_RESOURCES%" == "1" (copy ..\wxcrafter\wxcrafter.accelerators  "%TARGET_DIR%\CodeLite\plugins\resources" )

:OTHERS
:: Copy the misc files
copy %RUNTIME_DIR%\codelite_indexer.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite-cc.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite_cppcheck.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite-echo.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\ctagsd.exe "%TARGET_DIR%\CodeLite\" /Y
copy %CODELITE_SRC_DIR%\universal-ctags\win32\codelite-ctags.exe "%TARGET_DIR%\CodeLite\" /Y
copy %LLVM_BIN%\clang-format.exe "%TARGET_DIR%\CodeLite\codelite-clang-format.exe" /Y
copy %LLVM_BIN%\clangd.exe "%TARGET_DIR%\CodeLite\lsp\" /Y
copy %LLVM_BIN%\msvcp140.dll "%TARGET_DIR%\CodeLite\lsp\" /Y
copy %LLVM_BIN%\vcruntime140.dll "%TARGET_DIR%\CodeLite\lsp\" /Y
copy %LLVM_BIN%\vcruntime140_1.dll "%TARGET_DIR%\CodeLite\lsp\" /Y
copy %RUNTIME_DIR%\codelite-exec.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite-make.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\codelite-terminal.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\rm.exe "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\astyle.sample "%TARGET_DIR%\CodeLite\" /Y
copy %RUNTIME_DIR%\php.sample "%TARGET_DIR%\CodeLite\" /Y
copy %WX_CONFIG_DIR%\wx-config.exe "%TARGET_DIR%\CodeLite\" /Y

:END

echo CodeLite was updated into %TARGET_DIR%\CodeLite
date /T
time /T
