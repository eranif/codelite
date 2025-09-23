# ChatAI: Your Local AI Assistant
---

With the release of **CodeLite 18.3.0**, a brand‑new plugin has been added that embeds a chat interface powered by a local language model of your choice. The plugin talks to the model through an **Ollama** server.

## Getting Started
---

### 1. Install the Ollama Server & Pull a Model
---

1. Go to the [Ollama download page](https://ollama.com/download) and follow the instructions for your operating system.
2. Once installed, you can interact with Ollama from the command line using the `ollama` command.
3. Pull a model that fits your hardware. Recommended options:

   - `qwen3-coder:30b` – requires ~26 GB of RAM.
   - `gpt-oss:20b` – excellent performance, needs ~16 GB of RAM.

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

Once the server is running, open the **ChatAI** view (`Ctrl‑Shift‑H` on Windows, `Cmd‑Shift‑H` on macOS).
The view is split into two panes:

- **Output pane** – shows the model’s responses.
- **Prompt pane** – where you type your questions.

At the top of the prompt pane is a toolbar with useful buttons. If the Ollama server is correctly configured, the drop‑down will list all available models, allowing you to set a default.

Click the “wrench” icon to edit the plugin’s configuration file. A typical file looks like this:

```json
{
  "endpoints": {
    "http://127.0.0.1:11434": {
      "active": true,
      "http_headers": { "Host": "127.0.0.1" },
      "type": "ollama"
    }
  },
  "history_size": 100,
  "log_level": "info",
  "mcp_servers": {},
  "models": {
    "default": {
      "options": { "num_ctx": 32768, "temperature": 0 },
      "think_start_tag": "</think>",
      "think_end_tag": "</think>"
    }
  }
}
```

Save the file to apply changes; the plugin will reload automatically.

#### Endpoints
The `endpoints` section lets you configure multiple Ollama servers. A typical entry:

```json
"http://127.0.0.1:11434": {
  "active": true,
  "http_headers": { "Host": "127.0.0.1" },
  "type": "ollama"
}
```

- **active** – `true` or `false`. If none are active, the first one is used.
- **http_headers** – any headers you want to include in the REST call.
- **type** – currently unused.

#### MCP Servers (Model Context Protocol)
MCP extends a model’s capabilities by exposing tools that the model can call. For example, a model can’t normally write files, but with MCP you can provide a “write file” tool. The plugin informs the model of available tools and their arguments.

You can add your own MCP servers (e.g., Python scripts using FastMCP) to the `mcp_servers` section:

```json
"mcp_servers": {
  "my_tool": {
    "command": ["python3", "my_tool.py"],
    "enabled": true,
    "env": { "SOME_VAR1": "value1", "SOME_VAR2": "value2" }
  }
}
```

#### Models
All models are pre‑configured, but you can customize them by adding an entry under `models`. Example:

```json
"qwen3-coder:30b": {
  "options": { "num_ctx": 32768, "temperature": 0 }
}
```

- **num_ctx** – the context window (number of tokens the model can remember). Check the exact size with `ollama show <MODEL-NAME>`.
- **temperature** – controls randomness (0–1).
- **num_gpu** – optional; set to `0` to load the model on CPU instead of GPU.

#### General Settings
- **history_size** – the number of recent exchanges sent to the model for context.
- **log_level** – verbosity of the underlying library (default `info`).

### 3. Available Tools
---

CodeLite exposes the following tools to the models (NOTE: the model must have the "tools" capability in order to use them):

- **Get_the_text_of_the_active_tab_inside_the_editor** – no parameters.
- **Open_a_file_in_an_editor** – `filepath` (string) – Path of the file to open in the editor.
- **Read_file_from_the_file_system** – `filepath` (string) – Path of the file to read.
- **Read_the_compiler_build_output** – no parameters.
- **Write_file_content_to_disk_at_a_given_path** – `file_content` (string) – Text to write to the file; `filepath` (string) – Destination path for the file.

What can you do with this?

By manipulating the prompt, the model will pick one of the available tools and use it.

**Example use case:**

You compiled a C++ program and got a compiler error that you don’t understand. Typing this in the prompt:

```
Explain the build errors I got from the compiler and suggest ways to fix them.
```

The model will invoke the tool `Read_the_compiler_build_output` to fetch the build output and then provide an explanation.