# MinGW Specific
---

This page provides tips for building various components with **MinGW** on Windows.

## Install Missing Libraries
---

CodeLite needs a few extra libraries before it can be built. Follow these steps to install them:

1. Ensure you have a [proper MSYS2 environment][1] installed.
2. Open an **MSYS2** terminal and run:

```bash
pacman -S mingw-w64-clang-x86_64-zlib \
          mingw-w64-clang-x86_64-libssh \
          mingw-w64-clang-x86_64-hunspell \
          mingw-w64-clang-x86_64-openssl \
          mingw-w64-clang-x86_64-sqlite3 \
          mingw-w64-clang-x86_64-llvm-openmp \
          flex bison
```

[1]: /getting_started/windows