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
