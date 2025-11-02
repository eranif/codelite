# Language Server Protocol

## Overview

The Language Server Protocol (LSP) is a standardized protocol used between development environments and language intelligence providers. It enables the integration of advanced features such as auto-completion, go-to-definition, and find-all-references directly into your development tool. For more information, visit the [Language Server Protocol website][1].

---

## Installing Language Servers

The following sections provide installation instructions for the most commonly used language servers. After installing your desired language servers, proceed to [configure them in CodeLite](#automatic-detection).

### clangd (C/C++/Objective-C)

`clangd` is the official Language Server Protocol implementation from the Clang team, providing compiler-level code completion with exceptional accuracy for C, C++, and Objective-C. Visit the [clangd project page][4] for more information.

#### Prerequisites

`clangd` requires either a `compile_commands.json` or `compile_flags.txt` file to function. When using CodeLite's default C++ workspace, CodeLite can automatically generate this file:

1. Navigate to **Settings → Code Completion** from the menu bar
2. Enable the **Generate compile_commands.json file** option
3. Build your project

> **Note:** `clangd` relies on the `compile_commands.json` file to obtain build instructions for each source file. When adding new files to your project, you must ensure that `compile_commands.json` is updated to include these files. Additionally, `clangd` [requires a manual restart][17] after `compile_commands.json` is modified, as it does not automatically detect configuration changes.

#### Installation by Platform

**Windows**

Install `clangd` by installing the recommended packages as [described in the documentation][15].

**Linux**

Install `clangd` manually using your distribution's package manager. On Ubuntu and Debian, it is typically part of the `clang-tools` package:

```bash
sudo apt-get update
sudo apt-get install clang-tools
```

After installation, proceed to the [manual configuration section](#manual-configuration).

!!! Note
    The `clang-tools` package often includes a version number (e.g., `clang-tools-10`). Install the package with the highest available version number.

**macOS**

On macOS, `clangd` is provided as part of the `llvm` formula. Install it using Homebrew:

```bash
brew install llvm
```

On ARM-based macOS systems, `clangd` is located at: `/opt/homebrew/opt/llvm/bin/clangd`

### ctagsd (C/C++)

`ctagsd` is CodeLite's built-in code completion engine for C/C++ that implements the Language Server Protocol. It is automatically installed with CodeLite on all platforms.

CodeLite pre-configures `ctagsd`, but it is disabled by default. To enable it:

Navigate to **Plugins → Language Server → Settings**

### pylsp (Python)

`pylsp` is installed via `pip` and requires Python 3 and pip3 to be installed on your system.

**Standard Installation**

```bash
pip install python-lsp-server
```

**Windows (MSYS2)**

```bash
pacman -S mingw-w64-clang-x86_64-python mingw-w64-clang-x86_64-python-pip mingw-w64-clang-x86_64-python-ujson
pip install python-lsp-server
```

Visit the [python-lsp-server project page][3] for additional information.

### TypeScript

**Prerequisites**

- [Install Node.js][11]

**Installation**

```bash
npm install -g typescript typescript-language-server
```

If you choose to [configure it manually][12] in CodeLite, use the following command:

```bash
typescript-language-server --stdio
```

### rust-analyzer (Rust)

`rust-analyzer` is the recommended Language Server Protocol implementation for the Rust language.

**Prerequisites**

- [Install Rust][13]

**Installation**

**macOS and Linux**

```bash
rustup update
rustup +nightly component add rust-src rust-analyzer-preview
```

**Windows**

Build from sources using the MSYS2 terminal:

```bash
pacman -Sy mingw-w64-clang-x86_64-rust mingw-w64-clang-x86_64-rust-src
```

**Locating the Binary**

After installation, `rust-analyzer` is located in the rustup local folder. On Linux and macOS, you can find it using:

```bash
TARGET=$(rustup target list|grep installed|cut -d" " -f1)
$HOME/.rustup/toolchains/nightly-$TARGET/bin/rust-analyzer
```

### gopls (Go)

`gopls` (pronounced "Go Please") is the official Language Server Protocol implementation for Go.

**Installation**

```bash
go install golang.org/x/tools/gopls@latest
```

---

## Configuration

### Automatic Detection

After installing your preferred language servers, configure CodeLite to detect them automatically:

1. Navigate to **Plugins → Language Server → Settings**
2. Click the **Scan** button

### Manual Configuration

You can install and configure any language server that implements the LSP protocol. For a complete list of available LSP implementations, visit [this resource][2].

**Configuration Steps**

1. Install the desired LSP on your system
2. Navigate to **Plugins → Language Server → Settings**
3. Click the **Add** button
4. Fill in the required fields in the dialog:

| Field | Mandatory | Description |
|-------|-----------|-------------|
| Enabled | ✓ | Enable or disable this LSP |
| Name | ✓ | Provide a descriptive name for this LSP |
| Remote server | - | When enabled, the command is executed on a remote machine |
| Command | ✓ | The LSP execution command (e.g., `/usr/bin/clangd` for clangd) |
| Working directory | - | Path to set before running the command |
| Languages | ✓ | Associate this LSP with specific languages. Use the `...` button to view supported languages |
| Connection string | ✓ | Protocol used for communication (most LSP servers support `stdio`) |
| Display diagnostics | - | When checked, CodeLite displays error indicators in the editor |

### Resolving Conflicts

Multiple language servers can be configured for the same programming language (e.g., both `ctagsd` and `clangd` for C++). However, ensure that only one is enabled at a time to avoid conflicts.

---

## Restarting Language Servers

To restart the any language server:

1. Press ++ctrl+shift+p++ to open the command palette
2. Type `restart language` and select the appropriate command
3. Hit ++enter++

---

## CMake Integration

### Using clangd and ctagsd with CMake

Both `clangd` and `ctagsd` rely on `compile_flags.txt` or `compile_commands.json` files for build instructions. These language servers search for these files starting from the active file's directory and traversing up the parent directories until a match is found.

If both files exist in the same directory, `compile_flags.txt` takes precedence.

**CodeLite Default C++ Workspace**

When using CodeLite's [default C++ workspace](../workspaces/default.md), these files are generated automatically after the build process completes.

**Other Workspace Types**

When using other workspace types (e.g., [File System Workspace](../workspaces/file_system.md)), you must provide at least one of these files manually for `clangd` code completion to function.

### Generating compile_commands.json with CMake

To generate `compile_commands.json` using CMake, add the following to your top-level `CMakeLists.txt` file:

**Option 1: Modify CMakeLists.txt**

Add this line at the top of your `CMakeLists.txt`:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Add this command near the bottom of the top-level `CMakeLists.txt`:

```cmake
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
                ${CMAKE_SOURCE_DIR}/compile_commands.json)
```

**Option 2: CMake Command Line**

Alternatively, pass the variable during CMake invocation:

```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 <other arguments>
```

After running CMake, an up-to-date `compile_commands.json` file will be generated for `clangd`.

### Generating compile_commands.json for Makefile Projects

For traditional Makefile-based projects, use the `cc-wrapper` utility included with CodeLite. Refer to the [cc-wrapper documentation][16] for detailed instructions.

---

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
[16]: /misc/cc_wrapper
[17]: #restarting-language-servers