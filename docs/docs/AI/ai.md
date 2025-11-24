# ChatAI – Your Local AI Assistant

**Since CodeLite 18.2.0**

---

## Overview

CodeLite 18.2.0 ships with **ChatAI**, a built-in chat interface that connects to any language model you configure – locally (via an **Ollama** server) or remotely (e.g., Anthropic **Claude**). Adding new endpoints is now a guided wizard process, and all interactions are performed through a clean, toolbar-driven UI.

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

If the selected model advertises the **tools** capability, ChatAI can invoke the following helpers. The model decides when (and how) to call them.

- **`Create_new_workspace`**
    - **Description:** Creates a new workspace at a given path with a specified name. If a host is specified, it creates a remote workspace using SSH/SFTP; otherwise, it creates a local filesystem workspace.
    - **Required parameters:**
        - `path` (string) - The directory path where the workspace should be created
    - **Optional parameters:**
        - `name` (string) - The name of the workspace to create
        - `host` (string) - The SSH host for creating a remote workspace

- **`Get_the_text_of_the_active_tab_inside_the_editor`**
    - **Description:** Returns the text content of the currently active tab inside the editor.
    - **No parameters required**

- **`Lists_all_files_in_a_directory`**
    - **Description:** Lists all files in a specified directory matching a given pattern. This function scans a directory non-recursively for files matching the provided pattern and returns their full paths as a JSON array.
    - **Required parameters:**
        - `dir` (string) - The directory path to scan (can be relative or absolute)
        - `pattern` (string) - File pattern to match (e.g., "*.cpp", "*.txt")
    - **No optional parameters**

- **`Lists_all_subdirectories`**
    - **Description:** Lists all subdirectories within a specified directory path (non-recursive). This function scans the given directory and returns a JSON array containing the full paths of all immediate subdirectories. The scan is non-recursive and filters results to include only directories, excluding files. Empty path is allowed.
    - **Required parameters:**
        - `path` (string) - The directory path to scan
    - **No optional parameters**

- **Open_a_file_in_an_editor**
    - **Description:** Opens a file at the specified filepath and loads it into the editor for editing or viewing.
    - **Required parameters:**
        - `filepath` (string) - The path of the file to open inside the editor
    - **No optional parameters**

- **`Read_file_from_the_file_system`**
    - **Description:** Reads the entire content of the file from disk. On success, this function returns the entire file's content.
    - **Required parameters:**
        - `filepath` (string) - The path of the file to read
    - **No optional parameters**

- **`Write_file_content_to_disk_at_a_given_path`**
    - **Description:** Writes the provided content to the file system at the given filepath.
    - **Required parameters:**
        - `filepath` (string) - The path file path
        - `file_content` (string) - The content of the file to be written to the disk
    - **No optional parameters**

- **`Read_the_compiler_build_output`**
    - **Description:** Reads and fetches the compiler build log output of the most recent build command executed by the user. This is useful for helping explain and resolve build issues. On success, returns the complete build log output.
    - **No parameters required**


These tools enable the model to help with **file management**, **code editing**, **build-log analysis**, and **version-control queries** without you having to copy-paste anything.

### Quick Example

You get a confusing compile error. Type:

```
Explain the build errors and suggest fixes.
```

The model will automatically call `Read_the_compiler_build_output`, fetch the log, and then reply with a human-readable explanation and concrete fixes.

---

## 4. Placeholders

CodeLite offers a list of placeholders that can be used in prompts. This is useful when you want to create generic, reusable prompts.

**Supported placeholders:**

- `{{current_selection}}` – The currently selected text in the editor
- `{{current_file_fullpath}}` – Full path of the current file
- `{{current_file_ext}}` – File extension of the current file
- `{{current_file_dir}}` – Directory containing the current file
- `{{current_file_name}}` – Name of the current file
- `{{current_file_lang}}` – Programming language of the current file
- `{{current_file_content}}` – Complete content of the current file

---

## 5. Prompt Editor

Several AI-powered actions are available with a single click. The prompts that drive those actions can be edited from **AI → Open Prompt Editor**.

| Action | Shortcut/UI | Description |
|--------|-------------|-------------|
| **Generate Git Commit Message** | Click the toolbar button or use the context-menu entry. | Summarizes the staged changes into a concise, conventional commit message. |
| **Generate Git Release Notes** | Press the "Release Notes" button in the Git view. | Produces release notes for a selected commit range. |
| **Code Review** | Right-click a file → **AI-powered code generation → Review**. | Returns a comprehensive review with suggestions and identified issues. |
| **Generate Docstring** | ++ctrl+shift+m++ (or right-click → **Generate docstring for the current method**). | Inserts a language-appropriate docstring for the function/class under the cursor. |

The Prompt Editor lets you tweak the system prompt for each of these operations, add custom placeholders, or create entirely new AI actions.

---

## 6. AI-Powered IDE Features

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

## 7. Getting Help

- Open the chat box (++ctrl+shift+h++) and ask any question.
- For endpoint-specific issues, use **AI → Settings** to view or edit the stored URLs and tokens.

---

**Enjoy a smarter, faster coding experience with ChatAI in CodeLite 18.2.0!**