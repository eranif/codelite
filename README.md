
What is [CodeLite][2]?
====

[![Join the chat at https://gitter.im/eranif/codelite](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/eranif/codelite?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

[CodeLite][3] is an open source, free, cross platform IDE for the C/C++ programming languages which runs on all major Platforms ( OSX, Windows and Linux )

You can download pre-built binaries for Windows / OSX and Linux from our main **[Download Page][4]**

More information can be found here:

 - [Official Website][5]
 - [Wiki][6]
 - [Download Page][11]
 
----------


![Linux logo][7] Building CodeLite on Linux
----

To build CodeLite on your computer you will need these packages:

 - wxWidgets 3.0 or later
 - The gtk development package: often called libgtk2.0-dev or wxGTK-devel or similar
 - pkg-config (which usually comes with the gtk dev package)
 - The build-essential package (or the relevant bit of it: g++, make etc)
 - git
 - cmake

You should have wxWidgets 3.0 or later built on your machine. If you don't know how to build (or you're just lazy ...) you can [download it install wxWidgets from CodeLite's repository][8]


On Ubuntu / Debian you can install all of the above (except for wxWidgets 3.0) by typing:

    sudo apt-get install libgtk2.0-dev pkg-config build-essential git cmake libssh-dev libwxbase3.0-dev libsqlite3-dev libwxsqlite3-3.0-dev

Git clone the sources:

    git clone https://github.com/eranif/codelite.git

Run cmake and build codelite:

    cd codelite
    mkdir build-release
    cd build-release
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
    make -j4
    sudo make install

----------

![Apple logo][12] Building CodeLite on OS X
----
Prerequisites:

 - wxWidgets 3.x
 - CMake
 - HomeBrew
 - git
 - XCode
 - XCode command-line tools
 
Preparation:
 - (Optional) Make a separate folder for building if you want to get rid of all except the .app file after building
 - Install XCode from Mac App Store
 - Install XCode command-line tools `xcode-select --install`
 - Install HomeBrew :
```
 ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
 - Update HomeBrew : `brew update`
 - (Optional) Upgrade HomeBrew packages : `brew upgrade`
 - Install git : `brew install git`
 - Install CMake : `brew install cmake`
 - Install wxWidgets : `brew install wxmac --dev --use-llvm`
 

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
  make -j4
  make install
  ```
 
You should now have an app bundle `/Users/YOU/codelite/build-release/codelite.app`
 
To launch CodeLite:

`open /Users/YOU/codelite/build-release/codelite.app`

![Windows Logo][9] Building CodeLite on Windows
----
Git clone the sources

    git clone https://github.com/eranif/codelite.git


 - Download codelite installer for Windows from our [Download Page][10]
 - Download wxWidgets 3.0 installer. Also from our [Download Page][11]
 - Open the workspace **LiteEditor.workspace** (located in the codelite folder)
 - Make sure that the project **CodeLiteIDE** is selected (the active project uses **bold** font)
 - Select the **Win_x64_Release** or  **Win_x86_Release** (depending if you want to build a 32 or 64 bit version of CodeLite) and hit **F7**
 - When the compilation is over, close the workspace
 - Next, locate the workspace **codelite_utils/codelite_utils.workspce** and open it
 - Select the **Win_x64_Release** or  **Win_x86_Release** (depending if you want to build a 32 or 64 bit version of CodeLite) hit **F7** and wait for the compilation to end
 - Close CodeLite
 - To update your installation with the new codelite, close codelite and from a CMD.EXE window navigate to codelite-sources/Runtime/ and run the file **update.bat** OR **update64.bat** (again, depending on your selected arch)


  [1]: http://codelite.org/images/logo.png
  [2]: http://codelite.org
  [3]: http://codelite.org
  [4]: http://codelite.org/support.php
  [5]: http://codelite.org
  [6]: http://wiki.codelite.org
  [7]: http://wxcrafter.codelite.org/img/linux.png
  [8]: http://www.codelite.org/LiteEditor/WxWidgets30Binaries
  [9]: http://wxcrafter.codelite.org/img/windows.png
  [10]: http://codelite.org/support.php
  [11]: http://codelite.org/support.php
  [12]: https://lh6.googleusercontent.com/-OMHi1w_3gNk/AAAAAAAAAAI/AAAAAAAAAAA/NrDEagpDUQI/s46-c-k-no/photo.jpg
