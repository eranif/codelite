# Building wxWidgets from Source

---

## Windows

CodeLite relies on **MSYS2** to provide the compiler and associated tooling.

- Open an MSYS2 shell and make sure all prerequisites are installed, as detailed in the linked guide.  
- Clone the wxWidgets repository and initialise sub‑modules:

```bash
git clone https://github.com/wxWidgets/wxWidgets
cd wxWidgets
git submodule update --init
```

**Release build**

```bash
mkdir .build-release
cd $_
cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release                 \
         -DwxBUILD_DEBUG_LEVEL=0                                        \
         -DwxBUILD_MONOLITHIC=1 -DwxBUILD_SAMPLES=SOME -DwxUSE_STL=1    \
         -DCMAKE_INSTALL_PREFIX=$HOME/root
mingw32-make -j$(nproc) install
```

**Debug build**

```bash
mkdir .build-debug
cd .build-debug
cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DwxBUILD_DEBUG_LEVEL=1 \
    -DwxBUILD_SAMPLES=SOME  -DwxBUILD_MONOLITHIC=1  -DwxUSE_STL=1             \
    -DCMAKE_INSTALL_PREFIX=$HOME/root  
mingw32-make -j$(nproc) install
```

---

## Linux

To compile wxWidgets on a Linux machine you’ll need the following packages:

- GTK development libraries (`libgtk2.0-dev` for GTK 2, `libgtk-3-dev` for GTK 3)  
- `pkg-config` (usually bundled with the GTK dev package)  
- Build essentials (`g++`, `make`, etc.)  
- `git`  
- `cmake`

Install the prerequisites on **Ubuntu** with:

```bash
sudo apt-get install libgtk-3-dev       \
                     pkg-config         \
                     build-essential    \
                     git                \
                     cmake              \
                     libsqlite3-dev     \
                     libssh-dev         \
                     libedit-dev        \
                     libhunspell-dev    \
                     xterm
```

Build sequence:

```bash
mkdir -p $HOME/devl
cd $HOME/devl

git clone https://github.com/wxWidgets/wxWidgets
cd wxWidgets
git submodule update --init

mkdir -p .build-release
cd $_

../configure --disable-debug_flag --with-gtk=3 --enable-stl
make -j$(nproc) && sudo make install
```

---

## macOS

### Prerequisites

1. Install **Homebrew** – <https://brew.sh/>  
2. `brew install cmake git`  
3. Download the latest Xcode from the App Store  
4. Open Xcode → **Preferences** → **Downloads** → install the Command Line Tools (this places `clang`/`clang++` in `/usr/bin`)  

### Building with CMake

```bash
mkdir -p $HOME/devl
cd $_
git clone https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets
git submodule update --init

mkdir .build-release
cd $_
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DwxBUILD_DEBUG_LEVEL=0 \
         -DwxBUILD_MONOLITHIC=1 \
         -DwxBUILD_SAMPLES=ALL
make -j$(sysctl -n hw.physicalcpu)
sudo make install
```

[1]: https://brew.sh/  
[2]: https://wxwidgets.org/downloads/  
[3]: https://www.wxwidgets.org/downloads  
[4]: /build/mingw_builds/#prepare-a-working-environment
