@echo off
::Cleanup if there is an old update.
if exist update rmdir /S /Q update

::Make the directory tree
if not exist update\debuggers mkdir update\debuggers
if not exist update\plugins mkdir update\plugins
if not exist update\rc mkdir update\rc
if not exist update\templates\gizmos mkdir update\templates\gizmos
if not exist update\templates\projects\dynamic-library mkdir update\templates\projects\dynamic-library
if not exist update\templates\projects\dynamic-library-wx-enabled mkdir update\templates\projects\dynamic-library-wx-enabled
if not exist update\templates\projects\executable mkdir update\templates\projects\executable
if not exist update\templates\projects\premake-project mkdir update\templates\projects\premake-project
if not exist update\templates\projects\static-library mkdir update\templates\projects\static-library
if not exist update\templates\projects\static-library-wx-enabled mkdir update\templates\projects\static-library-wx-enabled
if not exist update\templates\projects\vc-dynamic-library mkdir update\templates\projects\vc-dynamic-library
if not exist update\templates\projects\vc-executable mkdir update\templates\projects\vc-executable
if not exist update\templates\projects\vc-static-lib mkdir update\templates\projects\vc-static-lib
if not exist update\images mkdir update\images
if not exist update\lexers\Default mkdir update\lexers\Default
if not exist update\lexers\BlackTheme mkdir update\lexers\BlackTheme
if not exist update\bin mkdir update\bin
if not exist update\sdk mkdir update\sdk
if not exist update\sdk\lib mkdir update\sdk\lib
if not exist update\sdk\include mkdir update\sdk\include
if not exist update\sdk\include\UnitTest++ mkdir update\sdk\include\UnitTest++
if not exist update\sdk\include\UnitTest++\win32 mkdir update\sdk\include\UnitTest++\win32

:: Copy the files
copy debuggers\*.dll "update\debuggers\"
copy plugins\*.dll "update\plugins\"
copy rc\*.xrc "update\rc\"
copy templates\* "update\templates\"
copy templates\gizmos\* "update\templates\gizmos\"
copy templates\projects\* "update\templates\projects\"
copy templates\projects\dynamic-library\* "update\templates\projects\dynamic-library\"
copy templates\projects\dynamic-library-wx-enabled\* "update\templates\projects\dynamic-library-wx-enabled\"
copy templates\projects\executable\* "update\templates\projects\executable\"
copy templates\projects\premake-project\* "update\templates\projects\premake-project\"
copy templates\projects\static-library\* "update\templates\projects\static-library\"
copy templates\projects\static-library-wx-enabled\* "update\templates\projects\static-library-wx-enabled\"
copy templates\projects\vc-dynamic-library\* "update\templates\projects\vc-dynamic-library\"
copy templates\projects\vc-executable\* "update\templates\projects\vc-executable\"
copy templates\projects\vc-static-lib\* "update\templates\projects\vc-static-lib\"
copy images\* "update\images\"
copy astyle.sample "update\"
copy *.html "update\"
copy config\*.default "update\config\"
copy lexers\Default\*.xml "update\lexers\Default\"
copy lexers\BlackTheme\*.xml "update\lexers\BlackTheme\"
copy CodeLite.exe "update\"
copy bin\*.exe "update\bin\"
copy "sdk\lib\win32\*.a" "update\sdk\lib\"
copy "sdk\include\UnitTest++\*.h" "update\sdk\include\UnitTest++\"
copy "sdk\include\UnitTest++\win32\*.h" "update\sdk\include\UnitTest++\win32\"
copy wxmsw28u_gcc*.dll "update\"
copy ..\sdk\curl\lib\libcurl-4.dll "update\"
copy bin\premakesinglebuild.lua "update\bin\"