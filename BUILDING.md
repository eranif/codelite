# Building CodeLite

This file explains how to build CodeLite from sources. All platforms use the same script: `build.sh`.

The script:

- Checks (and on Windows, installs) the build tools.
- Builds wxWidgets inside the build folder. You do not need to install wxWidgets.
- Configures CodeLite with CMake and builds it.

For more details, see the [online documentation](https://docs.codelite.org/build/build_from_sources/).

## Quick start

```bash
git clone https://github.com/eranif/codelite.git
cd codelite
git submodule update --init --recursive
./build.sh
```

The first build is slow, because it also builds wxWidgets. Next builds reuse it.

## Requirements

### Windows

- The `MSYS2` terminal (CLANG64 or CLANGARM64). Other shells are not supported.
- Nothing else. `build.sh` installs the required MSYS2 packages with `pacman`.
- You do **not** need `wx-config` or `wx-config-msys2`. CMake finds wxWidgets by its CMake package config.

### Linux

- A C++ compiler (`g++`), `cmake` and `git`.
- On Ubuntu / Debian, install the dependencies with:

```bash
sudo apt install build-essential            \
                 git cmake                  \
                 libgtk-3-dev               \
                 libssh-dev libsqlite3-dev  \
                 libmariadb-dev             \
                 libpq-dev                  \
                 libpcre2-dev bison flex    \
                 universal-ctags            \
                 fonts-noto
sudo fc-cache -fv
```

`fonts-noto` is needed to show Unicode symbols in the editor.

### macOS

- Xcode and the command line tools (`xcode-select --install`).
- [Homebrew](https://brew.sh), then:

```bash
brew update
brew install git cmake universal-ctags flex autoconf automake libtool mariadb postgresql gettext
echo 'export PATH=/opt/homebrew/bin:$PATH' >> $HOME/.$(basename $SHELL)rc
source $HOME/.$(basename $SHELL)rc
```

## Using build.sh

```
Usage: build.sh [options] [target]
```

| Target              | Description                                                       |
|---------------------|-------------------------------------------------------------------|
| _(none)_            | Build CodeLite in release mode (default)                          |
| `debug`             | Build CodeLite in debug mode                                      |
| `clean`             | Run `make clean` in the build folder                              |
| `distclean`         | Remove the whole build folder                                     |
| `package`           | Create an installer for the current platform                      |
| `wxcrafter`         | Build wxCrafter (Windows only)                                    |
| `package_wxcrafter` | Build wxCrafter and create an installer for it (Windows only)     |

| Option       | Description                                                          |
|--------------|----------------------------------------------------------------------|
| `--cmake`    | Run the CMake configure step again, even if it is up to date         |
| `--tests`    | Build the unit tests                                                 |
| `--no-build` | Run CMake only. Skip the build step                                  |
| `-h`, `--help` | Show the help message                                              |

| Environment variable | Description                                                              |
|----------------------|--------------------------------------------------------------------------|
| `CL_WX_VERSION`      | Windows only. The wxWidgets tag or branch to build, for example `v3.2.11` or `master`. If the existing build is of another version, it is rebuilt. |

Examples:

```bash
./build.sh                 # release build
./build.sh debug           # debug build
./build.sh --tests         # release build with the unit tests
./build.sh --cmake         # force the CMake configure step
./build.sh package         # create an installer
CL_WX_VERSION=v3.2.11 ./build.sh   # Windows: build with wxWidgets 3.2.11
```

You can use only one target at a time. CMake runs again by itself when a `CMakeLists.txt` file is newer than the build.

### Build folders

| Folder                      | Content                                       |
|-----------------------------|-----------------------------------------------|
| `.build-release`            | Release build of CodeLite                     |
| `.build-debug`              | Debug build of CodeLite                       |
| `.build-release-wxcrafter`  | wxCrafter build (Windows)                     |
| `<build folder>/wxWidgets-install` | The wxWidgets that `build.sh` built    |

### wxWidgets versions

`build.sh` clones and builds wxWidgets from GitHub:

- Windows and macOS: `v3.3.3.1`. On Windows, you can change it with `CL_WX_VERSION`.
- Linux: `v3.3.2`. Newer versions crash on copy (Ctrl-C) in WSLg.

## Running CodeLite

| Platform | Command                                                         |
|----------|-----------------------------------------------------------------|
| Windows  | `(cd .build-release/install/bin && ./codelite.exe)`             |
| Linux    | `.build-release/bin/codelite`                                   |
| macOS    | `open .build-release/codelite.app`                              |

For a debug build on Linux, install it first, then run it from the install folder:

```bash
(cd .build-debug && make -j$(nproc) install)
~/root/bin/codelite
```

## Tests

Build with `--tests`, then run `ctest` in the build folder:

```bash
./build.sh --tests
(cd .build-release && ctest --output-on-failure)
```

## Creating an installer

```bash
./build.sh package
```

- Windows: an installer is created in `.build-release/installer`.
- Linux: a `.deb` package is created in the build folder.
- macOS: the app is signed and notarized. The `CODELITE_PASSWORD` environment variable must be set.

## wxCrafter (Windows only)

```bash
./build.sh wxcrafter          # build
./build.sh package_wxcrafter  # build and create an installer
(cd .build-release-wxcrafter/install/bin && ./wxcrafter.exe)
```

wxCrafter uses the same wxWidgets build as CodeLite.

## Shell completion

To get tab completion for the options and targets of `build.sh` (Bash only), add this line to your `~/.bashrc`:

```bash
source /path/to/codelite/scripts/build-completion.bash
```

## Troubleshooting

- **A build problem after changing branches or updating the sources**: run `./build.sh --cmake`.
- **Still failing**: run `./build.sh distclean`, then `./build.sh`. Note that this also removes the wxWidgets build, so the next build is slow.
- **Missing sub-modules**: run `git submodule update --init --recursive`.
- **On Windows**: make sure you are in the `MSYS2` terminal, in the `CLANG64` (or `CLANGARM64`) environment.
