# Building CodeLite from sources

## Windows
----

!!! Important
    We only support `MSYS2` terminal

- [Prepare your working environment][10]
- [Build wxWidgets from sources][5]
- Open `MSYS` terminal, and type:

```bash
pacman -S mingw-w64-clang-x86_64-zlib     \
          mingw-w64-clang-x86_64-libssh   \
          mingw-w64-clang-x86_64-hunspell \
          mingw-w64-clang-x86_64-openssl  \
          mingw-w64-clang-x86_64-sqlite3  \
          flex bison
```

- Download and build `wx-config.exe` from sources:

```bash
git clone https://github.com/eranif/wx-config-msys2.git
cd wx-config-msys2
mkdir build-release
cd $_
cmake .. -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX="$HOME/root"
mingw32-make -j$(nproc) install
```

- Add `$HOME/root/bin` to your path and make sure that `which wx-config` points to the newly installed `wx-config`
- Build CodeLite (in `Release` mode):
- Set the environment variable `MSYS2_BASE` to point to `MSYS2` root installation folder (e.g. `C:/msys64`)

```bash
git clone https://github.com/eranif/codelite.git
cd codelite
git submodule update --init --recursive
mkdir build-release
cd $_
cmake .. -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles" -DWXWIN="$HOME/root" -Wno-dev
mingw32-make -j$(nproc) install
```

!!! Note
    To build CodeLite in debug mode, replace `CMake` argument from:
    `-DCMAKE_BUILD_TYPE=Release` into `-DCMAKE_BUILD_TYPE=Debug`

- To run the new CodeLite:

```bash
cd build-release/install
./codelite
```

## Linux
----
- To build CodeLite on your computer you will first need to install wXWidget 3.1+ (sudo apt install libwxgtk3.2-dev libgtk-3-dev) or [Build wxWidgets from sources][6]
- Install `libssh-dev` and `libsqlite3-dev` packages (or similar, depending on your distro). On **Ubuntu / Debian**, you can use this command:

```bash
    sudo apt install build-essential            \
                     git cmake flex             \
                     libssh-dev libsqlite3-dev  \
                     libpcre2-dev bison flex
```

- Git clone the sources:

```bash
    git clone https://github.com/eranif/codelite.git
    cd codelite
    git submodule update --init --recursive
```

- Build CodeLite:

```bash
 cd codelite
 mkdir build-release
 cd build-release
 cmake -DCMAKE_BUILD_TYPE=Release .. -DCOPY_WX_LIBS=1
 make -j$(nproc)
 sudo make install
```

- To uninstall CodeLite:

```bash
cd build-release # cd to the build directory
sudo xargs rm -vf < install_manifest.txt
```

----------

## macOS
----

Prerequisites:

 - wxWidgets 3.1 or later
 - CMake
 - Git
 - Xcode
 - Xcode command-line tools
 - Homebrew
 - Flex
 - autoconf

Preparation:

 - (Optional) Make a separate folder for building if you want to get rid of all except the `.app` file after building
 - Install Xcode from Mac App Store
 - Install Xcode command-line tools: `xcode-select --install` (or you can type `clang` from the command line and if it is missing you will be prompted to install it)
 - Install Homebrew:

```
 ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

 - Install the required libraries:

```bash
brew update
brew install git        \
             cmake      \
             flex       \
             autoconf   \
             automake   \
             libtool    \
             gettext
```

 - Usually, `brew` will install everything under `/opt/homebrew`, so run this from the terminal:

```bash
echo 'export PATH=/opt/homebrew/bin:$PATH' >> $HOME/.$(basename $SHELL)rc
source $HOME/.$(basename $SHELL)rc
```

 - Next step is to [Build wxWidgets from sources][9]
 - Finally, Build CodeLite:

```bash
    mkdir -p $HOME/devl
    cd $_
    git clone https://github.com/eranif/codelite.git
    cd codelite
    git submodule update --init --recursive
    mkdir build-release
    cd $_
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(sysctl -n hw.physicalcpu) install
```

You should now have an app bundle `$HOME/devl/codelite/build-release/codelite.app`

To launch CodeLite:

- `open $HOME/devl/codelite/build-release/codelite.app`

----------

 [1]: https://codelite.org
 [2]: https://codelite.org/support.php
 [3]: https://codelite.org
 [4]: https://codelite.org/support.php
 [5]: /build/build_wx_widgets/#windows
 [6]: /build/build_wx_widgets/#linux
 [8]: https://codelite.org/support.php
 [9]: /build/build_wx_widgets/#macos
 [10]: /getting_started/windows/
