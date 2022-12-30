# Windows

`CodeLite` is built using `clang++` from the MSYS2 environment and it is the recommended compiler to use on Windows.
This page provides instructions for how to setup a functional working environment using the MSYS2 shell.

## Common
---

- Download and install [MSYS2][2], set the installation folder to `C:\msys64`
- Open `MSYS2` terminal, and install the following packages:

```bash
pacman -Sy git                                  \
           openssh                              \
           mingw-w64-clang-x86_64-gdb           \
           mingw-w64-clang-x86_64-toolchain     \
           mingw-w64-clang-x86_64-python3       \
           mingw-w64-clang-x86_64-cmake         \
           mingw-w64-clang-x86_64-libffi        \
           mingw-w64-clang-x86_64-jq            \
           mingw-w64-clang-x86_64-libxml2       \
           unzip
```

- Next, still in the `MSYS2` terminal, Add the `/clang64/bin` to your `~/.bashrc` and `source` it:

```bash
echo 'export PATH=/clang64/bin:$PATH' >> ~/.$(basename $SHELL)rc
. ~/.$(basename $SHELL)rc
```

## Optional setup
---

### Python

- Setup `pip` for python:

```bash
cd /tmp
wget https://bootstrap.pypa.io/get-pip.py
/clang64/bin/python3 /tmp/get-pip.py
rm -f /tmp/get-pip.py
```

- Install debugger, language server and formatter for python:

```bash
# Python code completion provider
/clang64/bin/python3 -m pip install python-lsp-server --upgrade

# Python formatter tool
/clang64/bin/python3 -m pip install black --upgrade

# Python debugger
/clang64/bin/python3 -m pip install debugpy --upgrade
```

### Rust

```bash
# installs cargo, rustfmt, clippy etc
pacman -S mingw-w64-clang-x86_64-rust
```

[2]: https://www.msys2.org/#installation
