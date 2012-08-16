@echo off
xcopy config\*.default "%ProgramFiles(x86)%\CodeLite\config\" /E /I /H /Y /EXCLUDE:excludes
xcopy config\build_settings.xml.default.win "%ProgramFiles(x86)%\CodeLite\config\build_settings.xml.default" /E /I /H /Y /EXCLUDE:excludes
xcopy debuggers\*.dll "%ProgramFiles(x86)%\CodeLite\debuggers\" /E /I /H /Y /EXCLUDE:excludes
xcopy images\* "%ProgramFiles(x86)%\CodeLite\images\" /E /I /H /Y /EXCLUDE:excludes
xcopy lexers\*.xml "%ProgramFiles(x86)%\CodeLite\lexers\" /E /I /H /Y /EXCLUDE:excludes
xcopy plugins\*.dll "%ProgramFiles(x86)%\CodeLite\plugins\" /E /I /H /Y /EXCLUDE:excludes
xcopy lib*.dll "%ProgramFiles(x86)%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy plugins\resources\*.* "%ProgramFiles(x86)%\CodeLite\plugins\resources\" /E /I /H /Y /EXCLUDE:excludes
xcopy rc\*.xrc "%ProgramFiles(x86)%\CodeLite\rc\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\ "%ProgramFiles(x86)%\CodeLite\templates\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\formbuilder\* "%ProgramFiles(x86)%\CodeLite\templates\formbuilder\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\projects\* "%ProgramFiles(x86)%\CodeLite\templates\projects\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\gizmos\* "%ProgramFiles(x86)%\CodeLite\templates\gizmos\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\codedesigner\* "%ProgramFiles(x86)%\CodeLite\templates\codedesigner\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\databaselayer\* "%ProgramFiles(x86)%\CodeLite\templates\databaselayer\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\qmake\* "%ProgramFiles(x86)%\CodeLite\templates\qmake\" /E /I /H /Y /EXCLUDE:excludes
xcopy *.html "%ProgramFiles(x86)%\CodeLite\" /H /Y /EXCLUDE:excludes
xcopy codelite-icons.zip "%ProgramFiles(x86)%\CodeLite\" /H /Y /EXCLUDE:excludes
xcopy codelite-icons-fresh-farm.zip "%ProgramFiles(x86)%\CodeLite\" /H /Y /EXCLUDE:excludes
xcopy locale\* "%ProgramFiles(x86)%\CodeLite\locale\" /E /I /H /Y /EXCLUDE:excludes
xcopy ..\lib\gcc_lib\libdatabaselayersqlite*.dll "%ProgramFiles(x86)%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy ..\lib\gcc_lib\libwxshapeframework*.dll "%ProgramFiles(x86)%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes

if "%WXWIN%" == "" GOTO OTHERS
xcopy %WXWIN%\lib\gcc_dll\wxmsw*u_gcc_cl.dll "%ProgramFiles(x86)%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy %WXWIN%\lib\gcc_dll\wxbase*u_gcc_cl.dll "%ProgramFiles(x86)%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes
xcopy %WXWIN%\lib\gcc_dll\wxrc.exe "%ProgramFiles(x86)%\CodeLite\" /E /I /H /Y /EXCLUDE:excludes

:OTHERS
:: Copy the misc files
copy codelite_indexer.exe "%ProgramFiles(x86)%\CodeLite\" /Y
copy codelite_cppcheck.exe "%ProgramFiles(x86)%\CodeLite\" /Y
copy codelite_launcher.exe "%ProgramFiles(x86)%\CodeLite\" /Y
copy ..\sdk\clang\lib\clang.dll "%ProgramFiles(x86)%\CodeLite\" /Y
copy makedir.exe "%ProgramFiles(x86)%\CodeLite\" /Y
copy le_exec.exe "%ProgramFiles(x86)%\CodeLite\" /Y
copy CodeLite.exe "%ProgramFiles(x86)%\CodeLite\" /Y
copy codelite-clang.exe "%ProgramFiles(x86)%\CodeLite\" /Y
copy patch.exe "%ProgramFiles(x86)%\CodeLite\" /Y
copy rm.exe "%ProgramFiles(x86)%\CodeLite\" /Y
copy astyle.sample "%ProgramFiles(x86)%\CodeLite\" /Y
copy pthreadGC2.dll "%ProgramFiles(x86)%\CodeLite\" /Y
copy ..\sdk\wxconfig\wx-config.exe "%ProgramFiles(x86)%\CodeLite\" /Y
