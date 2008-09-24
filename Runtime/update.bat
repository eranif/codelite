@echo off

xcopy config\*.default "C:\Program Files\CodeLite\config\" /E /I /H /Y /EXCLUDE:excludes
xcopy config\build_settings.xml.default.win "C:\Program Files\CodeLite\config\build_settings.xml.default" /E /I /H /Y /EXCLUDE:excludes
xcopy debuggers\*.dll "C:\Program Files\CodeLite\debuggers\" /E /I /H /Y /EXCLUDE:excludes
xcopy images\* "C:\Program Files\CodeLite\images\" /E /I /H /Y /EXCLUDE:excludes
xcopy lexers\*.xml "C:\Program Files\CodeLite\lexers\" /E /I /H /Y /EXCLUDE:excludes
xcopy plugins\*.dll "C:\Program Files\CodeLite\plugins\" /E /I /H /Y /EXCLUDE:excludes
xcopy plugins\resources\*.* "C:\Program Files\CodeLite\plugins\resources\" /E /I /H /Y /EXCLUDE:excludes
xcopy rc\*.xrc "C:\Program Files\CodeLite\rc\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\ "C:\Program Files\CodeLite\templates\" /E /I /H /Y /EXCLUDE:excludes
xcopy templates\formbuilder\* "C:\Program Files\CodeLite\templates\formbuilder\" /E /I /H /Y /EXCLUDE:excludes
xcopy *.html "C:\Program Files\CodeLite\" /H /Y /EXCLUDE:excludes
:: Copy the misc files
copy ctags.dll "C:\Program Files\CodeLite\" /Y
copy makedir.exe "C:\Program Files\CodeLite\" /Y
copy le_exec.exe "C:\Program Files\CodeLite\" /Y
copy CodeLite.exe "C:\Program Files\CodeLite\" /Y
copy patch.exe "C:\Program Files\CodeLite\" /Y
copy astyle.sample "C:\Program Files\CodeLite\" /Y
copy ..\sdk\wxconfig\wx-config.exe "C:\Program Files\CodeLite\" /Y
copy ..\sdk\curl\lib\libcurl-4.dll "C:\Program Files\CodeLite\" /Y
copy ..\sdk\curl\lib\libcurl-4.dll . /Y
