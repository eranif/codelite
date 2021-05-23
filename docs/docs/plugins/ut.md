# UnitTest++
---

!!! warning
    The `UnitTest++` project is [soon to be archived][1], I do not recommend new users to use it for
    their projects
    
## Getting started
---

- Get the sources from GitHub: `git clone https://github.com/unittest-cpp/unittest-cpp`
- Build it using `CMake`:

```bash
 cd unittest-cpp
 mkdir build-release
 cd build-release
 cmake -DCMAKE_BUILD_TYPE=Release ..
 make -j4
 sudo make install
```

## Creating a new UniTest++ project
---

!!! important
    This plugins only work with the [default C++ workspace][2]
    
- From the menu, go to `Workspace` &#8594; `New Project` and select project category to `UnitTest++`
- Fill in the project details and click `OK`. 
 
A new project with a new file is created with a single `main.cpp` file. This file is heavily documented with examples on how manually to add tests

- From `Project Settings` &#8594; `Compiler` &#8594; `Include Paths` add the path to where `UnitTest++` installed its headers (on Linux, it's installed under `/usr/local/include`)
- From `Project Settings` &#8594; `Linker` &#8594; `Library search paths` add the path to `libUnitTest++.a` (on Linux, it's installed under `/usr/local/lib/`) 
- From `Project Settings` &#8594; `Linker` &#8594; `Libraries`, make sure it includes `libUnitTest++.a`

 [1]: https://github.com/unittest-cpp/unittest-cpp
 [2]: /workspaces/default/
