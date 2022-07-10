## What is Debug Adapter Client?
---

This plugin implements the DAP protocol for allowing CodeLite to communicate with third party debuggers

from the [dap website][1]:

Adding a debugger for a new language to an IDE or editor is not only a significant effort, but it is also frustrating that
this effort can not be easily amortized over multiple development tools, as each tool uses different APIs for implementing the same feature.

The idea behind the Debug Adapter Protocol (DAP) is to abstract the way how the debugging support of development tools
communicates with debuggers or runtimes into a protocol. Since it is unrealistic to assume that existing debuggers or runtimes adopt this protocol any time soon,
we rather assume that an intermediary component - a so called Debug Adapter - adapts an existing debugger or runtime to the Debug Adapter Protocol.

The Debug Adapter Protocol makes it possible to implement a generic debugger for a development tool that can communicate
with different debuggers via Debug Adapters. And Debug Adapters can be re-used across multiple development tools which
significantly reduces the effort to support a new debugger in different tools.

The Debug Adapter Protocol is a win for both debugger providers and tooling vendors!

## Install Debug Adapters
---

CodeLite does not install any debug adapter, this needs to be done manually.
You can visit [microsoft implementation page][2] to see the full list.

=== "lldb-vscode"
    **Windows**

    - Setup a [MSYS2 working environment][3]
    - Open `MSYS2` terminal and type: `pacman -S mingw-w64-clang-x86_64-toolchain`
    - You should now have `lldb-vscode` installed under `/clang64/bin/lldb-vsocde`

    **Linux**

    Most linux distros offer `lldb-vscode` in their repository. For example, under `Ubuntu 20.04`
    you can install it by typing (you can install a newer version if available, the newer, the better):

    `sudo apt update && sudo apt install lldb-12`

    **macOS**

    `lldb-vscode` is installed using `brew`:

    `brew install llvm`


Once installed, go to:

- `Settings` &#8594; `Debug Adapter Client`
- Click on the magnifier button
- Click `Ok`

if followed the instructions, CodeLite should be able to pick `lldb-vscode` and configure it.

## The configuration dialog
---

The configuration dialog is minimal and contains the following fields:

| Property | Description |
|------------------|-------------|
| Command | the debug adapter server execution command line |
| Connection string | an example for such connectin string: `tcp://127.0.0.1:12345`|
| Use relative paths| CodeLite will pass and convert files paths into relative paths before sending them to the dap|
| Use forward slash | (Windows only) Always use forward `/` slash |
| Use volume | (Windows only) include the volume part of the file path |
| Server environment | before launching the dap, apply this environment variables (in the form of `NAME=VALUE`)|
| Environment format | The server environment variables format, can be `List` or `Dictionary`, depends on the dap|

## Using dap instead of the built-in debuggers
---

### Default C++ workspace
---

- Open the project settings
- Select the `General` tab
- Under the `Debugger` section, click on the drop down control and chaneg the debugger

### File system workspace
---

- Open the workspace settings
- Select the `General` tab
- Change the debugger from the `Debugger` drop down control


 [1]: https://microsoft.github.io/debug-adapter-protocol/
 [2]: https://microsoft.github.io/debug-adapter-protocol/implementors/adapters/
 [3]: /build/mingw_builds/#prepare-a-working-environment

