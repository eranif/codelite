@echo off
if not exist wxWidgets-2.9.2 mkdir wxWidgets-2.9.2
xcopy C:\wxWidgets-2.9.2\lib\gcc_dll\* "wxWidgets-2.9.2\lib\gcc_dll\" /E /I /H /Y
xcopy C:\wxWidgets-2.9.2\include\* "wxWidgets-2.9.2\include\" /E /I /H /Y
xcopy C:\wxWidgets-2.9.2\build\msw\config.gcc "wxWidgets-2.9.2\build\msw\config.gcc" /Y
