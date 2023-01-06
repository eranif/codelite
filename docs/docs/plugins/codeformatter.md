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

