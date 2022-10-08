The `Remoty` plugin is an extension to the [File System Workspace][1]. It allows the user to open
[File System Workspace][1] based workspaces that resides on a remote machine over SSH.

All the commands set in the workspace, are executed on the **remote** machine via `codelite-remote` (see below)

## Prerequisites
---

Before you can use `Remoty`, there are some setup steps needed to be performed on the remote and on
the local machine

### Local machine

=== "Windows"
    * From the main menu bar: open `Plugins` &#8594; `Manage plugins` and make sure that `Remoty` plugin is checked
    * Make sure you have a [working MSYS2 environment][3]
    * From the main menu bar: open `Settings` &#8594; `Environment Variables`
    * Add the following line: `PATH=C:\msys64\mingw64\bin;$PATH`
    * Ensure you have a working `ssh` command line and its in your `PATH`
    * You are able to SSH to the remote machine **without password** (only via keys)

=== "Linux / macOS"
    * Make sure your have `/usr/bin/ssh` in your `PATH` (should be by default)
    * You are able to SSH to the remote machine **without password** (only via keys)

### Remote machine

* Python3 installed and in the `PATH` environment variable of the remote machine

## Open an existing workspace
---

* From the main menu bar: `File` &#8594; `Open` &#8594; `Open Workspace`
* In the `Switch to workspace` dialog that opened, click on the **bottom** `Browse` button (from the `Remote workspace` line)
* Select the workspace file on the remote machine
* Click `OK`

## Creating a new workspace
---

Creating a new workspace is similar to creating a File System Workspace, the difference is that
the folder is located on the remote machine.

## Remote configuration - `codelite-remote.json`
---

CodeLite uses a special file `WORKSPACE_PATH/.codelite/codelite-remote.json` to configure remote tools for the following plugins:

- [Language Servers Plugin][5] - all the LSP entries are placed under the `Language Server Plugin` property
- [Source Code Formatter Plugin][9] - all the tools are placed under the `Source Code Formatter` property

If this file does not exist for you, you can create a skeleton file by:

- Right click on the top level folder in the tree view
- Choose `Edit codelite-remote.json`
- Edit the file (update the commands, delete unwanted entries etc)
- Save the file
- CodeLite will prompt to you reload the workspcae, accept the offer

The skeleton file looks like this:

```json
{
  "Language Server Plugin": {
    "servers": [
      {
        "command": "/usr/bin/clangd -limit-results=500 -header-insertion-decorators=1",
        "env": [],
        "name": "clangd",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "rust-analyzer",
        "env": [],
        "name": "rust-analyzer",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "python3 -m pylsp",
        "env": [
          {
            "name": "PYTHONPATH",
            "value": ".:$PYTHONPATH"
          }
        ],
        "name": "python",
        "working_directory": "$(WorkspacePath)"
      }
    ]
  },
  "Source Code Formatter": {
    "tools": [
      {
        "command": "jq . -S $(CurrentFileRelPath)",
        "name": "jq",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "clang-format $(CurrentFileRelPath)",
        "name": "clang-format",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "xmllint --format $(CurrentFileRelPath)",
        "name": "xmllint",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "rustfmt --edition 2021 $(CurrentFileRelPath)",
        "name": "rustfmt",
        "working_directory": "$(WorkspacePath)"
      }
    ]
  }
}
```

!!! Important
    Each entry in the file: `codelite-remote.json` **MUST** match a configuration entry in your UI
    Entries that do not have a matching UI entry are ignored

    e.g. if you add a language server entry named `clangd`, you must have an entry named `clangd`
    defined under `Plugins` &#8594; `Language Server` &#8594; `Settings...`

    The same applies for the `Source Code Formatter` entries

## Supported features
---

The current features supported by `Remoty`

- [Code completion][8]
- [Source code formatting][9]
- Find in files
- Remote building
- Executing target on the remote machine
- Debugging
- Git plugin
- Project management (new class, rename, delete, etc)

## How does it work?
---

Under the hood, CodeLite uploads to the remote machine a python script `codelite-remote`
This script serves as a proxy server capable of executing commands on behalf of CodeLite

For example, running `ps -ef|grep codelite-remote` on the remote machine will output this:

```bash
eran@DESKTOP-F7URLL7:~/devl/codelite/docs$ ps -ef|grep codelite-remote
eran     17131 17117  0 16:47 ?        00:00:00 python3 /home/eran/devl/codelite/docs/.codelite/codelite-remote --context finder
eran     17132 17107  0 16:47 ?        00:00:00 python3 /home/eran/devl/codelite/docs/.codelite/codelite-remote --context builder
eran     17133 17130  0 16:47 ?        00:00:00 python3 /home/eran/devl/codelite/docs/.codelite/codelite-remote --context git
eran     17248    10  0 16:47 pts/0    00:00:00 grep --color=auto codelite-remote
```

 [1]: /workspaces/file_system
 [2]: /plugins/lsp
 [3]: /build/mingw_builds#prepare-a-working-environment
 [4]: how-does-it-work
 [5]: /plugins/lsp/
 [6]: /plugins/lsp/#install-clangd-c
 [7]: /plugins/lsp/#install-pylsp-python
 [8]: /plugins/remoty/#configuring-code-completion
 [9]: /plugins/codeformatter