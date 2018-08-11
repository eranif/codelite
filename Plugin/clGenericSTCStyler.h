#ifndef CLGENERICSTCSTYLER_H
#define CLGENERICSTCSTYLER_H

#include "codelite_exports.h"
#include <tuple>
#include <vector>
#include <algorithm>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clGenericSTCStyler : public wxEvtHandler
{
    wxStyledTextCtrl* m_ctrl;
    std::vector<std::pair<wxString, int> > m_words;
    std::vector<std::tuple<int, wxColour, wxColour> > m_styleInfo;
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

    typedef wxSharedPtr<clGenericSTCStyler> Ptr_t;
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
