@echo off
if not exist wxWidgets-3.0.1 mkdir wxWidgets-3.0.1
xcopy D:\src\wxWidgets-3.0.1\lib\gcc_dll\* "wxWidgets-3.0.1\lib\gcc_dll\" /E /I /H /Y /EXCLUDE:wx_excludes
xcopy D:\src\wxWidgets-3.0.1\include\* "wxWidgets-3.0.1\include\" /E /I /H /Y /EXCLUDE:wx_excludes
xcopy D:\src\wxWidgets-3.0.1\build\msw\config.gcc "wxWidgets-3.0.1\build\msw\config.gcc" /Y /EXCLUDE:wx_excludes
