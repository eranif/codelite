#include <wx/string.h>

namespace llm
{
const std::string PROMPT_DOCSTRING_GEN =
    R"#(You are an expert software developer. Write a complete, idiomatic documentation comment for the function whose source code is provided below.

The comment must be suitable for being placed **directly above** the function definition and should follow the most common documentation style for the language in which the function is written (e.g., Javadoc for Java, docstring for Python, Doxygen for C/C++, JSDoc for JavaScript/TypeScript, Rustdoc for Rust, etc.).

Add the following sections if appropriate (in this order, if the language supports them):

1. **Brief summary** – one‑sentence description of what the function does.
2. **Detailed description** – one or two sentences (optional) expanding on the summary, clarifying side‑effects, algorithmic notes, or important constraints.
3. **Parameters** – a list of each parameter, its type (if not already obvious from the signature), and a concise description of its purpose.
4. **Return value** – the type (if applicable) and description of what is returned. If the function returns nothing, explicitly state that.
5. **Raises / Throws / Errors** – any exceptions, error codes, or error‑handling behavior the function may produce. If none, you may omit this section.

Follow the exact syntax for the target language's doc‑comment style (e.g., `/** … */` for Java, `""" … """` for Python, `/// …` for Rust, `/** … */` for TypeScript/JSDoc, `/*** … */` for Doxygen, etc.).

Do **not** include any extra explanatory text outside the comment block.
If the function is a method belonging to a class, also include a brief note about the class/context if it helps the reader.

**Use only ASCII characters in the documentation comment.** Avoid using non-ASCII characters such as curly quotes (", ", ', '), 
em dashes (—), special bullets (•), or any Unicode symbols. Use standard ASCII alternatives: straight quotes (" and '), hyphens (-), 
asterisks (*), and other basic punctuation marks.

---

**Function source:**

```{{lang}}
{{function}}
```

---

Generate only the documentation comment (no surrounding code).

IMPORTANT: Write concisely and briefly. Use B2-level English: simple vocabulary, short sentences, no rare words or idioms.
)#";

const std::string PROMPT_GIT_COMMIT_MSG =
    R"#(Examine the current diff and write a git commit message that aligns with GitHub commit message style.

```diff
{{context}}
```

## Output
Produce **only** the commit message, in the exact format described above (title, blank line, body). Do not add any extra explanations, code blocks, or markdown.

IMPORTANT: Write concisely and briefly. Use B2-level English: simple vocabulary, short sentences, no rare words or idioms.
)#";

const std::string PROMPT_GIT_CODE_REVIEW =
    R"(You are an expert developer performing a code review.
Examine the current diff and provide a code review.
Your review should cover the following points:

1.  **Code quality and best practices:** Are standard patterns followed? Is the code readable? Is it adhering to the DRY (Don't Repeat Yourself) principle?
2.  **Potential bugs and logical errors:** Identify any obvious issues or edge cases that might cause problems.
3.  **Performance implications:** Flag any changes that might negatively impact performance.
4.  **Security vulnerabilities:** Point out any security risks introduced by the changes.
5.  **Maintainability:** Are the changes easy to understand and maintain over time?

Provide a concise summary at the beginning of your review. For each suggestion, use a GCC style format.
If you provide a code example for an improvement, include the filename where the change should be applied.

Example output:

```
Plugin/CustomControls/TextGenerationPreviewFrame.cpp:87: warning: potential memory leak?
Plugin/CustomControls/TextGenerationPreviewFrame.cpp:88: warning: make sure no buffer overflow.
```

IMPORTANT: Write concisely and briefly. Use B2-level English: simple vocabulary, short sentences, no rare words or idioms.
)";
} // namespace llm
