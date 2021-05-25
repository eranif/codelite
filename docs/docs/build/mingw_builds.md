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
cmake .. -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles"
mingw32-make -j8
```

## Building OpenSSL
---

Building OpenSSL for MinGW is a bit more trickier than `zlib`, it requires `MSYS` terminal:

- Get `MSYS2` from [here][2]
- Get latest OpenSSL source from [here][3]
- Install [`perl` for Windows from here][4] and add it to your `PATH` environment variable

Assuming that you have extracted your OpenSSL sources into `C:\src\openssl-1.0.2k`, Open an MSYS terminal and type:

```bash
cd /c/src/openssl-1.0.2k
perl Configure mingw64 no-shared no-asm --prefix=/c/OpenSSL
mingw32-make -j8 depend
mingw32-make -j8
make install
```

You should now have OpenSSL installed under `C:\OpenSSL` (this is what `--prefix=/c/OpenSSL` means)


!!! Tip
    The above builds for 64-bit, to build for 32-bit change the configure line to:
    ```bash
    perl Configure mingw no-shared no-asm --prefix=/c/OpenSSL
    ```



[1]: https://www.zlib.net/zlib1211.zip
[2]: https://www.msys2.org/#installation
[3]: https://www.openssl.org/source/
[4]: https://strawberryperl.com/
