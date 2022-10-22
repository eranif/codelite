## What is the Debug Adapter Client?
---

This plugin implements the DAP protocol, which allows CodeLite to communicate with third party debuggers.

From the [dap website][1]:

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

CodeLite does not install any debug adapter; this needs to be done manually.
Visit the [microsoft implementation page][2] to see the full list.

### lldb-vscode

A wrapper for the `lldb` debugger that implements the Debug Adapter Protocol

=== "Linux"

    We recommend to install `lldb-vscode` via `brew` (yes, use `brew` on Linux!)

    - [Install brew for Linux][6]
    - Run: `brew install llvm`

=== "macOS"

    `lldb-vscode` is installed using `brew`:

    - [Install brew for macOS][7]
    - Run: `brew install llvm`

=== "Windows"
    - Setup a [MSYS2 working environment][3]
    - Open `MSYS2` terminal and type:

    ~~~
    pacman -Sy  mingw-w64-clang-x86_64-toolchain \
                mingw-w64-clang-x86_64-python3 \
                mingw-w64-clang-x86_64-libffi
    ~~~
    - You should now have `lldb-vscode` installed under `/clang64/bin/lldb-vsocde`

----

### debugpy

[debugpy][4] is a debugger for Python that implements of the Debug Adapter Protocol for Python 3.
You can read more about it [here][5]

**Installation (all platforms)**

Installing `debugpy` is done using `pip`:

~~~bash
pip install --upgrade pip
pip install --upgrade debugpy
~~~

----

Once installed, go to:

- `Settings` &#8594; `Debug Adapter Client`
- Click on the magnifier button
- Click `OK`

If you followed those instructions, CodeLite should be able to locate `lldb-vscode` and configure it.

## The configuration dialog
---

The configuration dialog is minimal and contains the following fields:

| Property | Description |
|------------------|-------------|
| Command | the debug adapter server execution command line |
| Connection string | For example: `tcp://127.0.0.1:12345`|
| Use relative paths| CodeLite will pass and convert files paths into relative paths before sending them to the dap|
| Use forward slash | (Windows only) Always use forward `/` slash |
| Use volume | (Windows only) include the volume part of the file path |
| Server environment | Before launching the dap, apply these environment variables (in the `NAME=VALUE` format)|
| Environment format | The format for environment variables to provide to the server, which will pass them to the debuggee. The format might be `List` or `Dictionary`, depending on the dap. e.g.<br/>  `{ "FOO": "1", "BAR": "baz"} $launch_debuggee`<br/> or <br/> `FOO=1 BAR=baz $launch_debuggee`|

## Using dap instead of the built-in debuggers
---

### `C++` workspace
---

- Open the project settings.
- Select the `General` tab.
- Under the `Debugger` section, click on the drop-down control and change the debugger.

### File system workspace
---

- Open the workspace settings.
- Select the `General` tab.
- Choose a debugger from the `Debugger` drop-down control.


 [1]: https://microsoft.github.io/debug-adapter-protocol/
 [2]: https://microsoft.github.io/debug-adapter-protocol/implementors/adapters/
 [3]: /getting_started/windows/#common
 [4]: https://github.com/microsoft/debugpy
 [5]: https://github.com/microsoft/debugpy/blob/main/doc/Subprocess%20debugging.md
 [6]: /getting_started/linux/#optional-install-brew-for-linux
 [7]: /getting_started/macos/#install-brew
