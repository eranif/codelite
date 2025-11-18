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
- Extend the LUA API to support more options:
    - codelite.get_prompt(name)
    - codelite.is_workspace_opened()
    - codelite.file_explorer_selected_item() - return the selected item in the file explorer view

    
- Extend the "Prompt Editor" to support adding/deleting prompts (the default ones can not be deleted)
- Document the prompt placeholders in `ai.md`
- Add `codelite-api.lua` file to the installation folder to provide completion for LUA LSPs.
- Add "Wizard" for adding new MCP server.
- Support images (assistant library).
- Chat bar: add option to disable the tools (check box).
- Git commit message: generate diff only for the selected items.


Prepare release notes per version (e.g. 18.2.0, 18.3.0 etc)
