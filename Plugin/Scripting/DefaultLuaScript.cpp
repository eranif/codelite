#include <string>

static const std::string kDefaultCodeLiteLUA = R"#(
--- Rephrase the selected text in the editor using an AI generation service
--- Retrieves the current editor selection, formats it into a prompt, and generates a rephrased version
function on_polish_document()
    local text = codelite.editor_text()

    -- Early return if no text is selected
    if not text or text == "" then
        return
    end

    -- Define the prompt template with corrected spelling
    local prompt_template = [[
    # Task: Rewrite This Markdown Professionally

Please rewrite the following markdown document to make it professional and polished:

**Requirements:**
- Fix all formatting and markdown syntax issues
- Improve structure with clear headings and organization
- Enhance clarity and readability
- Use professional tone and correct grammar
- Add appropriate formatting (bold, italics, code blocks, lists, tables)
- Maintain all technical accuracy and key information
- Ensure consistent style throughout

**Output only the refactored markdown document. Do not include explanations, language detection notes, or any other text.**

**Original Document:**

```markdown
{{context}}
```
]]

    -- Replace placeholder with actual content
    local formatted_prompt = codelite.str_replace_all(prompt_template, "{{context}}", text)

    -- Generate the rephrased text
    codelite.generate(formatted_prompt)
end

--- Refactor the currently selected text in the editor using AI
--- Sends the selected code to an AI model with refactoring instructions
--- @return nil
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

    local formatted_prompt = codelite.str_replace_all(
        formatted_prompt,
        "{{lang}}",
        codelite.editor_language()
    )

    codelite.generate(formatted_prompt)
end

--- Registers LLM generation commands in the editor context menu.
local function registerLLMMenuItems()
    --- menu_id can be found in the file menu.xrc
    local menuItems = {
        { menu_id = "editor_context_menu_llm_generation", label = "Polish Markdown Document", handler = on_polish_document },
        { menu_id = "editor_context_menu_llm_generation", label = "Refactor Selected Code", handler = on_refactor_text },
    }

    for _, item in ipairs(menuItems) do
        codelite.add_menu_item(item.menu_id, item.label, item.handler)
    end
end

registerLLMMenuItems()
)#";
