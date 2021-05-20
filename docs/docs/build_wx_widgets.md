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

To build wxWidgets on you computer you will need these packages:

- The gtk development package: for GTK+2 it's often called `libgtk2.0-dev` or similar; for GTK3, `libgtk-3-dev`
- `pkg-config` (which usually comes with the gtk dev package) 
- The `build-essential` package (or the relevant bit of it: `g++`, `make` etc)
- `git`
- `cmake`

Use the following command to install the prerequisites for Ubuntu 18.04:

```bash
sudo apt-get install libgtk-3-dev \
                     pkg-config \
                     build-essential \
                     git \
                     cmake \
                     libsqlite3-dev \
                     libssh-dev \
                     libedit-dev \
                     libhunspell-dev \
                     clang-format-8 \
                     xterm
```

### GTK2

```bash
mkdir /home/$USER/devl
cd /home/$USER/devl
git clone https://github.com/wxWidgets/wxWidgets.git
cd /home/$USER/devl/wxWidgets
git submodule init
git submodule update
mkdir build-release
cd build-release
../configure --disable-debug_flag
make -j$(nproc) && sudo make install
```

### GTK3 (recommended)

```bash
mkdir /home/$USER/devl
cd /home/$USER/devl
git clone https://github.com/wxWidgets/wxWidgets.git
cd /home/$USER/devl/wxWidgets
git submodule init
git submodule update
mkdir build-release-gtk3
cd build-release-gtk3
../configure --disable-debug_flag --with-gtk=3
make -j$(nproc) && sudo make install
```

## macOS
---

#### Prerequisites

- Install [Homebrew][1]
- Install `cmake`
- Install `git`
- Install latest Xcode from Apple
- Install the Command Line Tools (open `Xcode` &#8594;  `Preferences` &#8594;  `Downloads and install the command line tools`). This will place `clang`/`clang++` in the default locations `/usr/bin`
- [Download wxWidgets sources][2]

#### Build wxWidgets

```bash
cd /Users/$USER/src/wx-src/
mkdir build-release
cd build-release
../configure --enable-shared --enable-monolithic --with-osx_cocoa CXX='clang++ -std=c++11 -stdlib=libc++' CC=clang --disable-debug --disable-mediactrl
make -j8
sudo make install
```

 [1]: https://brew.sh/
 [2]: https://wxwidgets.org/downloads/
 