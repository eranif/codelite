High:
-----

- [x] Replace the "Find References" view with `wxDataViewListCtrl` with custom renderer.
- [x] Implement all function: change to use the native `wxDataViewListCtrl`
- [x] Remove the TabGroups from CodeLite.
- [x] While the review-buddy agent is working, the spinner should spin in the agent's tab
- [x] Find references: include line number if possible.
- [ ] BUG: Markdown renderer, fails to render "task" (empty checkbox) if it is followed by code tags (inline like `code`)
- [ ] BUG: Markdown renderer, renders "checked task" incorrectly (checkbox with "V" inside it) if it is followed by code tags (inline like `code`) - it consumes some of the codetag background

Medium:
-----

- [ ] AI: Extend all the tools with a "Purpose" mandatory field, so each model will be forced to report the reason for the invocation.
- [ ] Terminal: when changing a terminal theme, apply it to newly opened terminals only.
- [ ] Terminal: remember, on per terminal type, the last theme used for it.
- [ ] wxCrafter: we should be able to provide relative path for the generated files
- [ ] GDB: use the new terminal to launch and avoid opening custom terminals for this purpose.

Low
---

- [ ] support attach to process
- [ ] support debugging core
- [ ] git: support showing file revision history
- [ ] editor: add an api to add end of line annotations
- [ ] git: explore the option of using the new api of end of line annotations to replace the nav bar commit info ^^
- [ ] dap: support stdio connection based
- [ ] dap: support attach to process
- [ ] SFTP plugin: re-use the remote find in files dialog
- [ ] SFTP plugin: use clTerminalViewCtrl for the log view
- [ ] Spell Checker: enable it for the commit dialog
- [ ] build tab: add AI option to the context menu.
- [ ] Add progress bar table for LSP progress messages (currently we only print messages in the LOG view)
- [ ] MarkdownStyler: mark gcc style lines "file:line" as hyperlink and clickable.
- [ ] Add `codelite-api.lua` file to the installation folder to provide completion for LUA LSPs.
- [ ] MarkdownStyler: ensure that all types of checkmarks are coloured in green (same for `x` symbols)

