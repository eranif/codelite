@echo off
if not exist wxWidgets-3.0.2 mkdir wxWidgets-3.0.2
xcopy D:\src\wxWidgets-3.0.2\lib\gcc_dll\* "wxWidgets-3.0.2\lib\gcc_dll\" /E /I /H /Y /EXCLUDE:wx_excludes
xcopy D:\src\wxWidgets-3.0.2\include\* "wxWidgets-3.0.2\include\" /E /I /H /Y /EXCLUDE:wx_excludes
xcopy D:\src\wxWidgets-3.0.2\build\msw\config.gcc "wxWidgets-3.0.2\build\msw\config.gcc" /Y /EXCLUDE:wx_excludes
