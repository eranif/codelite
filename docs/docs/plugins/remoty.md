The `Remoty` plugin is an extension to the [File System Workspace][1]. It allows the user to open 
[File System Workspace][1] based workspaces that resides on a remote machine over SSH.

All the commands set in the workspace, are executed on the **remote** machine via `codelite-remote` (see below)

## Prerequisites
---

* Make sure that the `Remoty` is enabled:
* From the main menu bar: `Plugins` &#8594; `Manage plugins` make sure that the `Remoty` plugin is checked
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
the folder is located on the remote machine

## Supported features
---

The current features are supported by `Remoty`

- Code completion (via the [Language Server Plugin][2])
- Find in files
- Remote building
- Executing target on the remote machine
- Debugging
- Git plugin
- Project management (new class, rename, delete, etc)

## How does it work?
---

Under the hood, CodeLite uploads to the remote machine a one file script `codelite-remote`
This script is a mini proxy servery capable of executing commands on behalf of CodeLite

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
