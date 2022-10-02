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
- `Inline editing`
- `Working directory`
- `Local command`
- `Remote command`

## `Enabled`

This is a self explanatory - is this formatter enabled or disabled?

## `Inline editing`

Some formatters do not print the formatted output to `stdout` but rather are updating the source file directly.
This check box marks a formatter as such. It is needed internally by CodeLite

## `Working directory`

CodeLite executes the formatter from this directory. If not provided, CodeLite uses the workspace root folder

## `Local command`

When working on a local workspace (i.e. a workspace that exists on your local machine), CodeLite executes the command
set in this field

## `Remote command`

When working on a remote workspace (i.e. a workspace that exists on a different machine using [Remoty][1]), CodeLite executes the command
set in this field


!!! Note
    For both the `Local command` & `Remote command` CodeLite ignores lines that:
    (1) Are starting with the `#` sign, (2) Are empty



 [1]: /plugins/remoty

