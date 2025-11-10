---@meta

---CodeLite Lua Scripting API
---
---This module provides the CodeLite API for Lua scripting extensions.
---The API is organized into five main categories:
--- - UI Functions: Display message boxes and add custom menu items
--- - Editor Functions: Access editor content, selections, language types, and file paths
--- - LLM Functions: Integrate with language learning models for chat and code generation
--- - Logging Functions: Output diagnostic information with multiple severity levels
--- - String Utilities: Perform text manipulation operations
---@class codelite
codelite = {}

---Message types for message boxes
---@class MessageType
MessageType = {
    ---Information message type
    kInfo = 0,
    ---Warning message type
    kWarn = 1,
    ---Error message type
    kError = 2,
}

---Available menu contexts for add_menu_item
---
---Context Menus:
--- - "editor_context_menu_llm_generation": AI-powered options in editor
--- - "editor_tab_right_click": Right-click on editor tab label
--- - "file_tree_file": Right-click on file in tree view
--- - "file_tree_folder": Right-click on folder in tree view
--- - "editor_right_click": Right-click within C++ file editor
--- - "editor_right_click_default": Right-click within non-C++ file editor
---
---Top-Level Menus:
--- - "File", "Edit", "Search", "Tools", etc.
---@alias MenuName string

-- ============================================================================
-- UI Functions
-- ============================================================================

---Displays a message dialog box with the specified message and type.
---@param message string The text to display in the dialog
---@param type integer The type of message dialog (MessageType.kInfo, MessageType.kWarn, or MessageType.kError)
---
---Example:
---```lua
---codelite.message_box("Configuration file not found", MessageType.kWarn)
---```
function codelite.message_box(message, type) end

---Prompts the user for text input via a dialog box and returns the entered text.
---@param title string The title/prompt message to display in the input dialog
---@return string user_input The user's input, or empty string if canceled
---
---Example:
---```lua
---local project_name = codelite.user_text("Enter project name:")
---if project_name ~= "" then
---    codelite.log_system("Creating project: " .. project_name)
---end
---```
function codelite.user_text(title) end

---Adds a menu item with an associated Lua action to a specified menu.
---The callback function will be executed when the menu item is triggered.
---@param menu_name MenuName The name of the menu to which the item will be added
---@param label string The display label for the menu item
---@param action function A Lua function that will be called when the menu item is activated
---
---Example:
---```lua
---codelite.add_menu_item("Tools", "My Custom Action", function()
---    codelite.message_box("Custom action triggered!", MessageType.kInfo)
---end)
---```
function codelite.add_menu_item(menu_name, label, action) end

---Adds a visual separator to the specified menu.
---If the menu does not exist, it will be created automatically with the separator as its first item.
---@param menu_name MenuName The name of the menu to which the separator should be added
---
---Example:
---```lua
---codelite.add_menu_separator("Tools")
---```
function codelite.add_menu_separator(menu_name) end

-- ============================================================================
-- Editor Functions
-- ============================================================================

---Retrieves the current selection text from the active editor.
---@return string selected_text The selected text, or empty string if no editor is active or no text is selected
---
---Example:
---```lua
---local selected = codelite.editor_selection()
---if selected ~= "" then
---    codelite.log_system("Selected text: " .. selected)
---end
---```
function codelite.editor_selection() end

---Retrieves the programming language associated with the currently active editor.
---The language is determined from the file type of the editor's path.
---@return string language The language name, or empty string if no editor is active
---
---Example:
---```lua
---local lang = codelite.editor_language()
---if lang == "cpp" then
---    codelite.log_system("Editing C++ file")
---end
---```
function codelite.editor_language() end

---Returns the full text content of the currently active editor.
---@return string content The entire text of the active editor, or empty string if no editor is active
---
---Example:
---```lua
---local content = codelite.editor_text()
---if content ~= "" then
---    codelite.log_debug("Editor has " .. #content .. " characters")
---end
---```
function codelite.editor_text() end

---Retrieves the file path of the currently active editor.
---@return string filepath The file path, or empty string if no editor is active
---
---Example:
---```lua
---local filepath = codelite.editor_filepath()
---if filepath ~= "" then
---    codelite.log_system("Editing: " .. filepath)
---end
---```
function codelite.editor_filepath() end

-- ============================================================================
-- LLM Functions
-- ============================================================================

---Initiates a chat session using the given prompt.
---This opens the chat window within the application with the provided text as the initial input.
---@param prompt string The initial text to be displayed in the chat input
---
---Example:
---```lua
---codelite.chat("Explain how to use this API")
---```
function codelite.chat(prompt) end

---Initiates an LLM text generation process with the given prompt.
---Displays a warning if a generation is already in progress.
---The function shows a generation dialog and processes the prompt using the LLM Manager.
---@param prompt string The text prompt to send to the LLM for generation
---
---Example:
---```lua
---codelite.generate("Write a function to sort an array")
---```
function codelite.generate(prompt) end

-- ============================================================================
-- Logging Functions
-- ============================================================================

---Logs an error message.
---This should be called whenever an error needs to be recorded.
---@param msg string The error message to be logged
---
---Example:
---```lua
---codelite.log_error("Failed to open configuration file")
---```
function codelite.log_error(msg) end

---Logs a system message.
---Intended for messages that are relevant to system operations and diagnostics.
---@param msg string The message to be logged
---
---Example:
---```lua
---codelite.log_system("Plugin initialized successfully")
---```
function codelite.log_system(msg) end

---Logs a warning message.
---@param msg string The warning message to be logged
---
---Example:
---```lua
---codelite.log_warn("Configuration value not found, using default")
---```
function codelite.log_warn(msg) end

---Logs a debug-level message.
---Intended for debugging output during development.
---@param msg string The debug message to be logged
---
---Example:
---```lua
---codelite.log_debug("Variable value: " .. tostring(my_var))
---```
function codelite.log_debug(msg) end

---Logs a trace message at the developer log level.
---Used for detailed execution tracing.
---@param msg string The trace message to be logged
---
---Example:
---```lua
---codelite.log_trace("Entering function process_data()")
---```
function codelite.log_trace(msg) end

-- ============================================================================
-- String Utilities
-- ============================================================================

---Replaces all occurrences of a substring within a string with another substring.
---If find_what is empty, the original string is returned unchanged.
---@param str string The input string to search within
---@param find_what string The substring to search for and replace
---@param replace_with string The substring to replace each occurrence of find_what with
---@return string result String with all occurrences replaced
---
---Example:
---```lua
---local input = "Hello world, world!"
---local result = codelite.str_replace_all(input, "world", "universe")
----- result is "Hello universe, universe!"
---```
function codelite.str_replace_all(str, find_what, replace_with) end

return codelite
