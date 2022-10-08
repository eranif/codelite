# Source Code Formatter Plugin
---

## General
---

The source code formatter plugin integrates various code formatters (aka beautifiers) tools into CodeLite
Once integrated, you can format your code using a single key stroke, by default it is set to ++ctrl+i++

## Tool configurations
---

Each supported tool contains the following details:

- `Enabled` flag
- `Inplace edit`
- `Working directory`
- `Command`

### `Enabled`

This is a self explanatory - is this formatter enabled or disabled?

### `Inplace edit`

Some formatters do not print the formatted output to `stdout` but instead are editing the source file directly.
This check box marks a formatter as such. It is needed internally by CodeLite

### `Working directory`

CodeLite executes the formatter from this directory. If not provided, CodeLite uses the workspace root folder
It is recommended to set the working directory to `$(WorkspacePath)`

### `Command`

The command to execute. Macros are allowed here
You may place each command on a separate line or separated by space. Lines starting with `#` are considered as comments and are ignored.
Empty lines are also ignored

## Integration with remote workspace
---

Once a remote workspace is loaded (e.g. via [Remoty][1] plugin), the plugin replaces the commands defined in the UI
with the commands taken from [`codelite-remote.json`][2] configuration file.


 [1]: /plugins/remoty
 [2]: /plugins/remoty/#remote-configuration-codelite-remotejson

