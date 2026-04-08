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

High:
-----

Medium:
-----

- Support auto reading `AGENTS.md` file and convert it into System-Messages.
- Implement `code_symbols` tool for AI.
- Add tool management configuration which allows users to trust tools permanently
- AI: Extend all the tools with a "Purpose" mandatory field, so each model will be forced to report the reason for the invocation.
- AI: Add a system-message that instruct the model to NEVER create a summary file
- Terminal: when optimizing for drawing, make sure that exclude cells with default attributes only.
- Terminal: when changing a terminal theme, apply it to newly opened terminals only.
- Terminal: remember, on per terminal type, the last theme used for it.

Low:
----

- build tab: add AI option to the context menu.
- Add progress bar table for LSP progress messages (currently we only print messages in the LOG view)
- MarkdownStyler: mark gcc style lines "file:line" as hyperlink and clickable.
- Add `codelite-api.lua` file to the installation folder to provide completion for LUA LSPs.
- MarkdownStyler: ensure that all types of checkmarks are coloured in green (same for `x` symbols)

BUGS
----

- Application hangs when renaming a symbol & the `MiniMap` editor is loaded...
