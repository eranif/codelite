# CodeLite Lua Scripting API Documentation

## Overview

This document provides comprehensive API documentation for CodeLite's Lua scripting interface. It serves as a complete reference guide for developers who want to extend CodeLite's functionality through Lua-based extensions and automation scripts.

## API Categories

The CodeLite Lua API is organized into five main categories:

- **UI Functions** – Display message boxes and add custom menu items
- **Editor Functions** – Access editor content, selections, language types, and file paths
- **LLM Functions** – Integrate with language learning models for chat and code generation
- **Logging Functions** – Output diagnostic information with multiple severity levels
- **String Utilities** – Perform text manipulation operations

## Function Reference

### UI Functions

Functions for creating user interface elements and displaying information to users.

*Each function entry includes its signature, description, parameters with data types, return values, and practical code examples.*

### Editor Functions

Functions for programmatically accessing and manipulating editor content and properties.

*Access current file content, manage text selections, identify programming languages, and retrieve file paths.*

### LLM Functions

Functions for integrating with language learning models to enhance development workflows.

*Implement AI-assisted features such as chat interfaces and automated code generation.*

### Logging Functions

Functions for outputting diagnostic and debugging information at various severity levels.

**Supported severity levels:**
- Trace
- Debug
- Info
- Warning
- Error

### String Utilities

Functions for performing common text manipulation and transformation operations.

*Process and format strings for use within CodeLite extensions.*

---

## UI Functions

### message_box

**Signature**:

```lua
codelite.message_box(message, type)
```

**Description**:

Displays a message dialog box with the specified message and type.

**Parameters**:
- `message` (string): The text to display in the dialog
- `type` (integer): The type of message dialog to display (e.g., `MessageType.kWarn`, `MessageType.kError`, or `MessageType.kInfo`)

**Example**:

```lua
-- Display a warning message
codelite.message_box("Configuration file not found", MessageType.kWarn)
```

---

### user_text

**Signature**:

```lua
codelite.user_text(title)
```

**Description**:

Prompts the user for text input via a dialog box and returns the entered text.

**Parameters**:
- `title` (string): The title/prompt message to display in the input dialog

**Returns**:

A `string` containing the user's input, or an empty string if the user canceled or entered no text.

**Example**:

```lua
-- Prompt user for a project name
local project_name = codelite.user_text("Enter project name:")
if project_name ~= "" then
    codelite.log_system("Creating project: " .. project_name)
else
    codelite.log_system("Project creation cancelled")
end
```

---

### add_menu_item

**Signature**:

```lua
codelite.add_menu_item(menu_name, label, action)
```

**Description**:

Adds a menu item with an associated Lua action to a specified menu (see below for a list of available menus).
The callback function will be executed when the menu item is triggered.

**Parameters**:

- `menu_name` (string): The name of the menu to which the item will be added
- `label` (string): The display label for the menu item
- `action` (function): A Lua function that will be called when the menu item is activated

**Example**:

```lua
-- Add a custom menu item
codelite.add_menu_item("Tools", "My Custom Action", function()
    codelite.message_box("Custom action triggered!", MessageType.kInfo)
end)
```

**Supported Menus**

The following menu names can be used as the first argument to specify context menu locations:

#### Available Menu Contexts

| Menu Name | Description |
|-----------|-------------|
| `editor_context_menu_llm_generation` | Context menu displayed when right-clicking within an editor and selecting **AI-powered options** |
| `editor_tab_right_click` | Context menu displayed when right-clicking on an open editor tab label |
| `file_tree_file` | Context menu displayed when right-clicking on a file in the tree view |
| `file_tree_folder` | Context menu displayed when right-clicking on a folder in the tree view |
| `editor_right_click` | Context menu displayed when right-clicking within a **C++** file editor |
| `editor_right_click_default` | Context menu displayed when right-clicking within any non-C++ file editor |

#### Top-Level Menu Items

In addition to the context menus listed above, you can reference any top-level menu item from the application menu bar, such as:

- `File`
- `Edit`
- `Search`
- Other standard menu bar items

---

### add_menu_separator

**Signature**:

```lua
codelite.add_menu_separator(menu_name)
```

**Description**:

Adds a visual separator to the specified menu. If the menu does not exist, it will be created automatically with the separator as its first item.

**Parameters**:

- `menu_name` (string): The name of the menu to which the separator should be added

**Example**:

```lua
-- Add a separator to the Tools menu
codelite.add_menu_separator("Tools")

-- Create a new menu with a separator
codelite.add_menu_separator("My Custom Menu")
```

---

## Editor Functions

### editor_selection

**Signature**:

```lua
codelite.editor_selection()
```

**Description**:

Retrieves the current selection text from the active editor.

**Returns**:

A `string` containing the selected text, or an empty string if no editor is active or no text is selected.

**Example**:

```lua
-- Get the current selection
local selected = codelite.editor_selection()
if selected ~= "" then
    codelite.log_system("Selected text: " .. selected)
end
```

---

### editor_language

**Signature**:

```lua
codelite.editor_language()
```

**Description**:

Retrieves the programming language associated with the currently active editor. The language is determined from the file type of the editor's path.

**Returns**:

A `string` containing the language name, or an empty string if no editor is active.

**Example**:

```lua
-- Check the current editor's language
local lang = codelite.editor_language()
if lang == "cpp" then
    codelite.log_system("Editing C++ file")
end
```

---

### editor_text

**Signature**:

```lua
codelite.editor_text()
```

**Description**:

Returns the full text content of the currently active editor.

**Returns**:

A `string` containing the entire text of the active editor, or an empty string if no editor is active.

**Example**:

```lua
-- Get all text from the current editor
local content = codelite.editor_text()
if content ~= "" then
    codelite.log_debug("Editor has " .. #content .. " characters")
end
```

---

### editor_filepath

**Signature**:

```lua
codelite.editor_filepath()
```

**Description**:

Retrieves the file path of the currently active editor.

**Returns**:

A `string` containing the file path, or an empty string if no editor is active.

**Example**:

```lua
-- Get the current file path
local filepath = codelite.editor_filepath()
if filepath ~= "" then
    codelite.log_system("Editing: " .. filepath)
end
```

---

## LLM Functions

### chat

**Signature**:

```lua
codelite.chat(prompt)
```

**Description**:

Initiates a chat session using the given prompt. This opens the chat window within the application with the provided text as the initial input.

**Parameters**:
- `prompt` (string): The initial text to be displayed in the chat input

**Example**:

```lua
-- Open chat with a predefined prompt
codelite.chat("Explain how to use this API")
```

---

### generate

**Signature**:

```lua
codelite.generate(prompt)
```

**Description**:

Initiates an LLM text generation process with the given prompt. Displays a warning if a generation is already in progress. The function shows a generation dialog and processes the prompt using the LLM Manager.

**Parameters**:
- `prompt` (string): The text prompt to send to the LLM for generation

**Example**:

```lua
-- Generate code using LLM
codelite.generate("Write a function to sort an array")
```

---

## Logging Functions

### log_error

**Signature**:

```lua
codelite.log_error(msg)
```

**Description**:

Logs an error message. This should be called whenever an error needs to be recorded.

**Parameters**:
- `msg` (string): The error message to be logged

**Example**:

```lua
-- Log an error
codelite.log_error("Failed to open configuration file")
```

---

### log_system

**Signature**:

```lua
codelite.log_system(msg)
```

**Description**:

Logs a system message. Intended for messages that are relevant to system operations and diagnostics.

**Parameters**:
- `msg` (string): The message to be logged

**Example**:

```lua
-- Log a system message
codelite.log_system("Plugin initialized successfully")
```

---

### log_warn

**Signature**:

```lua
codelite.log_warn(msg)
```

**Description**:

Logs a warning message.

**Parameters**:
- `msg` (string): The warning message to be logged

**Example**:

```lua
-- Log a warning
codelite.log_warn("Configuration value not found, using default")
```

---

### log_debug

**Signature**:

```lua
codelite.log_debug(msg)
```

**Description**:

Logs a debug-level message. Intended for debugging output during development.

**Parameters**:
- `msg` (string): The debug message to be logged

**Example**:

```lua
-- Log debug information
codelite.log_debug("Variable value: " .. tostring(my_var))
```

---

### log_trace

**Signature**:

```lua
codelite.log_trace(msg)
```

**Description**:

Logs a trace message at the developer log level. Used for detailed execution tracing.

**Parameters**:
- `msg` (string): The trace message to be logged

**Example**:

```lua
-- Log detailed trace information
codelite.log_trace("Entering function process_data()")
```

---

## String Utilities

### str_replace_all

**Signature**:

```lua
codelite.str_replace_all(str, find_what, replace_with)
```

**Description**:

Replaces all occurrences of a substring within a string with another substring. If `find_what` is empty, the original string is returned unchanged.

**Parameters**:
- `str` (string): The input string to search within
- `find_what` (string): The substring to search for and replace
- `replace_with` (string): The substring to replace each occurrence of `find_what` with

**Returns**:

A `string` with all occurrences of `find_what` replaced by `replace_with`.

**Example**:

```lua
-- Replace all occurrences
local input = "Hello world, world!"
local result = codelite.str_replace_all(input, "world", "universe")
-- result is "Hello universe, universe!"

-- No match case
local no_match = codelite.str_replace_all(input, "xyz", "abc")
-- no_match is "Hello world, world!" (unchanged)
```