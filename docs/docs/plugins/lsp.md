## What is Language Server?
---

From the [Language Server web site][1]: The Language Server Protocol (LSP) is used between the IDE and a language smartness provider (the server) to integrate features like auto complete, go to definition, find all references and alike into the tool 

## Configuring common Language Servers
---

By default, CodeLite is able to detect and configure language server for:

- [Python `pylsp` module][6]
- [C++ (`clangd`)][7]
- [Rust (`rls`)][8]

On startup CodeLite will attempt to configure all the above language servers.
However, if you installed them after you started CodeLite, you will need to tell 
CodeLite to run the scan process again. To do this:

- Goto `Plugins` &#8594;  `Language Server` &#8594; `Settings`
- Click on the `Scan` button

!!! Important
    CodeLite will configure the LSP for you, but it will **NOT** install it for you.
    See below steps for installing language servers

### Install `pylsp` (python)
---

`pylsps` is installed via `pip`. For this, you will need to install the following:

- python 3 installed
- pip3 installed

```bash
pip install python-lsp-server
```

Visit the [project home page][3]

### Install `clangd` (c++)
---

`clangd` is the LSP implementation from the `clang` team. It provide a compiler level completion with an unmatched accuracy


Visit the [project home page][7]

#### Windows

On Windows, CodeLite comes with a pre built binary of `clangd` configured and installed.

#### Linux

On Linux machines, you will need to manually install `clangd` via your package manager.

On `Ubuntu` and `Debian` it is usually part of the `clang-tools` package. So to install it, use the below code snippet:

```bash
sudo apt-get update
sudo apt-get install clang-tools
```

Once installed, follow the steps in the [manual configuration section](#manual-configuration)

!!! Note
    Often, the `clang-tools` package comes with a version number, e.g. `clang-tools-10`
    Make sure to install the one with the highest number 

#### macOS

Like Windows, `clangd` in part of CodeLite bundle

### Install `rls` (rust)
--- 

- Install [`rustup`][5]
- Then type:

```bash
rustup update
rustup component add rls rust-analysis rust-src
```

Visit the [project home page][8]

!!! Important
    The `rls` language server must be started from the cargo project directory
    i.e. the location of the `Cargo.toml` file

## Manual configuration
---

To manually configuring new LSP in CodeLite follow these steps:

- Install the LSP you want on your computer. You can [visit this site][2] to get a complete list of all LSP implementations out there
- From the main menu, `Plugins` &#8594; `Language Server` &#8594; `Settings...`
- You can use the `Scan` button to see if CodeLite supports auto detection of your LSP. If you are in luck, then you can skip the rest of the steps
- Click on the `Add` button
- In the dialog that opens, fill the mandatory fields:

Field   | Mandatory | Description 
--------|-----------|-------------
Enabled | &#10003;        | Is this LSP enabled?
Name    | &#10003;       | Provide a descriptive name for this LSP
Remote server| -   | When enabled, the `Command` is executed on a remote machine
Command | &#10003;       | The LSP execution command. For example, for `clangd`, the command can be as simple as `/usr/bin/clangd`
Working directory | - | Path to set before running the command
Languages | &#10003; | Since CodeLite can have multiple LSP configured, you can associate each LSP with a language. CodeLite will use the proper LSP based on the current file's language. Use the `...` button to see a list of supported languages
Connection string| &#10003; | Defines the protocol that CodeLite communicates with the LSP. Most of the LSP servers out there supporting the `stdio` protocol
Priority | &#10003; | Defines this LSP priority compared to other LSPs for the same language, the higher the better
Display diagnostics | - | When checked, CodeLite will display little red arrows next to potential code errors

## Conflicts
---

In addition to code completion provided by the Language Server plugin, CodeLite has list of internal code completion providers
which are not part of the Language Server plugin, each provider has its own internal priority:

Provider |Language |Priority
---------|---------|---------
C++ by ctags |C/C++ |75
PHP plugin |PHP |50
WebTools |XML |50
WebTools |CSS |50
WebTools |JavaScript |50 

Lets explain how the priority resolves conflicts for `C++`:

Assuming your configured language server for `C++` with priority `80`, it will **override** the default `ctags` completion engine.
However, if your language server is configured with priority `74` (or less), CodeLite will use `ctags` completion engine **first**

and in case `ctags` completion engine fails to complete, CodeLite will use the next language server for `c++` language with the next highest priority.
This mechanism allows you to control how CodeLite uses the language servers in conjunction with the internal completion engines

## CMake and `clangd`
---

### `compile_flags.txt` & `compile_commands.json`

`clangd` uses these two files as instructions for how to build any source file.
`clangd` searches for these file from the active file folder and going up to the parent until it finds a match (or it can't go up any more)
In case both files are found on the same directory, `compile_flags.txt` takes precedence.

When using CodeLite's [default C++ workspace](../workspaces/default.md) CodeLite generate these files for your automatically once the build process is completed

However, when using other workspaces (e.g. the [File System Workspace](../workspaces/file_system.md)) you need to provide these files (at least one of them) in order for `clangd` code completion to function.

If you are using `cmake` as your build system, you can add these two commands in the top level `CMakeLists.txt` file:

- At the top of your `CMakeLists.txt` add this line:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

- and somewhere at the bottom of the top level `CMakeLists.txt` file, add this command:

```cmake
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json 
                ${CMAKE_SOURCE_DIR}/compile_commands.json)
```

this way, after running `cmake`, you will get an up-to-date `compile_commands.json` file for `clangd`

[1]: https://langserver.org/
[2]: https://langserver.org/#implementations-server
[3]: https://github.com/python-lsp/python-lsp-server
[4]: https://clangd.llvm.org/
[5]: https://rustup.rs/
[6]: #install-pylsp-python
[7]: #install-clangd-c
[8]: #install-rls-rust
