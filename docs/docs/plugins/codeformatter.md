# Source Code Formatter Plugin
---

## General
---

The source code formatter plugin integrates various code formatters (aka beautifiers) tools into CodeLite
Once integrated, you can format your code using a single key stroke, by default it is set to ++ctrl+i++

## Tool configurations
---

Each supported tool contains the following properties:

Property name | Description
--------------|------------
`Enabled` | controls whether the formatter is enabled or not
`Inplace edit` | tells CodeLite whether the formatter outputs the fixed source to stdout, or edit the file directly
`Format on save?` | enable this if you want to format files automatically after saving them
`Working directory` | CodeLite executes the formatter from this directory. [Macros][3] are allowed. By default CodeLite uses `$(WorkspacePath)`
`Command` | the command to execute. [Macros][3] are allowed

## Formatting with the language server
---

The `LSP` formatter asks the [language server][4] of the file to format it (`textDocument/formatting`).
It has only the `Enabled`, `Format on save?` and `Supported languages` properties. It is used when:

- No formatter higher in the list is enabled for the file's language
- The file's language is in its `Supported languages` list (all languages by default)
- A language server is running for the file, and it supports formatting

To use the language server instead of a command formatter (for example `clangd` instead of `clang-format`),
disable the command formatter. To never use the language server for a language, remove that language from the
`LSP` formatter's `Supported languages`.

The language server can only format files that are open in an editor. Formatting a whole project or folder
does not use it.

When text is selected, `Format Current Source` (++ctrl+i++) formats only the selection
(`textDocument/rangeFormatting`). If the language server can not format a selection, nothing is changed and the
status bar says so. Without a selection, and when formatting on save, the whole file is formatted.

## Upgrading CodeLite
---

New versions of CodeLite might introduce a new supported formatters.
However, CodeLite will not override your local configuration, in order
to enable any new formatter added by a more recent version of CodeLite, click
the `Defaults` button in the `Options` dialog from:
`Plugins → Source Code Formatter → Options...`

## Integration with remote workspace
---

Once a remote workspace is loaded (e.g. via [Remoty][1] plugin), the plugin replaces the commands defined in the UI
with the commands taken from [`codelite-remote.json`][2] configuration file.


## Installing tools
---

=== "Windows"
    Language | Tool name | Command to install
    --------|-------------|-----------------------
    C/C++ | clang-format| `pacman -Sy mingw-w64-clang-x86_64-clang-tools-extra`
    JSON | jq| `pacman -Sy mingw-w64-clang-x86_64-jq`
    Python | black | `pip install black --upgrade`
    Rust | rustfmt | `pacman -Sy mingw-w64-clang-x86_64-rust`
    Xml | xmllint | `pacman -Sy mingw-w64-clang-x86_64-libxml2`
    Yaml | yq | `wget https://github.com/mikefarah/yq/releases/download/v4.28.1/yq_windows_amd64.exe|mv yq_windows_amd64.exe /usr/bin/yq.exe`
    CMake | cmake-format | `pip3 install cmake-format`

=== "Ubuntu"
    Language | Tool name | Command to install
    --------|-------------|-----------------------
    C/C++ | clang-format| `brew install llvm`
    JSON | jq| `sudo apt-get install jq`
    Python | black | `pip install black --upgrade`
    Rust | rustfmt | `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`
    Xml | xmllint | `sudo apt-get install libxml2-utils`
    Yaml | yq | `brew install yq`
    CMake | cmake-format | `pip3 install cmake-format`

=== "macOS"
    Language | Tool name | Command to install
    --------|-------------|-----------------------
    C/C++ | clang-format| `brew install llvm`
    JSON | jq| `brew install jq`
    Python | black | `pip install black --upgrade`
    Rust | rustfmt | `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`
    Xml | xmllint | Already installed
    Yaml | yq | `brew install yq`
    CMake | cmake-format | `pip3 install cmake-format`

 [1]: /plugins/remoty
 [2]: /plugins/remoty/#remote-configuration-codelite-remotejson
 [3]: /settings/macros
 [4]: /plugins/lsp

