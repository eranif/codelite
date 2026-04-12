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

- When showing the output pane, move the focus to the active tab
- Change the "Find-In-Files" to use the following strategy:

### 1. Implementation of "Summary Mode" (The Most Impactful)
Instead of returning all matches by default, change the tool's output strategy based on the number of results.

*   **The Threshold Trigger:** If the number of matches exceeds a certain limit (e.g., 10 matches), the tool should **stop** returning the actual line content.
*   **The Summary Response:** Instead of the text, return a summarized list:
    *   `file_path`: The path to the file.
    *   `match_count`: Number of matches in that file.
    *   `line_numbers`: A list of line numbers where matches occurred.
*   **The LLM Loop:** This forces the LLM to see the "map" of where the code is first, and then explicitly call `ReadFileContent` for the specific files/lines it actually cares about.

### 2. Response Formatting (Structure over Prose)
Avoid verbose natural language in the tool's output. Use a compact format (JSON or a tight TSV-like string) that the LLM can parse efficiently.

**Bad (Verbose):**
`Match found in src/main.py at line 42: "def calculate_total(price, tax):"`

**Better (Compact JSON):**
```json
[{"f": "src/main.py", "l": 42, "t": "def calculate_total(price, tax):"}]
```

*Using short keys (`f` for file, `l` for line, `t` for text) significantly reduces the input token count for the LLM when processing large result sets.*

Medium:
-----

- Support auto reading `AGENTS.md` file and convert it into System-Messages.
- Implement `code_symbols` tool for AI.
- AI: Extend all the tools with a "Purpose" mandatory field, so each model will be forced to report the reason for the invocation.
- AI: Add a system-message that instruct the model to NEVER create a summary file
- AI: Add token usage
- AI: Add "compact" or "summary" button to when the AI is losing it and needs a fresh start :)
- Terminal: when changing a terminal theme, apply it to newly opened terminals only.
- Terminal: remember, on per terminal type, the last theme used for it.
- Extend the single agent and add sub-agents that will offload heavy tasks, such as code browsing and reading large files.

An example of such tool:

```json
{
  "name": "query_code_browser",
  "description": "Ask the specialized Code-Browser agent to find, read, or analyze source code files in its isolated sandbox.",
  "inputSchema": {
    "type": "object",
    "properties": {
      "query": {
        "type": "string",
        "description": "The specific question or search command (e.g., 'Find where the global mutex is locked in editor.cpp')."
      },
      "files": {
        "type": "array",
        "items": { "type": "string" },
        "description": "Optional list of specific files to focus on."
      }
    },
    "required": ["query"]
  }
}
```

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
