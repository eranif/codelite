# Build wxWidgets from sources
---

## Windows
---

CodeLite uses `MSYS2` for installing compiler and other tools:

- Prepare a working terminal with all the tools required [as described here][4]
- Open `MSYS2` terminal, and clone wxWidgets sources and build them:

```bash
git clone https://github.com/wxWidgets/wxWidgets
cd ~/wxWidgets
git submodule update --init
cd build/mswu
# create a setup.h file
mingw32-make -f makefile.gcc setup_h SHARED=1 UNICODE=1 BUILD=release VENDOR=cl
```

CodeLite requires a wxWidgets build which enables Graphic Context, Direct2D and `wxUSE_SOCKET2` all enabled

To do so: 

- Open the file `~/wxWidgets/lib/gcc_dll/setup.h` in your editor
- Make sure that `wxUSE_GRAPHICS_CONTEXT`, `wxUSE_GRAPHICS_DIRECT2D` and `wxUSE_SOCKET2` are all set to `1` (if not, change it)

Start the build process (from within the `MSYS2` terminal):

```bash
cd ~/wxWidgets/build/mswu
mingw32-make -j$(nproc) -f Makefile.gcc SHARED=1 UNICODE=1 BUILD=release VENDOR=cl CXXFLAGS="-fno-keep-inline-dllexport -std=c++11"
```

!!! Tip
    if you need a `Debug` build of wxWidgets, follow the above steps and replace any occurrence of `BUILD=release` &#8594; `BUILD=debug`

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

!!! Tip
    If your distro provides a newer version of `clang-format`, install it. The higher, the better

### GTK3 (**recommended**)

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

### GTK2 (*Obsolete*)

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
 [3]: https://www.wxwidgets.org/downloads
 [4]: /build/mingw_builds/#prepare-a-working-environment