# CMakePlugin

CMake plugin for CodeLite IDE

## Build

### Requirements

Building this plugin requires:

* CodeLite sources.
* wxWidgets (> 2.9.5).
* Placing plugin's sources into "CMakePlugin" directory inside CodeLite's sources directory.
* C++98 compiler.
* CMake for Linux build.

### Windows

Open CodeLite's workspace in CodeLite and add CMakePlugin project into the workspace. You can build the plugin now but linker will need to build some CodeLite's libraries (or just build LiteEditor project before).

### Linux

Modification of the main CodeLite's CMakeLists.txt is required because current version (5.3) of the CodeLite doesn't support passing a list of user plugins as a cmake argument. Just add a line:

```CMake
add_subdirectory(CMakePlugin)
```

inside of block that starts with (at the end)

```CMake
if ( NOT NO_CPP_PLUGINS )
```

and then follows CodeLite's build instructions.

### Others

Others OS have not been tested, sorry.

## Manual

TODO

