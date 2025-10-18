# ChatAI: Your Local AI Assistant
---

With the release of **CodeLite 18.3.0**, a brand-new plugin has been introduced that embeds a chat interface powered by a local or remote language model of your choice.
We currently support local models through an **Ollama** server or Anthropic's **Claude**.

## Getting Started
---

### 1. Install the Ollama Server & Pull a Model
---

1. Visit the [Ollama download page](https://ollama.com/download) and follow the instructions for your operating system.
2. Once installed, you can interact with Ollama from the command line using the `ollama` command.
3. Pull a model that fits your hardware. Recommended options:

    - `qwen3-coder:30b` – requires ~26 GB of RAM.
    - `gpt-oss:20b` – excellent performance, needs ~16 GB of RAM.

   If your machine cannot handle these, try smaller models.

   **Example commands**

   ```bash
   # Download a model
   ollama pull qwen3-coder:30b

   # List downloaded models
   ollama ls

   # Show memory usage
   ollama ps
   ```

### 2. Configure the Plugin
---

Once the server is running, open the **AI** settings (a JSON file) from the menu bar:
`AI -> Settings...`

A typical configuration file looks like this:

```json
{
  "history_size": 50,
  "mcp_servers": {},
  "log_level": "info",
  "stream": true,
  "keep_alive": "24h",
  "server_timeout": {
      "connect_msecs": 500,
      "read_msecs": 300000,
      "write_msecs": 300000
  },
  "endpoints": {
    "http://127.0.0.1:11434": {
      "active": true,
      "http_headers": {
        "Host": "127.0.0.1"
      },
      "type": "ollama"
    }
  },
  "models": {
    "default": {
      "options": {
        "num_ctx": 16384,
        "temperature": 0
      },
      "think_end_tag": "",
      "think_start_tag": ""
    }
  }
}
```

Save the file to apply any changes; the changes are applied immediately.

---

Below is a breakdown of the JSON configuration:

#### Global Settings

- **`history_size`**
    - Integer (`50`).
    - Limits how many past interactions (e.g., chat turns) are kept in memory for context.

- **`mcp_servers`**
    - Empty object (`{}`).
    - Placeholder for optional “MCP” (Model Context Protocol) server definitions.

- **`log_level`**
    - String (`"info"`).
    - Controls verbosity of logs; common levels include `debug`, `info`, `warn`, `error`.

- **`stream`**
    - Boolean (`true`).
    - Enables streaming of responses (e.g., partial text output) rather than waiting for the entire result.

- **`keep_alive`**
    - String (`"24h"`).
    - Sets the duration the server should keep idle connections alive before closing them.

- **`server_timeout`**
    - Object containing millisecond timeouts for different phases of a request:
    - `connect_msecs`: `500` – time allowed to establish a TCP connection.
    - `read_msecs`: `300000` – time allowed for reading a response.
    - `write_msecs`: `300000` – time allowed for sending a request.

---

#### Endpoints

- **`endpoints`**
    - Object mapping endpoint URLs to their configuration.
    - Example entry:
    - **URL**: `http://127.0.0.1:11434`
    - `active`: `true` – the endpoint is enabled.
    - `http_headers`: `{ "Host": "127.0.0.1" }` – custom HTTP headers to send with each request.
    - `type`: `"ollama"` – indicates the backend or protocol (here, an Ollama model server).

---

#### Models

- **`models`**
  - Object defining default or named model configurations.
  - Example entry:
    - **Model name**: `default`
      - `options`:
        - `num_ctx`: `16384` – maximum number of tokens in the context window.
        - `temperature`: `0` – deterministic generation (no randomness).
      - `think_start_tag`: `""` – marker that signals the start of a “think” block in the model’s output.
      - `think_end_tag`: `""` – marker that signals the end of a “think” block.

---

#### How It All Connects

- The server listens on the defined endpoints (here, a local Ollama instance).
- When a connection is established, it uses the `server_timeout` values to manage connection timing.
- The `stream` flag tells the server to send partial responses as they become available.
- The `history_size` limits how much conversation history is stored for context.
- The `models.default` block supplies default inference parameters (context size, temperature) and tags that the model may use to structure its output. This can be duplicated with a specific model name.
- Logging verbosity is set to `info` (other values: `debug`, `trace`, `error`, and `warn`).
- A model is kept in-memory for `24 hours` before being removed from the server memory.

This configuration allows the service to operate with a single local endpoint, deterministic inference, and controlled resource usage.

### 3. Integration with CodeLite via "Tools"
---

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

These tools allow me to support tasks like editing code, reviewing build logs, examining file contents, and tracking changes in a version-controlled environment.

**Example use case:**

You compiled a C++ program and got a compiler error that you don’t understand. Typing this in the prompt:

```text
Explain the build errors I got from the compiler and suggest ways to fix them.
```

The model will invoke the tool `Read_the_compiler_build_output` to fetch the build output and then provide an explanation.

### 4. Using Claude
---

CodeLite's AI support also enables the use of Anthropic's Claude LLM.

#### Prerequisites

- You need an account or can register at [Anthropic's website](https://www.anthropic.com/).
- Generate an API key.
- Ensure you have sufficient credits.

#### Configuration

- Open the AI configuration file (from the menu bar: `AI` → `Settings...`) or by clicking (++ctrl+shift+h++) / (++cmd+shift+h++), and click on the wrench (🔧) button.

- In the `endpoints` section, add the following:

```json
    "https://api.anthropic.com": {
      "active": true,
      "http_headers": {
        "x-api-key": "<YOUR-API-KEY>"
      },
      "type": "claude"
    }
```

- Confirm that this block is the only one marked as `active`.
- Save the configuration.

You should now be able to use `claude`.

### 5. Integration inside the IDE
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


