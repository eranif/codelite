# CodeLiteLUA Helper Functions

## Overview
The following document describes the public helper functions exposed by the `codelite` name. 
These functions provide a Lua scripting interface for interacting with the CodeLite editor, menus, logging system, and the AI chat window. 
The intended audience is developers who want to extend or script CodeLite using Lua.

---

## Function: `message_box`
**Signature**
```lua
function codelite.message_box(message, type)
```

**Purpose**
- Display a modal message box in the CodeLite UI.

**Parameters**
- `message`: The text to display.
- `type`: Integer that maps to `MessageType` enum.
  - `MessageType::kInfo`  → Information icon (default).
  - `MessageType::kWarn`  → Warning icon.
  - `MessageType::kError` → Error icon.

**Behavior**
- Converts `message` from UTF‑8 to `wxString`.
- Shows the dialog with `wxOK`, centered, and the appropriate icon.
- Uses `wxMessageBox` from the wxWidgets library.

---

## Function: `add_menu_item`
**Signature**
```lua
function codelite.add_menu_item(menu_name, label, action)
```

**Purpose**
- Dynamically add a Lua‑defined menu item to an existing or new CodeLite menu.

**Parameters**
- `menu_name`: Name of the target menu (e.g., "Tools").
- `label`: Menu item caption.
- `action`: Lua function that will be executed when the item is selected.

**Behavior**
- Retrieves the singleton instance via `Get()`.
- Logs the addition attempt.
- Wraps `label` and `action` into a `LuaMenuItem`.
- Validates the item (`IsOk()`), logging a warning if the action is not a function.
- Stores the item in `self.m_menu_items[menu_name]`.

---

## Function: `editor_selection`
**Signature**
```lua
function codelite.editor_selection()
```

**Purpose**
- Return the currently selected text in the active editor.

**Behavior**
- Retrieves the active editor via `clGetManager()->GetActiveEditor()`.
- If no editor is active, returns an empty string.
- Converts the selection to a UTF‑8 `std::string`.

---

## Function: `editor_language`
**Signature**
```lua
function codelite.editor_language()
```

**Purpose**
- Identify the programming language of the active editor’s file.

**Behavior**
- Uses `FileExtManager` to map the file extension to a language enum.
- Returns the language name as a UTF‑8 string, or empty if no editor.

---

## Function: `editor_text`
**Signature**
```lua
function codelite.editor_text()
```

**Purpose**
- Fetch the entire contents of the active editor.

**Behavior**
- Returns the editor text as UTF‑8; empty string if no active editor.

---

## Function: `editor_filepath`
**Signature**
```lua
function codelite.editor_filepath()
```

**Purpose**
- Get the file path (local or remote) of the active editor.

**Behavior**
- Returns the path as a UTF‑8 string, or empty if no active editor.

---

## Logging Functions

| Function | LogLevel | Purpose |
|---|---|---|
| `log_error` | `Error` | Log error messages. |
| `log_system` | `System` | Log system‑level messages. |
| `log_warn` | `Warning` | Log warning messages. |
| `log_debug` | `Dbg` | Log debug messages. |
| `log_trace` | `Developer` | Log developer‑level trace messages. |

---

## Function: `chat`
**Signature**
```lua
function codelite.chat(prompt)
```

**Purpose**
- Trigger the CodeLite AI chat window with a given prompt.

**Behavior**
- Constructs a `wxCommandEvent` targeting the `ai_show_chat_window` ID.
- Sets the event string to the UTF‑8 prompt.
- Posts the event to the top frame’s event handler.
