# Building CodeLite from sources

## Windows
----

- Install compiler and other required tools (`git` etc) [as described here][10]
- Build wxWidgets from sources. See build instructions [here][5]
- Clone CodeLite sources:

```bash
git clone https://github.com/eranif/codelite.git
cd codelite
git submodule update --init
```

- Download and install CodeLite for Windows (`64 bit`) from our [Download Page][8]
- Open the workspace `CodeLiteIDE.workspace` (located in the CodeLite's folder)
- Make sure that the project `CodeLiteIDE` is selected (the active project uses **bold** font)
- Select the `Win_x64_Release` build configuration and click ++f7++
- When the compilation is over, close the workspace
- Close CodeLite
- To update your installation with the new CodeLite, close CodeLite and from a `cmd.exe` window (opened as Administrator), navigate to `codelite-sources/Runtime/` and run the file `update64.bat`

## Linux
----

- To build CodeLite on your computer you will first need to [Build wxWidgets from sources][6]
- Install `libssh-dev` and `libsqlite3-dev` packages (or similar, depending on your distro). On **Ubuntu / Debian**, you can use this command:

```bash
    sudo apt install build-essential            \
                     git cmake                  \
                     libssh-dev libsqlite3-dev  \
                     libpcre2-dev
```

- Git clone the sources:

```bash
    git clone https://github.com/eranif/codelite.git
    cd codelite
    git submodule update --init
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
brew install git \
             cmake \
             libssh \
             hunspell
```

 - Usually, `brew` will install everything under `/opt/homebrew`, so run this from the terminal:

```bash
echo 'export PATH=/opt/homebrew/bin:$PATH' >> $HOME/.$(basename $SHELL)rc
source $HOME/.$(basename $SHELL)rc
```

 - Next step is to [Build wxWidgets from sources][9]
 - Finally, Build CodeLite:

```bash
    mkdir -p $HOME/src
    cd $HOME/src
    git clone https://github.com/eranif/codelite.git
    cd codelite
    git submodule update --init

    # build CodeLite release configuration
    mkdir build-release
    cd build-release
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . -j $(sysctl -n hw.physicalcpu)
    cmake --build . --target install
```

You should now have an app bundle `$HOME/src/codelite/build-release/codelite.app`

To launch CodeLite:

- `open $HOME/src/codelite/build-release/codelite.app`

----------

 [1]: https://codelite.org
 [2]: https://codelite.org/support.php
 [3]: https://codelite.org
 [4]: https://codelite.org/support.php
 [5]: /build/build_wx_widgets/#windows
 [6]: /build/build_wx_widgets/#linux
 [8]: https://codelite.org/support.php
 [9]: /build/build_wx_widgets/#macos
 [10]: /build/mingw_builds/#prepare-a-working-environment