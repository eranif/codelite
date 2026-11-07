@ECHO OFF

set CURRENT_DUR=%CD%
set PATH=%PATH%;D:\software\7-Zip
rmdir /S /Q ..\tmp
7z a -tzip ../wxgui.zip *.wxcp *.cpp *.h LICENSE
REM The SVG icons of the dark and the light themes (bitmaps\dark and bitmaps\light)
pushd ..\assets
7z a -tzip ..\wxgui.zip bitmaps\dark\*.svg bitmaps\light\*.svg
popd
echo copy ..\wxgui.zip ..\..\Runtime
echo copy ..\wxgui.zip ..\Runtime
copy ..\wxgui.zip ..\..\Runtime
cd %CURRENT_DUR%
GOTO END

:END
