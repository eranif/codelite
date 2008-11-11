@echo off
if not exist wxWidgets-2.8.7 mkdir wxWidgets-2.8.7
xcopy C:\wxWidgets-2.8.7\lib\gcc_dll\* "wxWidgets-2.8.7\lib\gcc_dll\" /E /I /H /Y
xcopy C:\wxWidgets-2.8.7\include\* "wxWidgets-2.8.7\include\" /E /I /H /Y
xcopy C:\wxWidgets-2.8.7\build\msw\config.gcc "wxWidgets-2.8.7\build\msw\config.gcc" /Y
