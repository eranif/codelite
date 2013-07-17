@echo off
if not exist wxWidgets-2.9.5 mkdir wxWidgets-2.9.5
xcopy D:\src\wx295\lib\gcc_dll\* "wxWidgets-2.9.5\lib\gcc_dll\" /E /I /H /Y /EXCLUDE:wx_excludes
xcopy D:\src\wx295\include\* "wxWidgets-2.9.5\include\" /E /I /H /Y /EXCLUDE:wx_excludes
xcopy D:\src\wx295\build\msw\config.gcc "wxWidgets-2.9.5\build\msw\config.gcc" /Y /EXCLUDE:wx_excludes
