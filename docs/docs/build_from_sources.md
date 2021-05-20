# Building CodeLite from sources

## Windows
----

Git clone the sources:

    git clone https://github.com/eranif/codelite.git

 - Download CodeLite installer for Windows from our [Download Page][8]
 - Build wxWidgets 3.1 or later. See build instructions [here][5]
 - Open the workspace `CodeLiteIDE.workspace` (located in the CodeLite's folder)
 - Make sure that the project `CodeLiteIDE` is selected (the active project uses **bold** font)
 - Select the `Win_x64_Release` or `Win_x86_Release` (depending if you want to build a 32 or 64 bit version of CodeLite) 
   and hit ++f7++
 - When the compilation is over, close the workspace
 - Close CodeLite
 - To update your installation with the new CodeLite, close CodeLite and from a cmd.exe window navigate to `codelite-sources/Runtime/` and run the file `update64.bat` OR `update.bat` (again, depending on your selected arch)


## Linux
----

- To build CodeLite on your computer you will first need to [Build wxWidgets from sources][6]
- Install `libssh-dev` and `libsqlite3-dev` packages (or similar, depending on your distro). On **Ubuntu / Debian**, you can use this command:

```bash
    sudo apt install build-essential git cmake \
                     libssh-dev libsqlite3-dev 
```

- Git clone the sources:

```bash
    git clone https://github.com/eranif/codelite.git
```

- Run `cmake` and build CodeLite:

```bash
 cd codelite
 mkdir build-release
 cd build-release
 cmake -DCMAKE_BUILD_TYPE=Release .. -DCOPY_WX_LIBS=1
 make -j$(nproc)
 sudo make install
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
 - Homebrew.

Preparation:

 - (Optional) Make a separate folder for building if you want to get rid of all except the .app file after building
 - Install Xcode from Mac App Store
 - Install Xcode command-line tools: `xcode-select --install`
 - Install Homebrew:

```
 ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

- Update Homebrew: `brew update`
 - (Optional) Upgrade Homebrew packages: `brew upgrade`
 - Install Git: `brew install git`
 - Install CMake: `brew install cmake`
 - Install wxWidgets: `brew install wxmac --dev --use-llvm`

!!! Tip
    You can choose to build [wxWidgets from sources](/build_wx_widgets/#macos)


Clone the repo (lets assume that you have a folder `/Users/$USER/src`)

```bash
cd /Users/$USER/src
git clone https://github.com/eranif/codelite.git
```
 the above will create the folder `/Users/$USER/codelite`

 To build CodeLite:

```bash
cd /Users/$USER/codelite
mkdir build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j $(sysctl -n hw.physicalcpu)
cmake --build . --target install
```

You should now have an app bundle `/Users/$USER/codelite/build-release/codelite.app`

To launch CodeLite:

`open /Users/$USER/codelite/build-release/codelite.app`

----------


 [1]: https://codelite.org
 [2]: https://codelite.org/support.php
 [3]: https://codelite.org
 [4]: https://codelite.org/support.php
 [5]: /build_wx_widgets/#windows
 [6]: /build_wx_widgets/#linux
 [8]: https://codelite.org/support.php
