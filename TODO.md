Debug Adapter Client
---

- support attach to process
- support debugging core

others:
----

- git: support showing file revision history
- git: check before any operation if we have `.git/index.lock`
- editor: add an api to add end of line annotations
- git: explore the option of using the new api of end of line annotations to replace the nav bar commit info ^^
- dap: support stdio connection based
- dap: support attach to process
- SFTP plugin: re-use the remote find in files dialog
- SFTP plugin: use clTerminalViewCtrl for the log view
- Spell Checker: enable it for the commit dialog
- git reset: selection does not always consider all selected items.
- build tab: add AI option to the context menu.
- use json instead of cJSON everywhere

AI-powered IDE:
--------

- MarkdownStyler: mark gcc style lines "file:line" as hyperlink and clickable.
- Extend the LUA API to support more options:
    - codelite.get_prompt(name)
    - codelite.menu_add_separator()
    - codelite.is_workspace_opened()

- Extend the "Prompt Editor" to support adding/deleting prompts (the default ones can not be deleted)
- In the prompt editor, add support for placeholders:

    - `{{full_fullpath}}`
    - `{{file_ext}}`
    - `{{file_dir}}`
    - `{{file_content}}` - file content of a file at a given path
    - `{{workspace_path}}` - full path of the workspace
    - `{{workspace_config}}` - release / debug etc

Prepare release notes per version (e.g. 18.2.0, 18.3.0 etc)
