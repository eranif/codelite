@echo off
::Cleanup if there is an old update.
if exist update rmdir /S /Q update

:: Copy the folders
xcopy config\*.default "update\config\" /E /I /H /Y /EXCLUDE:excludes
xcopy debuggers\*.dll "update\debuggers\" /E /I /H /Y /EXCLUDE:excludes
xcopy images\* "update\images\" /E /I /H /Y /EXCLUDE:excludes
xcopy lexers\*.xml "update\lexers\" /E /I /H /Y /EXCLUDE:excludes
xcopy plugins\*.dll "update\plugins\" /E /I /H /Y /EXCLUDE:excludes
xcopy rc\*.xrc "update\rc\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\* "update\templates\" /E /I /H /Y /EXCLUDE:excludes
xcopy *.html "update\" /H /Y /EXCLUDE:excludes
:: Copy the UnitTest++ files
xcopy sdk\lib\win32\*.a "update\sdk\lib\" /Y
xcopy sdk\include\UnitTest++\*.h "update\sdk\include\UnitTest++\" /Y
xcopy sdk\include\UnitTest++\win32\*.h "update\sdk\include\UnitTest++\win32\" /Y
:: Copy the misc files
copy ctags-le.exe "update\" /Y
copy makedir.exe "update\" /Y
copy le_exec.exe "update\" /Y
copy astyle.sample "update\" /Y
copy CodeLite.exe "update\" /Y
copy wxmsw28u_gcc*.dll "update\" /Y
copy libsqlite3.dll "update\" /Y
copy ..\InnoSetup\license.txt "update\" /Y
copy ..\sdk\wxconfig\wx-config.exe "update\" /Y
copy ..\sdk\curl\lib\libcurl-4.dll "update\" /Y
