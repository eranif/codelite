# Building CodeLite from sources

## Windows
----

Git clone the sources:

    git clone https://github.com/eranif/codelite.git

 - Download CodeLite installer for Windows from our [Download Page][8]
 - Build wxWidgets 3.1 or later. See build instructions [here][7]
 - Open the workspace `CodeLiteIDE.workspace` (located in the CodeLite's folder)
 - Make sure that the project `CodeLiteIDE` is selected (the active project uses **bold** font)
 - Select the `Win_x64_Release` or `Win_x86_Release` (depending if you want to build a 32 or 64 bit version of CodeLite) 
   and hit ++f7++
 - When the compilation is over, close the workspace
 - Close CodeLite
 - To update your installation with the new CodeLite, close CodeLite and from a cmd.exe window navigate to `codelite-sources/Runtime/` and run the file `update64.bat` OR `update.bat` (again, depending on your selected arch)


## Linux
----

To build CodeLite on your computer you will need these packages:

 - wxWidgets 3.1 or later development packages (or a self-build).
 - The GTK development packages: often called `libgtk-3-dev` or `gtk3-devel`, or similar.
 - `pkg-config` (which usually comes with the GTK dev package).
 - The `build-essential` package (or the relevant bit of it: G++, Make etc).
 - `cmake`.
 - `git`.

In the Debian/Ubuntu repositories, wxWidgets libraries are at their 3.0 version, which is good enough for compiling Codelite 15.0, however, **you should have wxWidgets 3.1 or later to built the master branch on your machine**. If you don't know how to build wxWidgets (or you're just lazy...) you can [add the CodeLite's wxWidgets repository to you distro][6].


**Ubuntu / Debian**: After adding the proper libraries to you distro's sources, you can install all of the above by typing:

```bash
    sudo apt install    pkg-config build-essential git cmake \
                        libgtk-3-dev libssh-dev libsqlite3-dev \
                        libwxbase3.1-dev libwxbase3.1unofficial3-dev \
                        libwxgtk3.1unofficial3-dev
```

Git clone the sources:

```bash
    git clone https://github.com/eranif/codelite.git
```

Run CMake and build CodeLite:

```bash
  cd codelite
  mkdir build-release
  cd build-release
  cmake .. -DCMAKE_BUILD_TYPE=Release
  cmake --build . -j $(nproc)
  sudo cmake --build . --target install
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


Clone the repo (lets assume that you have a folder `/Users/YOU/src`)

```bash
cd /Users/YOU/src
git clone https://github.com/eranif/codelite.git
```
 the above will create the folder `/Users/YOU/codelite`

 To build CodeLite:

```bash
cd /Users/YOU/codelite
mkdir build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j $(sysctl -n hw.physicalcpu)
cmake --build . --target install
```

You should now have an app bundle `/Users/YOU/codelite/build-release/codelite.app`

To launch CodeLite:

`open /Users/YOU/codelite/build-release/codelite.app`

----------


[1]: https://codelite.org
[2]: https://codelite.org/support.php
[3]: https://codelite.org
[4]: https://codelite.org/support.php
[5]: https://wiki.codelite.org/pmwiki.php
[6]: https://wiki.codelite.org/pmwiki.php/Main/WxWidgets31Binaries
[7]: https://wiki.codelite.org/pmwiki.php/Developers/BuildingWxWidgetsWin
[8]: https://codelite.org/support.php