# ChatAI – Your Local AI Assistant

**Since CodeLite 18.2.0**

---

## Overview

CodeLite 18.2.0 ships with a built-in chat interface that connects to any language model you configure – locally (via an **Ollama** server) or remotely (e.g., Anthropic **Claude**). Adding new endpoints is now a guided wizard process, and all interactions are performed through a clean, toolbar-driven UI.

---

## 1. Add an LLM Endpoint

- Open **AI → Add New Endpoint** from the main menu.

   ![Menu – Add New Endpoint](/assets/menu-new-endpoint.png)

- Follow the wizard: choose a name, select the provider (Ollama, Claude, etc.), enter the URL and any required authentication, then click **Finish**.

   ![Wizard – Step 1](/assets/add-new-endpoint-1.png)
   ![Wizard – Step 2](/assets/add-new-endpoint-2.png)

- Test the endpoint: press ++ctrl+shift+h++ to open the chat box and send a short prompt (e.g., "Hello").

---

## 2. The Chat Box

Open the chat box at any time with ++ctrl+shift+h++. The window can be used for casual questions, code-related queries, or to instruct the model to read/write files.

![Chat Box Interface](/assets/chat-box.png)

### Toolbar (left → right)

| Icon | Action | Shortcut |
|------|--------|----------|
| **Clear** | Erases the chat history **and** the internal message log. | – |
| **Endpoint ▼** | Switches between the endpoints you added. | – |
| **Placeholders ▼** | Insert a [placeholder](#4-placeholders) into the prompt. | – |
| **Enable Tools** | Enable/disable the [built-in model tools](#3-built-in-model-tools). | – |
| **Play** | Sends the current input to the model. | ++shift+enter++ |
| **Stop** | Cancels a long-running request. | – |
| **Restart** | Restarts the LLM client (clears all history). | – |
| **Replay** | Opens a dialog that lists recent user messages – you can resend any of them. | – |
| **Link** | When enabled, the output pane auto-scrolls so the last line is always visible. | – |
| **Detach/Attach** | Pops the chat window out of the dock (floating) or puts it back. | – |

---

## 3. Built-in Model Tools

CodeLite exposes the following built-in tools for the model:

* **ApplyPatch** — Apply a git style diff patch to a file.
  * `file_path` (string, required): The path to the file that should be patched.
  * `patch_content` (string, required): The git style diff patch content to apply.
  * `dry_run` (boolean, optional): When true, validates the patch without modifying the file. Default is true.

* **CreateNewFile** — Create a new file at the specified path with optional content.
  * `filepath` (string, required): The path where the new file should be created.
  * `file_content` (string, optional): The initial content to write to the file.

* **CreateWorkspace** — Create a new workspace at the given path with the provided name. If a host is specified, it creates a remote workspace using SSH/SFTP; otherwise, it creates a local filesystem workspace.
  * `path` (string, required): The directory path where the workspace should be created.
  * `name` (string, optional): The name of the workspace to create.
  * `host` (string, optional): The SSH host for creating a remote workspace.

* **FindInFiles** — Search for a given pattern within files in a directory.
  * `find_what` (string, required): The text pattern to search for (literal string or regex pattern).
  * `file_pattern` (string, required): The file pattern to match, such as "*.txt" or "*.py". Use semi-colon list to pass multiple patterns.
  * `root_folder` (string, required): The root directory where the search begins.
  * `case_sensitive` (boolean, optional): When enabled, performs case-sensitive matching. Default is true.
  * `is_regex` (boolean, optional): When enabled, treats find_what as a regular expression pattern. Default is false.
  * `whole_word` (boolean, optional): When enabled, matches only complete words. Default is true.

* **GetActiveEditorFilePath** — Retrieves the file path of the currently active editor.
  * *(No arguments)*

* **GetActiveEditorText** — Return the text of the active tab inside the editor.
  * *(No arguments)*

* **GetLogInRangeCommit** — Return git history of commits between range of commits.
  * `start_commit` (string, required): The first commit in the range.
  * `end_commit` (string, required): The second commit in the range.

* **OpenFileInEditor** — Try to open file 'filepath' and load it into the editor for editing or viewing.
  * `filepath` (string, required): The path of the file to open inside the editor.

* **ReadCompilerOutput** — Reads and fetches the compiler build log output of the most recent build command executed by the user.
  * *(No arguments)*

* **ReadFileContent** — Reads the entire content of the file 'filepath' from the disk.
  * `filepath` (string, required): The path of the file to read.

* **ShellExecute** — Execute a shell command and return its output.
  * `command` (string, required): The shell command to execute.

### Quick Example

You get a confusing compile error. Type:

```
Explain the build errors and suggest fixes.
```

The model will automatically call `ReadCompilerOutput`, fetch the log, and then reply with a human-readable explanation and concrete fixes.

---

## 4. External MCP Servers

In addition to the built-in tools, CodeLite supports external MCP (Model Context Protocol) servers. You can integrate two types of external servers:

- **SSE (Server-Sent Events) over HTTPS** — For remote server connections
- **Local MCP Server (over STDIO)** — For locally running servers

### Adding External MCP Servers

To add an external MCP server, navigate to the menu bar and select one of the following options:

| Server Type | Menu Path |
|-------------|-----------|
| Local MCP Server | `AI` → `Add New Local MCP Server` |
| SSE MCP Server | `AI` → `Add New SSE MCP Server` |

---

## 5. Placeholders

CodeLite offers a list of placeholders that can be used in prompts. This is useful when you want to create generic, reusable prompts.

**Supported placeholders:**

- `{{current_selection}}` – The currently selected text in the editor
- `{{current_file_fullpath}}` – Full path of the current file
- `{{current_file_ext}}` – File extension of the current file
- `{{current_file_dir}}` – Directory containing the current file
- `{{current_file_name}}` – Name of the current file
- `{{current_file_lang}}` – Programming language of the current file
- `{{current_file_content}}` – Complete content of the current file

!!! Note
    Typing `{{` in the chat input will display a completion list.

---

## 6. Prompt Editor

Several AI-powered actions are available with a single click. The prompts that drive those actions can be edited from **AI → Open Prompt Editor**.

| Action | Shortcut/UI | Description |
|--------|-------------|-------------|
| **Generate Git Commit Message** | Click the toolbar button or use the context-menu entry. | Summarizes the staged changes into a concise, conventional commit message. |
| **Generate Git Release Notes** | Press the "Release Notes" button in the Git view. | Produces release notes for a selected commit range. |
| **Code Review** | Right-click a file → **AI-powered code generation → Review**. | Returns a comprehensive review with suggestions and identified issues. |
| **Generate Docstring** | ++ctrl+shift+m++ (or right-click → **Generate docstring for the current method**). | Inserts a language-appropriate docstring for the function/class under the cursor. |

The Prompt Editor lets you tweak the system prompt for each of these operations, add custom placeholders, or create entirely new AI actions.

---

## 7. AI-Powered IDE Features

### Git Commit Message

![Git Commit Message Generation](/plugins/images/git.gif)

One click generates a full commit message from the current diff.

### Git Release Notes

![Git Release Notes Generation](/plugins/images/release-notes.gif)

Select a commit range → **Generate Release Notes** → AI produces a polished changelog.

### Automatic Function Documentation

![Automatic Function Documentation](/plugins/images/auto-doc.gif)

Place the cursor inside a function, press ++ctrl+shift+m++, and the model writes a complete docstring.

---

## 8. Getting Help

- Open the chat box (++ctrl+shift+h++) and ask any question.
- For endpoint-specific issues, use **AI → Settings** to view or edit the stored URLs and tokens.

---

**Enjoy a smarter, faster coding experience with ChatAI in CodeLite 18.2.0!**