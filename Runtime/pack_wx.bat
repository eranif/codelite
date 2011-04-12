@echo off
if not exist wxWidgets-2.8.12 mkdir wxWidgets-2.8.12
xcopy C:\Users\eran\software\wxWidgets-2.8.12\lib\gcc_dll\* "wxWidgets-2.8.12\lib\gcc_dll\" /E /I /H /Y
xcopy C:\Users\eran\software\wxWidgets-2.8.12\include\* "wxWidgets-2.8.12\include\" /E /I /H /Y
xcopy C:\Users\eran\software\wxWidgets-2.8.12\build\msw\config.gcc "wxWidgets-2.8.12\build\msw\config.gcc" /Y
