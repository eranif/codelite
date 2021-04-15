# C++ workspace
---

This is the recommended workspace when developing a `C/C++` project from scratch.
The C++ workspace organizes all the projects in a tree view

## Project
---

The workspace does not contain real information about your code.
Instead, all of your code info (how to build, what to build etc) is stored in the project file.

If you are familiar with `Makefile`, a project contains all the information needed to construct a `Makefile` to build
your code into a binary

Keeping the `Makefile` analogy, each project in the workspace defines a single build target, where a build target can be one of:

- Shared Object (`.so`, `.dylib` or `.dll` depends on the platform)
- Static library (`.a`)
- Executable 

Each project, may have multiple build configurations. For example, you may wish to build your executable with debug information
or optimized. So you would define two build configurations: one `Release` and another named `Debug`

To view the project settings, right click on the project and choose `Settings...`

![project settings menu](default_project_settings_menu.png)

![project settings dialogue](default_project_settings_dlg.png)


### General
---

This page contains list of general information about the project

| Project property | Description |
|------------------|-------------|
|`Project enabled`  | enable or disable the project. A disabled project can still be built, but it is excluded from the workspace build |
|`Project type`|Determines the target type (as described above)|
|`Makefile Generator`| CodeLite offers multiple `Makefile` generators, the recommended value here is `CodeLite Makefile Generator`|
|`Compiler`| Choose to compiler to use by this project (more specifically, by this project configuration)|
|`Intermediate directory`| Sets the location of the object files. When `Makefile Generator` is used as the generator, this field is ignored|
|`Output File`| Sets the name of the output file|
|`Pause when execution ends`|When checked, CodeLite will run your program under a terminal which will pause before your program exits|
|`This program is a GUI application`| When checked, CodeLite will avoid wrapping your program with a terminal|
|`Executable to run / debug`|Set the executable to execute. This can be different value from the `Output File`|
|`Working directory`|Sets the working directory for the target execution / debug|
|`Program arguments`|Pass these arguments to the executed program|
|`Debugger`|Choose the debugger to use. Under Windows & Linux it is recommended to use the `GNU GDB Debugger` while under macOS, choose `LLDB`|
|`Use separate debugger args`|When checked, CodeLite will pass to the debugger the program arguments found under the `Debugger Program Arguments` rather than the ones that placed under the `Program Arguments` field|
|`Debugger Program Arguments`|Pass these arguments to the debugger program|

### Compiler
---

Contains all the information needed in order to **compile** your code (this is different from `Linking`):

| Project property | Description |
|------------------|-------------|
|`Compiler is not required for this project` | enable this option if this project does not require compiling|
|`Use with global settings`| See [Global Settings below](#global-settings)|
|`C++ compiler options`|semi colon list of options to pass to the compiler when compiling a C++ source file e.g. `-g;-Wall;-std=c++17`|
|`C compiler options`|semi colon list of options to pass to the compiler when compiling a C source file e.g. `-g;-Wall`|
|`Assembler options`|semi colon list of options to pass to the compiler when compiling a `.s` file (assembly file)|
|`Include Paths`|semi colon list of directories to pass to the compiler for searching header files. It is recommended to use here relative paths, or use environment variables. e.g. `$(HOME)/includes;$(BOOST_DIR)/includes`|
|`Preprocessors`|semi colon list of macros (AKA `defines`) to the compiler, e.g. `USE_GUI=1;NDEBUG=1`|
|`Header file`|Path to the PCH file to use|
|`Explicitly include PCH`|When checked, CodeLite will include the PCH header file in the build command line using the `-include <pch-file>`|
|`PCH Compile flags`|Set here any special compiler options for the building the PCH file. **recommended to leave this empty**|
|`PCH Compiler flags policy`|CodeLite can use the `PCH compile flags` value in addition to the other compiler options as defined in `C++ compiler options` and `C compiler options`. You can set here the policy for how merge between the two|

### Environment
---

Set the environment variables to use when building / running this project.
You can set variables in two ways:

- Use a [pre-defined evnironment variables set](../../settings/environment_variables/#using-pre-defined-set)
- Manually set them for [using the same syntax as explained here](../../settings/environment_variables/#setting-global-environment-variables)

### Linker
---

| Project property | Description |
|------------------|-------------|
|`Use with global settings`| See [Global Settings below](#global-settings)|
|`Linker options`|semi colon list of options to pass to the linker e.g. `-pthread;-s`|
|`Libraries search path`|semi colon list of directories to pass to the compiler for searching libraries. It is recommended to use here relative paths, or use environment variables. e.g. `$(HOME)/lib;$(BOOST_DIR)/lib`|
|`Libraries`|semi colon list of libraries to link agaist. e.g. `boost_fiber;boost_system`|

!!! TIP
    When setting the `Libraries` field, there is no need to include the prefix `lib` or its prefix e.g. `.a`
    so if you want to link against `libboost_fiber.a` and `libboost_regex.a`, your `Libraries` field should look like:
    `boost_fiber;boost_regex`

### Debugger
---

This page allows to override the global debugger settings.
Settings that can be overriden here:

- Custom debugger executable
- Add debugger search paths for source files
- Custom start-up commands 
- Remote attach commands (executed after the debugger attaches a remote target)

You can also specify here if debugging over `gdbserver` by enabling the `Debugging a remote target`. You can [read more about `gdbserver` here](https://sourceware.org/gdb/current/onlinedocs/gdb/Server.html)

### Resources
---

**Windows ONLY**

CodeLite inovke the `winres` compiler for `.rc` files on Windows to compile resources.
Here you can set search path and other resource compiler options (`windres`) similar to the [Compiler page](#compiler)

### Pre / Post Build commands
---

Run list of commands before or after the build process.
As part of the build command, you may use:

- Environment variables
- Project variables (click the `Help` button in the project settings dialog to get a full list of them)

The below example uses the project variable: `OutputFile` and the environment variable `HOME`:

```bash
echo Build of $(OutputFile) completed successfully!
echo my home dir is $(HOME)
```

### Customize / Custom 
---

**DEPRECATED** 
If you need to use a custom build. Please use the [File System Workspace](file_system.md) workspace

### Customize / Custom Makefile Rules
---

This page allows user to specify makefile rule(s) to be executed during the pre-build stage 
(actually, it will be executed before the commands provided in the `Pre Build` page).

!!! Important
    You must be familiar with makefile writing conventions to use this page.

The main difference between the commands executed in the `Pre Build` stage and the commands that will be executed in this rule, 
is that the commands in the `Pre Build` stage are executed unconditionally, while rules set here are executed based on their dependencies.

For example, lets assume that we have a project with a `yacc` & `flex` files that needs to be preprocessed by the `Yacc` & `Flex` tools 
before compilation starts

In the `Dependencies` line, set: `parser.cpp lexer.cpp`

While in the `Rule actions` we place this code:

```Makefile
## rule to generate parser.cpp based on grammar.y
parser.cpp: grammar.y
    @echo Generating parser.cpp and lexer.h ...
    yacc -dl  -t -v grammar.y
    mv y.tab.h lexer.h
    mv y.tab.c parser.cpp

## rule to generate lexer.cpp based on lexer.l
lexer.cpp: lexer.l
    @echo Generating lexer.cpp ...
    flex -L  lexer.l
    mv lex.yy.c lexer.cpp
```

### Global Settings
---

The global settings page contains list of properties that can be used with the following pages:

- Compiler
- Linker
- Resource compiler

On the above pages (`Compiler`, `Linker` and `Resources`) you can set the merging policy:

| Policy | Description |
|--------|-------------|
| Append | The page properties are appended to the global settings|
| Prepend | The page properties are prepended to the global settings|
| Overwrite | The page properties overwrite the global settings|

## Active project
---

All the default keyboard accelerators are applied to the **active** project.

*Example:*

If you hit ++f7++ to start the build process, it will build the **active** project 

