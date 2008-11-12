@echo off
if not exist wxWidgets-2.8.9 mkdir wxWidgets-2.8.9
xcopy C:\wxWidgets-2.8.9\lib\gcc_dll\* "wxWidgets-2.8.9\lib\gcc_dll\" /E /I /H /Y
xcopy C:\wxWidgets-2.8.9\include\* "wxWidgets-2.8.9\include\" /E /I /H /Y
xcopy C:\wxWidgets-2.8.9\build\msw\config.gcc "wxWidgets-2.8.9\build\msw\config.gcc" /Y
