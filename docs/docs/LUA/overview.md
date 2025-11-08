# LUA Scripting

Since CodeLite 18.3.0, CodeLite embeds a LUA engine to enable customization and extensibility. Custom actions can be added to CodeLite through menu integrations, allowing developers to extend the IDE's functionality. To facilitate interaction with CodeLite's internal components, a dedicated `codelite` namespace has been added to the LUA engine.

## Complete Example: AI-Powered Code Refactoring

### Objective

This example demonstrates how to create a custom functionality that leverages CodeLite's integrated LLM (AI) capability to refactor code selections.

**User Workflow:**

1. User selects code in the editor
2. Right-clicks and chooses **Refactor Selection** from the context menu
3. The LLM analyzes and refactors the code according to best practices
4. Refactored code is presented in a separate window with copy functionality

### Implementation

````lua
function on_refactor_text()
    local selected_text = codelite.editor_selection()

    -- Early return if no text is selected
    if not selected_text or selected_text == "" then
        return
    end

    local refactoring_prompt = [[
# Task: Refactor The Provided Code

You are an expert code refactoring assistant. Your task is to:

Analyze the following {{lang}} code snippet and refactor it according to that language's best practices and modern conventions.

**Output only the refactored code. Do not include explanations, language detection notes, or any other text.**

### Refactoring Guidelines:

- **Readability**: Improve variable/function names, add clarity
- **Best Practices**: Follow language-specific conventions and idioms
- **Efficiency**: Optimize where applicable without sacrificing readability
- **Documentation**: Improve or add comments where helpful
- **Structure**: Better organize code structure if needed
- **Modern Syntax**: Use modern language features when appropriate

Code:
```{{lang}}
{{context}}
```
]]
    local formatted_prompt = codelite.str_replace_all(
        refactoring_prompt,
        "{{context}}",
        selected_text
    )

    formatted_prompt = codelite.str_replace_all(
        formatted_prompt,
        "{{lang}}",
        codelite.editor_language()
    )

    codelite.generate(formatted_prompt)
end

-- Connect the new action "on_refactor_text" to an existing menu
-- The menu ID can be found in the file `rc/menu.xrc` in CodeLite installation.
codelite.add_menu_item(
    "editor_context_menu_llm_generation", -- The menu ID
    "Refactor Selected Text",             -- The new item's label
    on_refactor_text                      -- The action
)
````

### Code Explanation

| Component | Description |
|-----------|-------------|
| `codelite.editor_selection()` | Returns the currently selected text from the active editor |
| `refactoring_prompt` | Template string containing the LLM prompt with placeholders |
| `codelite.str_replace_all()` | Utility method to replace placeholders (`{{lang}}` and `{{context}}`) with actual values |
| `codelite.editor_language()` | Returns the programming language of the current editor context |
| `codelite.generate()` | Initiates LLM code generation with the formatted prompt |
| `codelite.add_menu_item()` | Associates the custom action with an existing menu item |

**Key Steps:**

1. Retrieve selected text using `codelite.editor_selection()`
2. Define the refactoring prompt template with placeholders
3. Replace `{{context}}` placeholder with the selected text
4. Replace `{{lang}}` placeholder with the current editor language
5. Invoke the LLM generation process
6. Register the action with the `editor_context_menu_llm_generation` menu

> **Note:** [A complete reference to CodeLite's LUA API can be found here](/LUA/lua)

### Integration

To integrate this functionality into CodeLite:

1. Navigate to **Settings → Edit CodeLite's LUA script** from the menu bar
2. Place the above code in the `codelite.lua` file
3. Save the file

The script is automatically compiled upon saving and becomes immediately available—no additional actions required.