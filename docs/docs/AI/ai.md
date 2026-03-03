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

Open the chat box at any time with ++ctrl+shift+h++. The window can be used for casual questions, code-related queries,
or to instruct the model to perform tasks.

![Chat Box Interface](/assets/chat-box.png)

---

## 3. Built-in Model Tools

CodeLite exposes the following built-in tools for the model:

### File Operations
- **ReadFileContent** - Read file contents
  - `filepath` (required): Path to the file to read
  - `from_line` (optional): Starting line number (1-based)
  - `line_count` (optional): Number of lines to read

- **CreateNewFile** - Create a new file with optional content
  - `filepath` (required): Path where the new file should be created
  - `file_content` (optional): Initial content to write to the file

- **OpenFileInEditor** - Open a file for editing or viewing
  - `filepath` (required): Path of the file to open

- **ApplyPatch** - Apply a git-style diff patch to a file
  - `file_path` (required): Path to the file to patch
  - `patch_content` (required): Git-style diff patch content to apply

### Search
- **FindInFiles** - Search for a pattern within files in a directory
  - `root_folder` (required): Root directory where search begins
  - `find_what` (required): Text pattern to search for
  - `file_pattern` (required): File pattern to match (e.g., "*.txt", "*.py")
  - `case_sensitive` (optional): Enable case-sensitive matching (default: true)
  - `is_regex` (optional): Treat find_what as regex pattern (default: false)
  - `whole_word` (optional): Match only complete words (default: true)

### Git Operations
- **GetLogInRangeCommit** - Return git history between two commits
  - `start_commit` (required): First commit in the range
  - `end_commit` (required): Second commit in the range

### Workspace Management
- **CreateWorkspace** - Create a new workspace (local or remote)
  - `path` (required): Directory path where workspace should be created
  - `name` (optional): Name of the workspace to create
  - `host` (optional): SSH host for creating a remote workspace

### Editor Information
- **GetActiveEditorFilePath** - Get the file path of the currently active editor (no arguments)

- **GetActiveEditorText** - Get the text content of the active editor tab (no arguments)

### System Information
- **GetOS** - Get the current active operating system (no arguments)

### Build & Compilation
- **ReadCompilerOutput** - Read the most recent build command's compiler output (no arguments)

### Shell Execution
- **ShellExecute** - Execute a shell command
  - `command` (required): The shell command to execute

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

CodeLite provides a comprehensive set of placeholders that can be utilized within prompts. When you type `{{` in the chat box,
a completion menu will appear displaying all available placeholders.

**Supported placeholders:**

- `{{current_selection}}` – The currently selected text in the editor
- `{{current_file_fullpath}}` – Full path of the current file
- `{{current_file_ext}}` – File extension of the current file
- `{{current_file_dir}}` – Directory containing the current file
- `{{current_file_name}}` – Name of the current file
- `{{current_file_lang}}` – Programming language of the current file
- `{{current_file_content}}` – Complete content of the current file

---

## 6. CodeLite Prompt Store

### Overview

CodeLite provides a **Prompt Store** feature that enables users to write and store prompts for future use. This
functionality facilitates efficient prompt management and customization within the development environment.

### Prompt Editor

The **Prompt Editor** allows you to:

- Tweak the system prompt for each operation
- Add custom prompts
- Create entirely new AI actions

These prompts can be used from the Chat-Box "Options" drop down menu.

---

## 7. AI-Powered IDE Features

### Git Commit Message

![Git Commit Message Generation](/plugins/images/git.gif)

One click generates a full commit message from the current diff.

### Automatic Function Documentation

![Automatic Function Documentation](/plugins/images/auto-doc.gif)

Place the cursor inside a function, press ++ctrl+shift+m++, and the model writes a complete docstring.

---

## 8. Getting Help

- Open the chat box (++ctrl+shift+h++) and ask any question or ask the model to perform tasks for you.
- For endpoint-specific issues, use **AI → Settings** to view or edit the stored URLs and tokens.

---

**Enjoy a smarter, faster coding experience with ChatAI in CodeLite**
