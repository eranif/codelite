@ECHO OFF
set CURRENT_DUR=%CD%
set PATH=%PATH%;%ProgramFiles(x86)%\7-Zip;C:\Program Files\7-Zip
rmdir /S /Q ..\tmp-1
svn export . ..\tmp-1 --force
cd ..\tmp-1
7z a -tzip codelite-icons-fresh-farm.zip *
copy codelite-icons-fresh-farm.zip ..\Runtime
cd %CURRENT_DUR%
rmdir /S /Q ..\tmp-1
