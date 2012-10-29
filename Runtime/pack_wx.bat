@echo off
if not exist wxWidgets-2.9.4 mkdir wxWidgets-2.9.4
xcopy D:\src\wx294\lib\gcc_dll\* "wxWidgets-2.9.4\lib\gcc_dll\" /E /I /H /Y /EXCLUDE:wx_excludes
xcopy D:\src\wx294\include\* "wxWidgets-2.9.4\include\" /E /I /H /Y /EXCLUDE:wx_excludes
xcopy D:\src\wx294\build\msw\config.gcc "wxWidgets-2.9.4\build\msw\config.gcc" /Y /EXCLUDE:wx_excludes
