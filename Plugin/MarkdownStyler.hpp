#pragma once

#include "clSTCContainerStylerBase.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <stack>
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
    kCodeBlockKeyword,    // 25
    kCodeBlockString,     // 26
    kCodeBlockNumber,     // 27
    kCodeBlockFunction,   // 28
    kCodeBlockOperator,   // 29
    kCodeBlockMacro,      // 30
    kCodeBlockComment,    // 31
    // Skip to 40 since 32-39 are used internally
    kDiffAdd = 40,        // 40
    kDiffDelete,          // 42
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
    virtual ~MarkdownStyler();
    void InitStyles() override;
    void Reset() override;
    wxString GetUrlFromPosition(int pos);

private:
    void OnStyle(clSTCAccessor& accessor);
    void OnHostspotClicked(wxStyledTextEvent& event);

    /**
     * @brief Style code block content with syntax highlighting
     * @param accessor The text accessor
     * @param language The language tag (e.g., "c++", "python", "javascript")
     */
    void StyleCodeBlockContent(clSTCAccessor& accessor, const wxString& language);

    /**
     * @brief Check if a word is a keyword for the given language
     */
    bool IsKeyword(const wxString& word, const wxString& language) const;

    /**
     * @brief Check if a character is an operator
     */
    bool IsOperator(wxChar ch) const;

    std::stack<MarkdownState> m_states;
    wxString m_currentCodeBlockLanguage;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_MARKDOWN_LINK_CLICKED, clCommandEvent);