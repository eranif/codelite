#include <string>

static const std::string kDefaultCodeLiteLUA =
    R"#(---@class codelite
---@type codelite
codelite = codelite or {}

--- Polishes and reformats the current markdown document using AI generation
--- @return nil
function OnPolishDoc()
    local text = codelite.editor_text()

    -- Early return if no text is selected
    if not text or text == "" then
        return
    end

    -- Define the AI prompt template for document polishing
    local PROMPT_TEMPLATE = [[
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

{{context}}
]]

    -- Substitute the placeholder with actual document content
    local formatted_prompt = codelite.str_replace_all(PROMPT_TEMPLATE, "{{context}}", text)

    -- Request AI generation with the formatted prompt
    codelite.generate(formatted_prompt)
end

--- Polishes and professionally rewrites selected markdown content
--- Uses AI generation to improve formatting, structure, and clarity
function OnPolishMarkdownSelection()
    local selected_text = codelite.editor_selection()

    -- Early return if no text is selected
    if not selected_text or selected_text == "" then
        codelite.message_box("Empty Selection", 1)
        return
    end

    -- Prompt template for professional markdown rewriting
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

{{context}}
]]

    -- Substitute placeholder with selected content
    local formatted_prompt = codelite.str_replace_all(prompt_template, "{{context}}", selected_text)

    -- Generate and apply the polished markdown
    codelite.generate(formatted_prompt)
end

--- Refactor the currently selected text in the editor using AI
--- Sends the selected code to an AI model with refactoring instructions
--- @return nil
function OnRefactorText()
    local selected_text = codelite.editor_selection()

    -- Early return if no text is selected
    if not selected_text or selected_text == "" then
        codelite.message_box("Empty Selection", 1)
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

--- Registers LLM generation commands in the editor context menu.
function RegisterLLMMenuItems()
    --- menu_id can be found in the file menu.xrc
    local menuItems = {
        { menu_id = "editor_context_menu_llm_generation", handler = nil },
        { menu_id = "editor_context_menu_llm_generation", label = "Polish Markdown (Entire Doc)", handler = OnPolishDoc },
        { menu_id = "editor_context_menu_llm_generation", label = "Polish Markdown (Selected Text)", handler = OnPolishMarkdownSelection },
        { menu_id = "editor_context_menu_llm_generation", handler = nil },
        { menu_id = "editor_context_menu_llm_generation", label = "Refactor Selected Code", handler = OnRefactorText },
    }

    for _, item in ipairs(menuItems) do
        if item.handler == nil then
            codelite.add_menu_separator(item.menu_id)
        else
            codelite.add_menu_item(item.menu_id, item.label, item.handler)
        end
    end
end

RegisterLLMMenuItems()
)#";
