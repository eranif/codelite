# Build wxWidgets from sources
---

## Windows
---

### Prerequisites
- Download and install MinGW. CodeLite is built with MinGW w64 7.1 64 bit
- Open `CMD` terminal and add the `bin` folder to the `PATH` environment, e.g: `C:\MinGW-64\bin` (assuming you installed MinGW under `C:\MinGW-64\bin`) `set PATH=C:\MinGW-64\bin;%PATH%`
- Download wxWidgets sources, CodeLite requires wxWidgets `3.1.x` and later [from here](http://www.wxwidgets.org/downloads)
- If you have cloned wxWidgets repository from git master, you will also need to run this: `git submodule update --init` from the root wxWidgets folder 

### Release

- CodeLite requires a wxWidgets build which enables Graphic Context, to do this, open a `CMD` terminal and type:

```bash
 cd \Path\To\wxWidgets\Sources\build\msw
 mingw32-make -f makefile.gcc setup_h SHARED=1 UNICODE=1 BUILD=release VENDOR=cl
```

- Next, open the file: `\path\to\wxwidgets\lib\gcc_dll\mswu\wx\setup.h` and ensure that `wxUSE_GRAPHICS_CONTEXT` and `wxUSE_GRAPHICS_DIRECT2D` are both set to `1`: 

```c++
#   define wxUSE_GRAPHICS_CONTEXT 1
...
#   define wxUSE_GRAPHICS_DIRECT2D 1
```

- Start the build:

```bash
 mingw32-make -j8 -f Makefile.gcc SHARED=1 UNICODE=1 BUILD=release VENDOR=cl CXXFLAGS="-fno-keep-inline-dllexport -std=c++11"
```

### Debug

To build in `Debug` confiugration, follow the `Release` steps and replace any occurrence of `BUILD=release` &#8594; `BUILD=debug`

## Linux
---

## macOS
---

