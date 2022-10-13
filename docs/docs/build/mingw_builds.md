# MinGW specific
---

This page contains tips for building various components using `MinGW` for Windows

## Prepare a working environment
---

Before you start, make sure you have a [proper MSYS2 environment][2]

## Building `zlib`
---

- Setup a [proper MSYS2 environment][2] if you hadn't done this before
- Open `MSYS2` terminal, and type:

```batch
wget https://www.zlib.net/zlib1211.zip
unzip zlib1211.zip
cd zlib-1.2.11
mkdir build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=$HOME/root
mingw32-make.exe -j$(nproc) install
```

## Building `OpenSSL`
---

```batch
git clone https://github.com/janbar/openssl-cmake.git
cd openssl-cmake
mkdir build-release
cd build-release
cmake .. -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=$HOME/root
mingw32-make.exe -j$(nproc) install
```

## Building `libssh`
---

Before we can get started:

- [Build zlib][5]
- [Build OpenSSL][6]
- Clone the sources

```batch
git clone https://git.libssh.org/projects/libssh.git libssh-git
cd libssh-git
```

Now, `libssh` does not build out of the box for MinGW, a small change is required in `CMakeLists.txt`

- Open the file `src/CMakeLists.txt`
- Search for the line that contains `set(libssh_SRC`
- Just before that line, paste this block:

```cmake
if (WIN32)
  set(LIBSSH_LINK_LIBRARIES
    ${LIBSSH_LINK_LIBRARIES}
    ws2_32
  )
endif (WIN32)
```

- Start the build process:

```batch
mkdir build-release
cd build-release
cmake .. -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=$HOME/root -DOPENSSL_ROOT_DIR=$HOME/root
mingw32-make.exe -j$(nproc) install
```

**Congratulations!** you should now have all libraries built and installed under: `$HOME/root`

[1]: https://www.zlib.net/zlib1211.zip
[2]: /getting_started/windows
[3]: https://www.openssl.org/source/
[4]: https://strawberryperl.com/
[5]: #building-zlib
[6]: #building-openssl
