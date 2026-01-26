# Building wxWidgets from Source

---

## Windows

CodeLite uses **MSYS2** to provide the compiler and related tools.

1. Open an MSYS2 shell and install all prerequisites as described in the linked guide.
2. Clone the wxWidgets repository and initialize its sub‑modules:

```bash
git clone https://github.com/wxWidgets/wxWidgets
cd wxWidgets
git submodule update --init
```

### Release build

```bash
mkdir .build-release
cd .build-release
cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release \
         -DwxBUILD_DEBUG_LEVEL=0 \
         -DwxBUILD_MONOLITHIC=1 -DwxBUILD_SAMPLES=ALL -DwxUSE_STL=1 \
         -DCMAKE_INSTALL_PREFIX=$HOME/root
mingw32-make -j$(nproc) install
```

### Debug build

```bash
mkdir .build-debug
cd .build-debug
cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DwxBUILD_DEBUG_LEVEL=1 \
         -DwxBUILD_SAMPLES=ALL -DwxBUILD_MONOLITHIC=1 -DwxUSE_STL=1 \
         -DCMAKE_INSTALL_PREFIX=$HOME/root
mingw32-make -j$(nproc) install
```

---

## Linux

To build wxWidgets on Linux you need the following packages:

- GTK development libraries
  - `libgtk2.0-dev` for GTK 2
  - `libgtk-3-dev` for GTK 3
- `pkg-config` (usually included with the GTK development package)
- Build essentials (`g++`, `make`, etc.)
- `git`
- `cmake`

Install the prerequisites on **Ubuntu** with:

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
                     xterm
```

### Build sequence

```bash
mkdir -p $HOME/devl
cd $HOME/devl

git clone https://github.com/wxWidgets/wxWidgets
cd wxWidgets
git submodule update --init

mkdir -p .build-release
cd .build-release

../configure --disable-debug_flag --with-gtk=3 --enable-stl
make -j$(nproc) && sudo make install
```

---

## macOS

### Prerequisites

1. Install **Homebrew** – <https://brew.sh/>
2. Run `brew install cmake git`
3. Download the latest Xcode from the App Store
4. In Xcode, open **Preferences → Downloads** and install the Command Line Tools (adds `clang`/`clang++` to `/usr/bin`)

### Building with `configure`

```bash
mkdir -p $HOME/devl
cd $HOME/devl
git clone https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets
git submodule update --init

mkdir .build-release
cd .build-release
../configure --enable-shared        \
             --enable-monolithic    \
             --with-osx_cocoa       \
             --disable-debug        \
             --disable-mediactrl    \
             --enable-stl           \
             --with-libtiff=no      \
             --enable-utf8
make -j$(sysctl -n hw.physicalcpu)
sudo make install
```

### Building with CMake

```bash
mkdir -p $HOME/devl
cd $HOME/devl
git clone https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets
git submodule update --init

mkdir .build-release
cd .build-release
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DwxBUILD_DEBUG_LEVEL=0    \
         -DwxBUILD_MONOLITHIC=1     \
         -DwxBUILD_SAMPLES=ALL      \
         -DwxUSE_SYS_LIBS=OFF       \
         -DwxUSE_LUNASVG=OFF
make -j$(sysctl -n hw.physicalcpu)
sudo make install
```

[1]: https://brew.sh/
[2]: https://wxwidgets.org/downloads/
[3]: https://www.wxwidgets.org/downloads
[4]: /build/mingw_builds/#prepare-a-working-environment