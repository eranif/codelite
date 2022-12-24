# MinGW specific
---

This page contains tips for building various components using `MinGW` for Windows

## Install missing libraries
---

CodeLite requries some additional libraries before it can be built.
This section will help you install these missing libraries:

- Make sure you have a [proper MSYS2 environment][1]
- Open `MSYS2` terminal, and type:

```batch
pacman -S mingw-w64-clang-x86_64-zlib       \
          mingw-w64-clang-x86_64-libssh     \
          mingw-w64-clang-x86_64-hunspell   \
          mingw-w64-clang-x86_64-openssl    \
          mingw-w64-clang-x86_64-sqlite3
```

[1]: /getting_started/windows
