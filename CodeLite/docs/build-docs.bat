@echo off
rem To create the HTML help you will need to download DoxyS from http://doxys.dk
doxys DoxySfile

copy /Y default.css htmlhelp\common\

rem And to compile from it the CHM file you'll need to download the HTML Help from 
rem http://download.microsoft.com/download/0/a/9/0a939ef6-e31c-430f-a3df-dfae7960d564/htmlhelp.exe
"%PROGRAMFILES%\HTML Help Workshop\hhc.exe" htmlhelp\CodeLite.hhp
