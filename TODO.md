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

- Diff View: Open it inside a tab and not as a separate wxFrame.
- wxCrafter: we should be able to provide relative path for the generated files
- GDB: use the new terminal to launch and avoid opening custom terminals for this purpose.

Medium:
-----

- Implement `code_symbols` tool for AI.
- AI: Extend all the tools with a "Purpose" mandatory field, so each model will be forced to report the reason for the invocation.
- Terminal: when changing a terminal theme, apply it to newly opened terminals only.
- Terminal: remember, on per terminal type, the last theme used for it.

Low:
----

- build tab: add AI option to the context menu.
- Add progress bar table for LSP progress messages (currently we only print messages in the LOG view)
- MarkdownStyler: mark gcc style lines "file:line" as hyperlink and clickable.
- Add `codelite-api.lua` file to the installation folder to provide completion for LUA LSPs.
- MarkdownStyler: ensure that all types of checkmarks are coloured in green (same for `x` symbols)

