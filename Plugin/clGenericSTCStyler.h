#ifndef CLGENERICSTCSTYLER_H
#define CLGENERICSTCSTYLER_H

#include "codelite_exports.h"

#include <memory>
#include <tuple>
#include <vector>
#include <wx/event.h>
#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clGenericSTCStyler : public wxEvtHandler
{
    wxStyledTextCtrl* m_ctrl;
    std::vector<std::pair<wxString, int>> m_words;
    std::vector<std::tuple<int, wxColour, wxColour>> m_styleInfo;
    int m_nextAvailStyle;

public:
    enum eStyles { kDefault = 0, kInfo, kWarning, kError, kLastStyle };

protected:
    void OnStyleNeeded(wxStyledTextEvent& event);
    void OnThemChanged(wxCommandEvent& event);
    bool GetNextLine(const wxString& inText, wxString& lineText) const;
    int GetStyleForLine(const wxString& lineText) const;
    void ResetStyles();
    void InitDefaultStyles();

public:
    clGenericSTCStyler(wxStyledTextCtrl* stc);
    virtual ~clGenericSTCStyler();

    using Ptr_t = std::shared_ptr<clGenericSTCStyler>;
    /**
     * @brief register keywords for a known style
     */
    void AddStyle(const wxArrayString& words, clGenericSTCStyler::eStyles style);
    /**
     * @brief register a user style for a given phrases
     */
    void AddUserStyle(const wxArrayString& words, const wxColour& fgColour, const wxColour& bgColour = wxColour());

    /**
     * @brief apply the user provided styles
     */
    void ApplyStyles();

    /**
     * @brief clear the current styles
     */
    void ClearAllStyles();
};

#endif // CLGENERICSTCSTYLER_H
