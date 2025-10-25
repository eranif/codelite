# ChatAI: Your Local AI Assistant
---

With the release of **CodeLite 18.2.0**, CodeLite introduces embeds a chat interface powered by a local or remote language model of your choice. We currently support local models through an **Ollama** server or Anthropic's **Claude**. More to be introduced.

## Getting Started
---

### 1. Add LLM endpoint
---

- From the main menu, go to `AI` -> `Add New Endpoint`
 ![](/assets/menu-new-endpoint.png)

- Fill the wizard fields and click `Finish`
![](/assets/add-new-endpoint-1.png)
![](/assets/add-new-endpoint-2.png)

- To test the newly added endpoint, click ++ctrl+shift+h++ and try the chat box.

### 2. The Chat Box

- CodeLite offers a built-in chat box by clicking ++ctrl+shift+h++.
- The chat can be used for asking questions and also for instructing the AI to perform actions (like reading / writing files)

![](/assets/chat-box.png)

Tool-bar buttons, in-order of appearance:

1. "clear" - clears the chat history, this not only clears the chat box text, but also erases the history.
2. endpoint selection dropdown box - allows you to choose between the different endpoints.
3. "restart" - restarts the client, all history is lost.
4. "play" - submit the text to the LLM. (You can also use ++shift+enter++)
5. "stop" - cancel a long running operation.
6. "replay" - show a dialog with recent messages sent to the LLM.
7. "link" - when checked, the LLM output is aut-scrolled (i.e. last line is always visible).
8. "detach" / "attach" - detach (when docked) the chat window into a floating window. When floated, the button becomes "attach".

CodeLite exposes the following tools to the models (NOTE: the model must have the "tools" capability in order to use them):

| Tool Name | Description |
| --- | --- |
| Get_the_text_of_the_active_tab_inside_the_editor | This tool retrieves the text content of the currently active tab in the editor. It's useful when you want to view or manipulate the content of the file you're currently working on. |
| Open_a_file_in_an_editor | This tool allows you to open a specific file in the editor for viewing or editing. You provide the path to the file, and it will be loaded into the editor. |
| Read_file_from_the_file_system | This tool reads the entire content of a specified file from the disk and returns it. It's helpful when you need to examine or process the contents of a file without opening it in an editor. |
| Read_the_compiler_build_output | This tool fetches the build log output from the most recent compiler build command executed by the user. It's useful for diagnosing and resolving build issues by providing detailed information about what happened during the build process. |
| Write_file_content_to_disk_at_a_given_path | This tool writes a given content to a file at a specified path on the disk. It's useful for creating or updating files programmatically. |
| git_commit_log_history_between_two_commits | This tool retrieves the git commit history between two specified commits. You provide the start and end commit identifiers, and it returns the log of changes between those points in the repository. |

With these tools, the model can assist you in various ways related to file management, code editing, build processes, and version control:

1. **File Management**:
    - Read or write files on the disk.
    - Open files in an editor for viewing or editing.
    - Retrieve the content of the active tab in the editor.

2. **Code Editing**:
    - View or modify the content of files directly.
    - Work with code in an interactive environment.

3. **Build Process Assistance**:
    - Read compiler build output to help diagnose and resolve build issues.
    - Understand what went wrong during a build process.

4. **Version Control**:
    - Retrieve git commit history between two commits to understand changes made in the repository.

These tools allow the model to support tasks like editing code, reviewing build logs, examining file contents, and tracking changes in a version-controlled environment.

**Example use case:**

You compiled a C++ program and got a compiler error that you don't understand. Typing this in the prompt:

```text
Explain the build errors I got from the compiler and suggest ways to fix them.
```

The model will invoke the tool `Read_the_compiler_build_output` to fetch the build output and then provide an explanation.


### 3. The Prompt Editor

CodeLite offers several AI-Powered operations that can be done with a single click:

- Git commit message - With a single click, the AI will generate a detailed git commit message involving your recent changes.
- Git Release Notes - Let the AI generate a release notes based on git commit range.
- Code Review - Improve your commits by letting the AI offers a comperhensive code review.
- Function / Classes auto documentation - use the AI to document your code, place your anywhere in the code and hit ++ctrl+shift+m++

The prompt for these operations can be edited from the main menu: `AI` -> `Open Prompt Editor`.

### 4. Integration inside the IDE
---

Beyond the chat interface and action triggering capabilities within the IDE through tools, CodeLite also incorporates AI-powered features:

#### Git Commit

CodeLite now provides an automated, AI-driven commit message feature. As demonstrated in the animated GIF below:

<div align="center">
  <img src="/plugins/images/git.gif"/>
</div>

#### Git Release Notes

Automating release note creation is typically a laborious and repetitive task. Leveraging AI to handle this process can significantly save time and effort. This can now be accomplished with just a few clicks.

1. Select the desired commit range.
2. Press the "Generate Release Notes" button.
3. Enter the first and second commit identifiers.
4. Click "Ok".

This streamlined approach makes it easy to produce comprehensive release notes without manual input.

<div align="center">
  <img src="/plugins/images/release-notes.gif"/>
</div>

#### Automated Function Documentation with AI

Code documentation has never been simpler.

1. Position your cursor within the desired function
2. Press ++ctrl+shift+m++ or access the context menu by right-clicking and selecting `AI-powered code generation` → `Generate docstring for the current method`.

This streamlined process allows developers to quickly generate comprehensive docstrings using artificial intelligence assistance, enhancing code readability and maintainability.

<div align="center">
  <img src="/plugins/images/auto-doc.gif"/>
</div>

**assistant**:
# ChatAI – Your Local AI Assistant
**CodeLite 18.2.0**

---

## Overview

CodeLite 18.2.0 ships with **ChatAI**, a built‑in chat interface that talks to any language‑model you configure – locally (via an **Ollama** server) or remotely (e.g. Anthropic **Claude**).  Adding new endpoints is now a guided wizard, and all interactions are performed through a clean, toolbar‑driven UI.

---

## 1. Add an LLM Endpoint

1. Open **AI → Add New Endpoint** from the main menu.

   ![Menu – Add New Endpoint](/assets/menu-new-endpoint.png)

2. Follow the wizard: choose a name, select the provider (Ollama, Claude, …), enter the URL and any required authentication, then click **Finish**.

   ![Wizard – Step 1](/assets/add-new-endpoint-1.png)
   ![Wizard – Step 2](/assets/add-new-endpoint-2.png)

3. Test the endpoint: press **Ctrl + Shift + H** to open the chat box and send a short prompt (e.g. “Hello”).

---

## 2. The Chat Box

Open the chat box at any time with **Ctrl + Shift + H**.
The window can be used for casual questions, code‑related queries, or to instruct the model to read/write files.

![](./assets/chat-box.png)

### Toolbar (left → right)

| Icon | Action | Shortcut |
|------|--------|----------|
| **Clear** | Erases the chat history **and** the internal message log. | – |
| **Endpoint ▼** | Switches between the endpoints you added. | – |
| **Restart** | Restarts the LLM client (clears all history). | – |
| **Play** | Sends the current input to the model. <br>*Alternative:* **Shift + Enter** | – |
| **Stop** | Cancels a long‑running request. | – |
| **Replay** | Opens a dialog that lists recent user messages – you can resend any of them. | – |
| **Link** | When enabled, the output pane auto‑scrolls so the last line is always visible. | – |
| **Detach / Attach** | Pops the chat window out of the dock (floating) or puts it back. | – |

---

## 3. Built‑in Model Tools

If the selected model advertises the **tools** capability, ChatAI can invoke the following helpers.  The model decides when (and how) to call them.

| Tool | What it does |
|------|--------------|
| **Get_the_text_of_the_active_tab_inside_the_editor** | Returns the full source text of the currently focused editor tab. |
| **Open_a_file_in_an_editor** | Opens a given file path in CodeLite for viewing or editing. |
| **Read_file_from_the_file_system** | Reads a file from disk and returns its contents. |
| **Read_the_compiler_build_output** | Retrieves the log of the most recent build performed inside CodeLite. |
| **Write_file_content_to_disk_at_a_given_path** | Writes supplied text to a file at the specified location. |
| **git_commit_log_history_between_two_commits** | Returns the git log between two commit IDs (or refs). |

These tools enable the model to help with **file management**, **code editing**, **build‑log analysis**, and **version‑control queries** without you having to copy‑paste anything.

### Quick example

You get a confusing compile error.  Type:

```
Explain the build errors and suggest fixes.
```

The model will automatically call **Read_the_compiler_build_output**, fetch the log, and then reply with a human‑readable explanation and concrete fixes.

---

## 4. Prompt Editor

Several AI‑powered actions are available with a single click.  The prompts that drive those actions can be edited from **AI → Open Prompt Editor**.

| Action | Shortcut / UI | What it does |
|--------|----------------|--------------|
| **Generate Git Commit Message** | Click the toolbar button or use the context‑menu entry. | Summarises the staged changes into a concise, conventional commit message. |
| **Generate Git Release Notes** | Press the “Release Notes” button in the Git view. | Produces release notes for a selected commit range. |
| **Code Review** | Right‑click a file → **AI‑powered code generation → Review**. | Returns a comprehensive review with suggestions and identified issues. |
| **Generate Docstring** | **Ctrl + Shift + M** (or right‑click → **Generate docstring for the current method**). | Inserts a language‑appropriate docstring for the function/class under the cursor. |

The Prompt Editor lets you tweak the system prompt for each of these operations, add custom placeholders, or create entirely new AI actions.

---

## 5. AI‑Powered IDE Features

### Git Commit Message
![](./plugins/images/git.gif)
One click generates a full commit message from the current diff.

### Git Release Notes
![](./plugins/images/release-notes.gif)
Select a commit range → **Generate Release Notes** → AI produces a polished changelog.

### Automatic Function Documentation
![](./plugins/images/auto-doc.gif)
Place the cursor inside a function, press **Ctrl + Shift + M**, and the model writes a complete docstring.

---

## 6. Getting Help

- Open the chat box (**Ctrl + Shift + H**) and ask any question.
- For endpoint‑specific issues, use **AI → Settings** to view or edit the stored URLs and tokens.

---

**Enjoy a smarter, faster coding experience with ChatAI in CodeLite 18.2.0!**