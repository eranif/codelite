## What is Language Server?
---

From the [Language Server web site][1]: The Language Server Protocol (LSP) is used between the IDE and a language smartness provider (the server) to integrate features like auto complete, go to definition, find all references and alike into the tool

## Install Language Servers
---

Below you may find the installation instructions for the most common Language Servers,
select the ones you are interested in and then [configure them in CodeLite][14]

=== "clangd"
    `clangd` is the LSP implementation from the `clang` for `C/C++/Objective-C` team. It provide a compiler level completion with an unmatched accuracy.
    Visit the [project home page][7]

    **All**

    `clangd` relies on a file named `compile_commands.json` or `compile_flags.txt`. When using CodeLite default C++ workspace
    CodeLite can generate this file for the current workspace:

    1. From the menu bar: `Settings` &#8594; `Code Completion ...`
    2. Select the option `Generate compile_commands.json file`
    3. Build your project

    **Windows**

    Install `clangd` by install the recommended packages as [described here][15]

    ** Linux **

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

    ** macOS **

    Like Windows, `clangd` in part of CodeLite bundle

=== "ctagsd"
    `ctagsd` is CodeLite's builtin code completion engine for `C/C++` that implements the Language Server Protocol.
    `ctagsd` is installed along with CodeLite on all platforms.

    CodeLite configures `ctagsd` for you, but it is disabled by default. To enable it:

    `Plugins` &#8594; `Settings` &#8594; `ctagsd`

=== "pylsp"
    `pylsps` is installed via `pip`. For this, you will need to install the following:

    - python 3 installed
    - pip3 installed

    ```bash
    pip install python-lsp-server
    ```

    On Windows / MSYS2, use this command:

    ```bash
    pacman -S mingw-w64-clang-x86_64-python mingw-w64-clang-x86_64-python-pip mingw-w64-clang-x86_64-python-ujson
    pip install python-lsp-server
    ```

    Visit the [project home page][3]

=== "TypeScript"
    - Install [`node`][11]
    - Type:

    ```bash
    npm install -g typescript typescript-language-server
    ```

    If you choose to [configure it manually][12] in CodeLite, use this as the command:

    ```bash
    typescript-language-server --stdio
    ```

=== "rust-analyzer"
    `rust-analyzer` is the recommended LSP for the rust language. To install it, follow these steps:

    - [Install rust][13]
    - Open a terminal and type:
        - On `macOS` and `Linux`:
            ```bash
            rustup update
            rustup +nightly component add rust-src rust-analyzer-preview
            ```
        - On `Windows`, we build it from sources. Open `MSYS2` terminal and type:

            ```bash
            cd ~
            git clone https://github.com/rust-lang/rust-analyzer.git
            cd rust-analyzer
            cargo build --release \
                && mkdir -p ~/.cargo/bin \
                && cp target/release/rust-analyzer.exe target/release/rust-analyzer-proc-macro-srv.exe ~/.cargo/bin
            ```

    You should now have `rust-analyzer` installed under `rustup` local folder, for example, under `Linux` or `macOS`,
    it can be found here:

    ```bash
    TARGET=$(rustup target list|grep installed|cut -d" " -f1)
    $HOME/.rustup/toolchains/nightly-$TARGET/bin/rust-analyzer
    ```

=== "rls"
    `rls` is yet another rust language server, but less popular than `rust-analzyer`

    To install it:

    - [Install rust][13]
    - Open a terminal and type:
        - On `macOS` and `Linux`:
            ```bash
            rustup update
            rustup component add rls rust-analysis rust-src
            ```
        - On `Windows`:
            ```batch
            %USERPROFILE%\.cargo\bin\rustup update
            %USERPROFILE%\.cargo\bin\rustup component add rls rust-analysis rust-src
            ```
    Visit the [project home page][8]

    !!! Important
        The `rls` language server must be started from the cargo project directory
        i.e. the location of the `Cargo.toml` file

---

## Automatic detection
---

Once you have installed your favourite Language Servers, its time to tell CodeLite about it:

- Goto `Plugins` &#8594;  `Language Server` &#8594; `Settings`
- Click on the `Scan` button

## Manual configuration
---

The above instructions are for the most common language servers. However, you can install
and configure any server the follows the LSP protocol. You will however, need to do this manually:

- Install the LSP you want on your computer. You can [visit this site][2] to get a complete list of all LSP implementations out there
- From the main menu, `Plugins` &#8594; `Language Server` &#8594; `Settings...`
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

## Resolving conflicts - using the priority property
---

User can configure multiple language servers for the same coding language (e.g. `ctagsd` and `clangd` for `C++`)
In order to decide which language server to use first, CodeLite uses the `Priority` property of the language server. The server with the highest value is used first

## CMake `clangd` / `ctagsd`
---

### `compile_flags.txt` & `compile_commands.json`

`clangd` and `ctagsd` uses these two files as instructions for how to build any source file.
`clangd` and `ctagsd` will searche for these files from the active file folder and going up to the parent until it finds a match (or it can't go up any more)
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
[9]: #install-rust-analyzer
[10]: #install-typescript-language-server
[11]: https://nodejs.org
[12]: #manual-configuration
[13]: /misc/install_rust
[14]: #automatic-detection
[15]: /getting_started/windows/#common