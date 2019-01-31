@ECHO OFF

set CURRENT_DUR=%CD%
set PATH=%PATH%;D:\software\7-Zip
rmdir /S /Q ..\tmp
7z a -tzip ../wxgui.zip *.png *.wxcp *.cpp *.h LICENSE
echo copy ..\wxgui.zip ..\..\Runtime
echo copy ..\wxgui.zip ..\Runtime
copy ..\wxgui.zip ..\..\Runtime
cd %CURRENT_DUR%
GOTO END

:END
