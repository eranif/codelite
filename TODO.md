Debug Adapter Client
---

- support attach to process
- support debugging core

others:
----

- git: support showing file revision history
- editor: add an api to add end of line annotations
- git: explore the option of using the new api of end of line annotations to replace the nav bar commit info ^^
- dap: support stdio connection based
- dap: support attach to process
- SFTP plugin: re-use the remote find in files dialog
- SFTP plugin: use clTerminalViewCtrl for the log view
- Spell Checker: enable it for the commit dialog
- build tab: add AI option to the context menu.
- use json instead of cJSON everywhere

AI-powered IDE:
--------

- MarkdownStyler: mark gcc style lines "file:line" as hyperlink and clickable.
- Add `codelite-api.lua` file to the installation folder to provide completion for LUA LSPs.
- Make another tool `RemoteFindInFile` for supporting remote find-in-files.
- Replace the standard "find-in-files" tool with `grep` command (on all platforms) - this will allow it to run on remote files.
- Add system message telling the model on which OS it is running.

Prepare release notes per version (e.g. 18.2.0, 18.3.0 etc)

BUGS
----

Application hangs when renaming a symbol & the `MiniMap` editor is loaded...
