#pragma once

#include "clSTCContainerStylerBase.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <stack>
#include <wx/event.h>
#include <wx/stc/stc.h>

enum MarkdownStyles {
    kDefault,
    kHeader1,
    kHeader2,
    kHeader3,
    kHeader4,
    kHeader5,
    kHeader6,
    kHeaderText,
    kCodeBlockTag,
    kCodeBlockText,
    kCodeWord,
    kStrongText,
    kStrong2Text,
    kStrongTag,
    kStrong2Tag,
    kEmphasisText,
    kEmphasis2Text,
    kEmphasisTag,
    kEmphasis2Tag,
    kBacktick,
    kHorizontalLine,
    kListItem,
    kNumberedListItem,
    kNumberedListItemDot,
    kUrl,
    kCodeBlockKeyword,
    kCodeBlockString,
    kCodeBlockNumber,
    kCodeBlockFunction,
    kCodeBlockOperator,
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