#pragma once

#include "clSTCContainerStylerBase.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <stack>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <wx/event.h>
#include <wx/stc/stc.h>

enum MarkdownStyles {
    kDefault,             // 0
    kHeader1,             // 1
    kHeader2,             // 2
    kHeader3,             // 3
    kHeader4,             // 4
    kHeader5,             // 5
    kHeader6,             // 6
    kHeaderText,          // 7
    kCodeBlockTag,        // 8
    kCodeBlockText,       // 9
    kCodeWord,            // 10
    kStrongText,          // 11
    kStrong2Text,         // 12
    kStrongTag,           // 13
    kStrong2Tag,          // 14
    kEmphasisText,        // 15
    kEmphasis2Text,       // 16
    kEmphasisTag,         // 17
    kEmphasis2Tag,        // 18
    kBacktick,            // 19
    kHorizontalLine,      // 20
    kListItem,            // 21
    kNumberedListItem,    // 22
    kNumberedListItemDot, // 23
    kUrl,                 // 24
    kActor,               // 25
    kKeyboardKey,         // 26
    kCodeBlockKeyword,    // 27
    kCodeBlockString,     // 28
    kCodeBlockNumber,     // 29
    kCodeBlockFunction,   // 30
    kCodeBlockOperator,   // 31
    // Skip to 40 since 32-39 are used internally
    kCodeBlockMacro = 40, // 40
    kCodeBlockComment,    // 41
    kDiffAdd,             // 42
    kDiffDelete,          // 43
    kKeyboardKeyBracket,  // 44
    kCheckMarkSymbol,     // 45
    kErrorSymbol,         // 46
    kDiffHeader,          // 47
    kDiffFileMarker,      // 48
};

enum class MarkdownState {
    kDefault,
    kCodeBlock3,
    kCodeBlock4,
    kCodeBlock5,
    kCodeBlockContent,
    kCodeBlockTag,
    kCodeWord,
    kStrong2Text,
    kEmphasis2Text,
    kHeaderText,
    kUrl,
};

class WXDLLIMPEXP_SDK MarkdownStyler : public clSTCContainerStylerBase
{
public:
    MarkdownStyler(wxStyledTextCtrl* ctrl);
    ~MarkdownStyler() override;
    void InitStyles() override;
    void Reset() override;
    wxString GetUrlFromPosition(int pos);

    /// Style all the text of the accessor, starting with the default state
    void OnStyle(AccessorBase& accessor);
    void OnHostspotClicked(wxStyledTextEvent& event);

protected:
    /// Line by line styling (see clSTCContainerStylerBase). Only a fenced code block continues on the next line, so
    /// the state of a line is the fence and the language of the code block.
    void RestoreLineState(int state) override;
    void StyleLine(AccessorBase& accessor) override;
    int SaveLineState() override;

private:
    /// The language of a fenced code block, with the values that styling needs. They are computed once for each tag.
    struct CodeBlockLanguage {
        wxString name; // the normalised name, e.g. "c++"
        const std::unordered_set<wxString>* keywords{nullptr};
        bool is_plain_text{true};
        bool is_diff{false};
        bool is_rust{false};
        bool cpp_comments{false};
        bool hash_comments{false};
    };

    /// Style until the end of the text of the accessor, starting with the current state
    void StyleRange(AccessorBase& accessor);

    /**
     * @brief Style code block content with syntax highlighting
     * @param accessor The text accessor
     * @param language The language of the code block
     */
    void StyleCodeBlockContent(AccessorBase& accessor, const CodeBlockLanguage& language);
    bool StyleDiffPatchCommentInCodeBlock(AccessorBase& accessor);

    /**
     * @brief Check if a word is a keyword for the given language
     */
    bool IsKeyword(const wxString& word, const CodeBlockLanguage& language) const;

    /**
     * @brief Check if a character is an operator
     */
    bool IsOperator(wxChar ch) const;

    /// Return the id of the language of a code block tag (0 when there is no tag). The line state holds this id.
    int GetLanguageId(const wxString& tag);
    const CodeBlockLanguage& GetCurrentLanguage() const;
    /// The tag of the fence line (e.g. "cpp" in "```cpp") is complete
    void EndCodeBlockTag();

    /// At the start of a line of a code block: if the line is the closing `fence` (it can be indented), style it, end
    /// the code block and return true
    bool StyleClosingFence(AccessorBase& accessor, std::string_view fence);

    /// Style a line that is indented by 4 columns or more as an indented code block, if the rules allow it. Return
    /// true if it did
    bool StyleIndentedCodeLine(AccessorBase& accessor, size_t indent_columns, bool is_blank);
    bool IsInFencedCodeBlock() const;

    std::stack<MarkdownState> m_states;
    wxString m_currentCodeBlockLanguage; // the tag, while it is read
    int m_currentLanguageId{0};

    // For the indented code blocks (line by line styling only)
    bool m_prevLineBlank{false};
    bool m_prevLineIndentedCode{false};
    bool m_inList{false};                       // an indented line in a list continues the list item, it is not code
    std::vector<CodeBlockLanguage> m_languages; // the language with the id `N` is at index `N - 1`
    std::unordered_map<wxString, int> m_languageIds;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_MARKDOWN_LINK_CLICKED, clCommandEvent);