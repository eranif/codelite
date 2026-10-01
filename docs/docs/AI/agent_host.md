# Agent Host: Claude Code & Kiro CLI

!!! Note
    CodeLite offers two flavours of AI. This page covers **Agent Host**, which runs an existing coding agent in a CodeLite
    tab. CodeLite also has a built-in assistant, see [Chat AI](chat_ai.md).

**Claude Code: since CodeLite 18.5.0. Kiro CLI and the Agent Host name: since CodeLite 19.0.0.**

The Agent Host plugin runs an existing coding agent inside a dedicated CodeLite tab, so you can work with it right next to your
editor instead of switching to an external terminal. Two agents are supported:

- [Claude Code](https://claude.com/product/claude-code)
- [Kiro CLI](https://kiro.dev)

The tab is a regular CodeLite terminal. File paths, symbols and URLs that the agent prints can be opened directly inside the IDE.
Each agent type keeps its own page, so you can run both at the same time.

## Enabling the Plugin

Agent Host is provided by a plugin. To use it, open `Plugins` &#8594; `Manage Plugins...` and make sure that the `AgentHost`
plugin is checked. If you used the **Claude Code** plugin before, it was renamed to Agent Host and stays enabled after you
upgrade.

## Requirements

The plugin does not bundle the agents. It launches their command line tools, which you must install separately:

| Agent | Executable |
|-------|------------|
| Claude Code | `claude` (see [claude.com/product/claude-code](https://claude.com/product/claude-code)) |
| Kiro CLI | `kiro-cli` (see [kiro.dev](https://kiro.dev)) |

A workspace must be open before you can launch an agent. The plugin uses the workspace root as the agent's working directory.

## Launching an Agent

There are two ways to start a session:

- Click the agent button on the left side bar.
- From the menu bar: `Plugins` &#8594; `Agent Host` &#8594; `Launch Claude Code` (++ctrl+shift+c++) or `Launch Kiro Cli` (++ctrl+shift+k++).

If a tab for that agent is already open, either action switches to it instead of starting a second session. When you click the
tab, the focus moves to its terminal.

Each time you launch an agent, the plugin tries to resume the most recent conversation for the workspace. If there is no
previous conversation, it starts a fresh session automatically. When the agent process exits, its tab closes on its own.

## Locating the Agent Executable

By default, the plugin looks up `claude` and `kiro-cli` on your `PATH`. If an executable is not on the `PATH`, or you want to
use a specific build, set an explicit path:

- `Plugins` &#8594; `Agent Host` &#8594; `Settings...`
- Enter the full path to the executable of the agent and click `OK`.

Leave a field empty to go back to resolving the executable from the `PATH`.

!!! Note
    On a **remote workspace** (opened via the [Remoty](../plugins/remoty.md) plugin over SSH/SFTP), the plugin always runs the agent
    from the remote host's `PATH`. The configured local executable path is not used in that case.

## Agent System Prompt

Agent Host passes the Chat AI [system prompt](chat_ai.md#system-prompt) to the agent when it starts. Edit it with
**Chat AI → Edit System Prompt...**.

| Agent | How the prompt is passed |
|-------|--------------------------|
| Claude Code | Saved to a `SYSTEM_PROMPT.md` file in CodeLite's settings folder and passed with `--append-system-prompt-file`. |
| Kiro CLI | Written to `.kiro/steering/SYSTEM_PROMPT.md` in the workspace. |

If the system prompt is empty, nothing is passed to the agent.

!!! Note
    For Kiro CLI, the file in your workspace is overwritten each time the agent starts. You may want to add
    `.kiro/steering/SYSTEM_PROMPT.md` to your `.gitignore`.

## Clickable Terminal Output

Inside the agent tab, Ctrl+Left-Click (Cmd+Left-Click on macOS) any word or path that the agent prints to open it inside
CodeLite, without leaving the terminal:

- **File paths**: opened in the CodeLite editor. Relative paths are resolved against the workspace.
- **Directories**: opened in your system's file explorer.
- **Executables**: launched with the system's default handler.
- **Plain URLs**: opened in your default web browser.
- Anything else, for example a class or symbol name, is looked up through the **Open Resource** dialog. You can jump straight
  to its definition even when the agent did not print a full file path.

## Agent Host on Remote Workspaces

When your workspace is a remote one (via the [Remoty](../plugins/remoty.md) plugin), the agent tab connects over the same SSH
account and runs the agent on the remote host. Clicking a file path resolves it as a remote file, opened through the SFTP file
cache, instead of looking for it on the local disk.

## Tab Behavior

- The tab's title tracks the title that the agent reports for the current session.
- The terminal's colors and font follow CodeLite's active editor theme, and update automatically if you switch themes while the
  session is running.

**Tip:** you can keep several long-running agent sessions for different workspaces open in separate CodeLite windows. Each
workspace gets its own tab.
