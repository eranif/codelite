# Agent Host: Claude Code, Kiro CLI & OpenCode

!!! Note
    CodeLite offers two flavours of AI. This page covers **Agent Host**, which runs an existing coding agent in a CodeLite
    tab. CodeLite also has a built-in assistant, see [Chat AI](chat_ai.md).

**Claude Code: since CodeLite 18.5.0. Kiro CLI, OpenCode and the Agent Host name: since CodeLite 19.0.0.**

The Agent Host plugin runs an existing coding agent inside a dedicated CodeLite tab, so you can work with it right next to your
editor instead of switching to an external terminal. Three agents are supported:

- [Claude Code](https://claude.com/product/claude-code)
- [Kiro CLI](https://kiro.dev)
- [OpenCode](https://opencode.ai)

The tab is a regular CodeLite terminal. File paths, symbols and URLs that the agent prints can be opened directly inside the IDE.
Each agent type keeps its own page, so you can run them at the same time.

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
| OpenCode | `opencode` (see [opencode.ai](https://opencode.ai)) |

A workspace must be open before you can launch an agent. The plugin uses the workspace root as the agent's working directory.

## Launching an Agent

There are two ways to start a session:

- Click the agent button on the left side bar (the one with the terminal icon), and select the agent from the menu. The menu shows the keyboard shortcut of each agent (if it has one).
- From the menu bar: `Plugins` &#8594; `Agent Host` &#8594; `Launch Claude Code` (++ctrl+shift+c++), `Launch Kiro Cli` (++ctrl+shift+k++) or `Launch OpenCode` (no default shortcut, you can assign one in the `Keyboard Shortcuts` dialog).

If a tab for that agent is already open, either action switches to it instead of starting a second session. When you click the
tab, the focus moves to its terminal.

Each time you launch an agent, the plugin tries to resume the most recent conversation for the workspace. If there is no
previous conversation, it starts a fresh session automatically. When the agent process exits, its tab closes on its own.

## Locating the Agent Executable

By default, the plugin looks up `claude`, `kiro-cli` and `opencode` on your `PATH`. If an executable is not on the `PATH`, or you want to
use a specific build, set an explicit path:

- `Plugins` &#8594; `Agent Host` &#8594; `Settings...`
- Enter the full path to the executable of the agent (there is one field for each agent) and click `OK`.

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
| OpenCode | Saved to a `SYSTEM_PROMPT.md` file in the `.codelite` folder of the workspace. CodeLite also writes an `opencode.json` file next to it, which lists the prompt file under `instructions`, and starts OpenCode with the `OPENCODE_CONFIG` environment variable pointing to it. |

If the system prompt is empty, nothing is passed to the agent.

!!! Note
    For Kiro CLI, the file in your workspace is overwritten each time the agent starts. You may want to add
    `.kiro/steering/SYSTEM_PROMPT.md` to your `.gitignore`.

!!! Note
    For OpenCode, the prompt is added to the OpenCode instructions. It does not replace the OpenCode system prompt. On a remote
    workspace, `OPENCODE_CONFIG` is set only for the local SSH process, so the prompt is not passed to OpenCode unless your SSH
    account forwards that variable.
## Clickable Terminal Output

Inside the agent tab, Ctrl+Left-Click (Cmd+Left-Click on macOS) any word or path that the agent prints to open it inside
CodeLite, without leaving the terminal:

- **File paths**: opened in the CodeLite editor. Relative paths are resolved against the workspace.
- **Directories**: opened in your system's file explorer.
- **Executables**: launched with the system's default handler.
- **Plain URLs**: opened in your default web browser.
- Anything else, for example a class or symbol name, is looked up through the **Open Resource** dialog. You can jump straight
  to its definition even when the agent did not print a full file path.

## Terminal Right-Click Menu

Right-click inside the agent terminal to open its menu. Besides `Copy`, `Paste` and `Refresh` (restarts the agent), it has:

- When text is selected: `Search '<text>' in Workspace` and `Open Symbol '<text>'`. Only the first line of the selection is used.
  `Search ... in Workspace` shows the keyboard shortcut of `Grep Selection in the Workspace` (if you assigned one), and that
  shortcut also works while the agent terminal has the focus.
- The `Review Buddy` actions, see below.

The shortcuts of the main window (for example ++f6++, which toggles the terminal view) also work while the agent terminal has
the focus.

## Review Buddy

**Review Buddy** starts a second agent (any of the supported agents) that reviews the work that is not pushed yet, in a pane next to
the main agent. Start it from the right-click menu of the agent terminal.

- The two agents work in the same folder and talk to each other through files in `.agents/reviews/<id>/`. The reviewer writes
  its comments, the main agent fixes them, and the reviewer checks again. This repeats until the review is clean, or for at most
  5 rounds.
- CodeLite types the requests into the terminals for you. It tells you when the review is finished, when the round limit is
  reached (the last fixes are not reviewed in that case), or when an agent seems to wait for you for a long time. If you are not
  looking at the page, you get a system notification.
- The right-click menu also has `Resend`, `Stop`, `Open the Latest Review` and `Close Review Buddy`.
- It needs a working directory inside a git repository (a sub-folder or a worktree is fine). It works for remote workspaces too.
  The `.agents/reviews/` folder is added to `.git/info/exclude`, so it does not show in `git status`.

To pass extra command line arguments to the reviewer (for example, a custom agent), set them in `Plugins` &#8594; `Agent Host`
&#8594; `Settings...`, on the `Review Buddy` page. There is one field for each agent.

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
