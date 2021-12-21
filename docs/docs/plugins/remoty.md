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

=== "Linux / macOS"
    * Make sure your have `/usr/bin/ssh` in your `PATH` (should be by default)

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

## Configuring Code Completion
---

CodeLite launches [Language Servers][5] on the remote machine as described in the `codelite-remote.json`
configuration file.

### `codelite-remote.json` file
---

- Open a terminal on your remote machine, or connect to it via SSH
- Type:
    ```bash
    mkdir -p ~/.codelite-remote.json
    touch ~/.codelite-remote/codelite-remote.json
    ```
- Add [Language Servers][5] entries so you will end up with `~/.codelite-remote.json` that looks similar to this:
    ```json
    {
     "Language Server Plugin": {
      "servers": [{
        "name": "clangd",
        "command": "/usr/bin/clangd-12 -limit-results=500 -header-insertion-decorators=0",
        "languages": ["c", "cpp"],
        "priority": 90,
        "working_directory": ""
       }, {
        "name": "python",
        "command": "/usr/bin/python3 -m pylsp",
        "languages": ["python"],
        "priority": 80,
        "working_directory": ""
       }]
     }
    }
    ```
- Modify the `command` field in the above example, to match the actual paths installed on your machine
- Add / remove more `server` entries
- Save the file

!!! Note
    Whenever you modify your `$HOME/.codelite-remote/codelite-remote.json` file,
    remember to reload your workspace in CodeLite (Close followed by Open)

!!! Note
    The following code block contains the complete list of supported languages known to CodeLite:

    ```json
    {
        "bat",        "bibtex",     "clojure",     "coffeescript",  "c",
        "cpp",        "csharp",     "css",         "diff",          "dart",
        "dockerfile", "fsharp",     "git-commit",  "git-rebase",    "go",
        "groovy",     "handlebars", "html",        "ini",           "java",
        "javascript", "json",       "latex",       "less",          "lua",
        "makefile",   "markdown",   "objective-c", "objective-cpp", "perl and perl6",
        "php",        "powershell", "jade",        "python",        "r",
        "razor",      "ruby",       "rust",        "scss",          "sass",
        "scala",      "shaderlab",  "shellscript", "sql",           "swift",
        "typescript", "tex",        "vb",          "xml",           "xsl",
        "yaml"
    };
    ```

## Supported features
---

The current features are supported by `Remoty`

- [Code completion][8]
- Find in files
- Remote building
- Executing target on the remote machine
- Debugging
- Git plugin
- Project management (new class, rename, delete, etc)

## How does it work?
---

Under the hood, CodeLite uploads to the remote machine a python script `codelite-remote`
This script is a mini proxy server capable of executing commands on behalf of CodeLite

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