set CurrentDir=%CD%
cd C:\wxWidgets-2.8.10\build\msw
set PATH=C:\MinGW-4.4.0\bin;%PATH%

mingw32-make -j 4 -f makefile.gcc SHARED=1 UNICODE=1 MONOLITHIC=1 BUILD=release clean
mingw32-make -j 4 -f makefile.gcc SHARED=1 UNICODE=1 MONOLITHIC=1 BUILD=release

mingw32-make -j 4 -f makefile.gcc SHARED=1 UNICODE=1 MONOLITHIC=1 BUILD=debug clean
mingw32-make -j 4 -f makefile.gcc SHARED=1 UNICODE=1 MONOLITHIC=1 BUILD=debug

cd %CurrentDir%
