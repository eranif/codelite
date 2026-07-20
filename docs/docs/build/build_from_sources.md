# Building CodeLite from sources

## Windows
----

!!! Important
    We only support `MSYS2` terminal

- Open `MSYS2` terminal, and type:

```bash
git clone https://github.com/eranif/codelite.git
cd codelite
git submodule update --init --recursive

# Use the build script to build CodeLite along with its dependencies
./build.sh
```

- To run the new CodeLite:

```bash
(cd .build-release/install/bin && ./codelite.exe)
```

## Linux
----
- To build CodeLite on your computer you will first need to install wXWidget 3.1+ (sudo apt install libwxgtk3.2-dev libgtk-3-dev) or [Build wxWidgets from sources][6]
- Install `libssh-dev` and `libsqlite3-dev` packages (or similar, depending on your distro). On **Ubuntu / Debian**, you can use this command:

```bash
sudo apt install build-essential            \
                 git cmake                  \
                 libssh-dev libsqlite3-dev  \
                 libmariadb-dev             \
                 libpq-dev                  \
                 libpcre2-dev bison flex    \
                 universal-ctags            \
                 fonts-noto
sudo fc-cache -fv
```

!!! Note
    `fonts-noto` is required for proper display of Unicode symbols in the editor.

- Git clone the sources and build CodeLite:

```bash
git clone https://github.com/eranif/codelite.git
cd codelite
git submodule update --init --recursive
./build.sh
```

- To uninstall CodeLite:

```bash
cd .build-release # cd to the build directory created by build.sh script
sudo xargs rm -vf < install_manifest.txt
```

----------

## macOS
----

Prerequisites:

 - CMake
 - Git
 - clang++ / clang
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
brew install git            \
             cmake          \
             universal-ctags\
             flex           \
             autoconf       \
             automake       \
             libtool        \
             mariadb        \
             postgresql     \
             gettext
```

 - Usually, `brew` will install everything under `/opt/homebrew`, so run this from the terminal:

```bash
echo 'export PATH=/opt/homebrew/bin:$PATH' >> $HOME/.$(basename $SHELL)rc
source $HOME/.$(basename $SHELL)rc
```

 - Finally, Build CodeLite:

```bash
mkdir -p $HOME/devl
cd $_
git clone https://github.com/eranif/codelite.git
cd codelite
git submodule update --init --recursive
./build.sh
```

You should now have an app bundle `$HOME/devl/codelite/build-release/codelite.app`

To launch CodeLite:

- `open $HOME/devl/codelite/build-release/codelite.app`

----------

 [1]: https://codelite.org
 [2]: https://codelite.org/support.php
 [3]: https://codelite.org
 [4]: https://codelite.org/support.php
 [6]: /build/build_wx_widgets/#linux
 [8]: https://codelite.org/support.php
 [10]: /getting_started/windows/
