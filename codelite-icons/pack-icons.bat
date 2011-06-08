@ECHO OFF
set CURRENT_DUR=%CD%
set PATH=%PATH%;%ProgramFiles(x86)%\7-Zip;C:\Program Files\7-Zip
rmdir /S /Q ..\tmp
svn export . ..\tmp --force
cd ..\tmp
7z a -tzip codelite-icons.zip *
copy codelite-icons.zip ..\Runtime
cd %CURRENT_DUR%
rmdir /S /Q ..\tmp
