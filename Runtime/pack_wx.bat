@echo off
if not exist wxWidgets-2.8.10 mkdir wxWidgets-2.8.10
xcopy C:\Users\eran\software\wxWidgets-2.8.10\lib\gcc_dll\* "wxWidgets-2.8.10\lib\gcc_dll\" /E /I /H /Y
xcopy C:\Users\eran\software\wxWidgets-2.8.10\include\* "wxWidgets-2.8.10\include\" /E /I /H /Y
xcopy C:\Users\eran\software\wxWidgets-2.8.10\build\msw\config.gcc "wxWidgets-2.8.10\build\msw\config.gcc" /Y
