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
      "think_end_tag": "</think>",
      "think_start_tag": "</think>"
    }
  }
}
```

Save the file to apply changes; the plugin will reload automatically.

---

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
      - `think_start_tag`: `"</think>"` – marker that signals the start of a “think” block in the model’s output.
      - `think_end_tag`: `"</think>"` – marker that signals the end of a “think” block.

---

#### How It All Connects

- The server listens on the defined endpoints (here, a local Ollama instance).
- When a connection establishes, it uses the `server_timeout` values to manage connection timing.
- The `stream` flag tells the server to send partial responses as they become available.
- The `history_size` limits how much conversation history is stored for context.
- The `models.default` block supplies default inference parameters (context size, temperature) and tags that the model may use to structure its output. This can be duplicated with a specific model name.
- Logging verbosity is set to `info` (other values: `debug`, `trace`, `error` and `warn`)
- A model is kept in-memory for `24 hours` before being removed from the server memory.

This configuration allows the service to operate with a single local endpoint, deterministic inference, and controlled resource usage.

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

### 4. Using `claude`

CodeLite's AI support also enables the use of Anthropic's Claude LLM.

#### Prerequisites

- You need an account or can register at [Anthropic's website](https://www.anthropic.com/).
- Generate an API key.
- Ensure you have sufficient credits.

#### Configuration

- Open the ChatAI configuration file using (++ctrl+shift+h++) or (++cmd+shift+h++), and click on the wrench (🔧) button.
- In the `endpoints` section, add the followings:

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
