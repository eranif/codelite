# LUA Scripting

Since CodeLite 18.3.0, CodeLite embeds a LUA engine to enable customization and extensibility. Custom actions can be added to CodeLite through menu integrations, allowing developers to extend the IDE's functionality. To facilitate interaction with CodeLite's internal components, a dedicated `codelite` namespace has been added to the LUA engine.

## Complete Example: Selection Info

### Objective

This example shows how to add a custom item to the editor context menu. The item shows information about the selected text.

**User Workflow:**

1. User selects text in the editor
2. Right-clicks and chooses **Selection Info** from the context menu
3. A message box shows the file, the language, the number of lines and the number of characters of the selection

### Implementation

````lua
function on_selection_info()
    local selected_text = codelite.editor_selection()

    -- Early return if no text is selected
    if not selected_text or selected_text == "" then
        codelite.message_box("Nothing is selected", MessageType.kWarn)
        return
    end

    -- Count the lines of the selection
    local _, newlines = string.gsub(selected_text, "\n", "")
    local lines = newlines + 1

    local info = string.format(
        "File: %s\nLanguage: %s\nLines: %d\nCharacters: %d",
        codelite.editor_filepath(),
        codelite.editor_language(),
        lines,
        #selected_text
    )
    codelite.message_box(info, MessageType.kInfo)
end

-- Connect the new action "on_selection_info" to an existing menu
-- The menu ID can be found in the file `rc/menu.xrc` in CodeLite installation.
codelite.add_menu_item(
    "editor_right_click",  -- The menu ID
    "Selection Info",      -- The new item's label
    on_selection_info      -- The action
)
````

### Code Explanation

| Component | Description |
|-----------|-------------|
| `codelite.editor_selection()` | Returns the currently selected text from the active editor |
| `codelite.editor_filepath()` | Returns the full path of the file in the active editor |
| `codelite.editor_language()` | Returns the programming language of the current editor context |
| `codelite.message_box()` | Shows a message box with the given text and type |
| [`codelite.add_menu_item()`](/LUA/lua/#add_menu_item) | Associates the custom action with an existing menu |

**Key Steps:**

1. Retrieve selected text using `codelite.editor_selection()`
2. Count the lines of the selection
3. Build the message with the file path, the language, the lines and the characters
4. Show it with `codelite.message_box()`
5. Register the action with the `editor_right_click` menu

> **Note:** [A complete reference to CodeLite's LUA API can be found here](/LUA/lua)

### Integration

To integrate this functionality into CodeLite:

1. Navigate to **Settings → Edit CodeLite's LUA script** from the menu bar
2. Place the above code in the `codelite.lua` file
3. Save the file

The script is automatically compiled upon saving and becomes immediately available—no additional actions required.