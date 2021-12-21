# Rust plugin
---

In its core, the `Rust` plugin is a fancy wrapper around the [File System Workspace][1]

## Prerequisites
---

=== "Windows"
    - Install [MSYS2 Environment][3]
    - Install the [`rust` toolchain][2]
    - [Install `rust-analyzer`][4]

=== "Linux / macOS"
    - Install the [`rust` toolchain][2]
    - [Install `rust-analyzer`][4]

## Create a new workspace
---

Similar to any other Workspace:

- From the main menu bar, choose `File` &#8594; `New` &#8594; `New workspace`
- Choose the `Rust` workspace
- Choose the folder for the workspace.
- Give the workspace a name (`rust` recommendations are to use `snake_case` naming conventions)
- Once everything is installed, click ++f7++ to build the skeleton `main.rs` file created


 [1]: /workspaces/file_system
 [2]: /misc/install_rust/
 [3]: /build/mingw_builds/#prepare-a-working-environment
 [4]: /plugins/lsp/#install-language-servers