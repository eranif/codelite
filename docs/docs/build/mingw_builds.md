# MinGW specific
---

This page contains tips for building various components using `MinGW` for Windows
For the purpose of this document, we will assume the following:

- [x] `cmake` is installed and placed under `C:\cmake`
- [x] `gcc` is installed and placed under `C:\compilers\mingw64`

## Prepare a CMD terminal
---

- Open a terminal Window by clicking ++left-windows+r++ and typing `cmd` in the window that pops
- Set both `cmake` & `MinGW` in your `PATH` environment variable

```batch
set PATH=C:\cmake\bin;%PATH%
set PATH=C:\compilers\mingw64\bin;%PATH%
```

## Building zlib
---

- Download [zlib sources from here][1] and extract it (latest version is `1.2.11`)
- Use the following to build `zlib`

```batch
cd c:\src\zlib-1.2.11
mkdir build-release
cmake .. -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=C:\root
mingw32-make -j8 
mingw32-make -j8 install
```

## Building OpenSSL
---

### Using `cmake` (**recommended**)
---

```batch
git clone https://github.com/janbar/openssl-cmake.git
cd openssl-cmake
mkdir build
cd build
cmake .. -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=C:\root
mingw32-make -j16 && mingw32-make install
```

### Using `configure` (**not recommended**)
---

Building OpenSSL for MinGW is a bit more trickier than `zlib`, it requires `MSYS` terminal:

- Get `MSYS2` from [here][2]
- Get latest OpenSSL source from [here][3]
- Install [`perl` for Windows from here][4] and add it to your `PATH` environment variable

Assuming that you have extracted your OpenSSL sources into `C:\src\openssl-1.0.2k`, Open an MSYS terminal and type:

```bash
cd /c/src/openssl-1.0.2k
perl Configure mingw64 no-shared no-asm --prefix=/c/root
mingw32-make -j8 depend
mingw32-make -j8
make install
```

You should now have OpenSSL installed under `C:\root` (this is what `--prefix=/c/root` means)

!!! Tip
    The above builds for 64-bit, to build for 32-bit change the configure line to:
    ```bash
    perl Configure mingw no-shared no-asm --prefix=/c/root
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

- Open the file `src\CMakeLists.txt`
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
cmake .. -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=C:\root -DZLIB_ROOT_DIR=C:\root -DOPENSSL_ROOT_DIR=C:\root
mingw32-make -j16 && mingw32-make install
```

[1]: https://www.zlib.net/zlib1211.zip
[2]: https://www.msys2.org/#installation
[3]: https://www.openssl.org/source/
[4]: https://strawberryperl.com/
[5]: #building-zlib
[6]: #building-openssl