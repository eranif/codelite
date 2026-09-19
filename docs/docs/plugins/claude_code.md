## The Claude Code Plugin
---

**Since CodeLite 18.5.0**

The Claude Code plugin runs [Claude Code](https://claude.com/product/claude-code) inside a dedicated CodeLite tab, so you can work
with it right next to your editor instead of switching to an external terminal. The tab is a regular CodeLite terminal, wired up
so file paths, symbols and URLs that Claude Code prints can be opened directly inside the IDE.

### Requirements

The plugin does not bundle Claude Code — it launches the `claude` CLI, which must be installed separately. See
[claude.com/product/claude-code](https://claude.com/product/claude-code) for installation instructions.

A workspace must be open before you can launch Claude Code; the plugin uses the workspace root as Claude Code's working directory.

### Launching Claude Code

There are two ways to start a session:

- Click the **Claude Code** button on the left side bar.
- From the menu bar: `Plugins` &#8594; `Claude Code` &#8594; `Launch Claude Code` (++ctrl+shift+i++).

If a Claude Code tab is already open, either action simply switches to it instead of starting a second session.

Each time you launch Claude Code, the plugin tries to resume the most recent conversation for the workspace (equivalent to running
`claude --continue`); if there is no previous conversation to resume, it falls back to a fresh session automatically.

When the underlying `claude` process exits, its tab closes on its own.

### Locating the `claude` Executable

By default, the plugin looks up `claude` on your `PATH`. If it isn't on the `PATH`, or you want to point CodeLite at a specific
build, set an explicit path:

- `Plugins` &#8594; `Claude Code` &#8594; `Options...`
- Enter the full path to the `claude` executable and click `OK`.

Leave the field empty to go back to resolving `claude` from the `PATH`.

!!! Note
    On a **remote workspace** (opened via the [Remoty](remoty.md) plugin over SSH/SFTP), the plugin always runs `claude` from the
    remote host's `PATH` — the configured local executable path is not used in that case.

### Clickable Terminal Output

Inside the Claude Code tab, Ctrl+Left-Click (Cmd+Left-Click on macOS) any word or path that Claude Code prints to open it inside
CodeLite, without leaving the terminal:

- **File paths** — opened in the CodeLite editor. Relative paths are resolved against the workspace.
- **Directories** — opened in your system's file explorer.
- **Executables** — launched with the system's default handler.
- **Plain URLs** — opened in your default web browser.
- Anything else — for example a class or symbol name — is looked up through the **Open Resource** dialog, so you can jump straight
  to its definition even when Claude Code didn't print a full file path.

### Remote Workspaces

When your workspace is a remote one (via the [Remoty](remoty.md) plugin), the Claude Code tab connects over the same SSH account
and runs `claude` on the remote host. Clicking a file path resolves it as a remote file — opened through the SFTP file cache —
instead of looking for it on the local disk.

### Tab Behavior

- The tab's title tracks the title Claude Code reports for the current session.
- The terminal's colors and font follow CodeLite's active editor theme, and update automatically if you switch themes while the
  session is running.

---

**Tip:** you can keep multiple long-running Claude Code sessions across different workspaces open in separate CodeLite windows —
each workspace gets its own tab.
