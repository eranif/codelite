@echo off
xcopy bin\*.exe "C:\Program Files\CodeLite\bin\" /E /I /H /Y /EXCLUDE:excludes
xcopy config\*.default "C:\Program Files\CodeLite\config\" /E /I /H /Y /EXCLUDE:excludes
xcopy debuggers\*.dll "C:\Program Files\CodeLite\debuggers\" /E /I /H /Y /EXCLUDE:excludes
xcopy images\* "C:\Program Files\CodeLite\images\" /E /I /H /Y /EXCLUDE:excludes
xcopy lexers\*.xml "C:\Program Files\CodeLite\lexers\Default\" /E /I /H /Y /EXCLUDE:excludes
xcopy plugins\*.dll "C:\Program Files\CodeLite\plugins\" /E /I /H /Y /EXCLUDE:excludes
xcopy rc\*.xrc "C:\Program Files\CodeLite\rc\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\ "C:\Program Files\CodeLite\templates\" /E /I /H /Y /EXCLUDE:excludes
xcopy *.html "C:\Program Files\CodeLite\" /H /Y /EXCLUDE:excludes
:: Copy the misc files
xcopy ..\sdk\curl\lib\libcurl-4.dll "C:\Program Files\CodeLite\" /Y
xcopy ..\sdk\curl\lib\libcurl-4.dll . /Y
xcopy astyle.sample "C:\Program Files\CodeLite\" /Y
xcopy CodeLite.exe "C:\Program Files\CodeLite\" /Y
