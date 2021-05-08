## Creating a CMake-based project
---

### Quick start

Creating a `CMake` based project is the same process as described in the [Quick Start Guide](../hello_world.md#hello-world-program)
with one change: under the `Build System` drop down, choose `CMake`

Now, when opening the project settings dialog, you will notice the under `General` &#8594; `Makefile Generator` the option selected is `CMake`

### Building

With `CMake` based project, you should see 3 new entries for the project context menu:

- `Run CMake`
- `Export CMakeList.txt`
- `Open CMakeLists.txt`

To get the build started, select the option `Run CMake` and CodeLite will generate new `CMakeLists.txt` file and will run `CMake` for it

You should output similar to this:

```bash
C:\CMake\bin\cmake.exe C:\Users\Eran\Desktop\Test\CMakeTest  -G"MinGW Makefiles"
-- The C compiler identification is GNU 7.3.0
-- The CXX compiler identification is GNU 7.3.0
-- Check for working C compiler: C:/compilers/mingw64/bin/gcc.exe
-- Check for working C compiler: C:/compilers/mingw64/bin/gcc.exe -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: C:/compilers/mingw64/bin/g++.exe
-- Check for working CXX compiler: C:/compilers/mingw64/bin/g++.exe -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: C:/Users/Eran/Desktop/Test/CMakeTest/cmake-build-Debug/CMakeTest
==== Done ====
```

Now you are ready to build your project, by simply clicking ++f7++
and you should get an output similar to this:

```bash
cd C:\Users\Eran\Desktop\Test\CMakeTest\cmake-build-Debug\CMakeTest && C:/compilers/mingw64/bin/mingw32-make.exe -j24 SHELL=cmd.exe -e
Scanning dependencies of target CMakeTest
[ 50%] Building CXX object CMakeFiles/CMakeTest.dir/main.cpp.obj
[100%] Linking CXX executable ..\output\CMakeTest.exe
[100%] Built target CMakeTest
====0 errors, 0 warnings====
```

!!! TIP
    Remember to re-run `Run CMake` whenever you are adding new files to the project


!!! TIP
    CodeLite always builds outside of the source tree in a separate folder.
    For example, for the `Debug` configuration, it will create the following build folder:
    `WORKSPACE_PATH/cmake-build-Debug` and will instruct `CMake` to place all build files there


You can customise the `CMake` command from the project settings dialog, under `General` &#8594; `Makefile Generator` &#8594; `Arguments`

For example: in order to enable `Debug` build with a custom `C++` compiler, you can set this in the `Arguments` field:

```cmake
-DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=/opt/gcc/gcc-10
```

