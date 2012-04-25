set CurrentDir=%CD%
cd C:\wx-trunk\build\msw
set PATH=C:\MinGW-4.6.1\bin;%PATH%

mingw32-make -j 4 -f makefile.gcc SHARED=1 UNICODE=1 MONOLITHIC=1 BUILD=release clean
mingw32-make -j 4 -f makefile.gcc SHARED=1 UNICODE=1 MONOLITHIC=1 BUILD=release

mingw32-make -j 4 -f makefile.gcc SHARED=1 UNICODE=1 MONOLITHIC=1 BUILD=debug clean
mingw32-make -j 4 -f makefile.gcc SHARED=1 UNICODE=1 MONOLITHIC=1 BUILD=debug

cd %CurrentDir%
