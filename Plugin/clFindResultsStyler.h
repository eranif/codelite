#ifndef CLFINDRESULTSSTYLER_H
#define CLFINDRESULTSSTYLER_H

#include "codelite_exports.h"
#include "smart_ptr.h"

#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clFindResultsStyler : public wxEvtHandler
{
public:
    enum eState {
        kHeader,
        kStartOfLine,
        kFile,
        kLineNumber,
        kScope,
        kMatch,
    };

    enum {
        // Custom styles
        LEX_FIF_DEFAULT = 0,
        LEX_FIF_FILE,
        LEX_FIF_MATCH,
        LEX_FIF_LINE_NUMBER,
        LEX_FIF_HEADER,
        LEX_FIF_SCOPE,
        LEX_FIF_MATCH_COMMENT,
    };
    typedef SmartPtr<clFindResultsStyler> Ptr_t;

protected:
    wxStyledTextCtrl* m_stc;
    eState m_curstate;

protected:
    void OnStyleNeeded(wxStyledTextEvent& e);

public:
    clFindResultsStyler();
    clFindResultsStyler(wxStyledTextCtrl* stc);
    virtual ~clFindResultsStyler();

    /**
     * @brief style the control text
     */
    virtual void StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasSope);

    /**
     * @brief reset the styler
     */
    virtual void Reset();

    /**
     * @brief initialize the control styles and colours
     */
    virtual void SetStyles(wxStyledTextCtrl* ctrl);

    /**
     * @brief user clicked on the editor, return the matching style (LEX_FIF_*)
     * @return return the clicked line + the style on that line
     */
    virtual int HitTest(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, int& line);

    /**
     * @brief same as above, but uses the wxSTC passed in the Ctor
     */
    virtual int HitTest(wxStyledTextEvent& e, int& line);

    /**
     * @brief check whether a "Togglable" style was clicked
     * @return return the line number that was clicked if the style on that line is a togglable style
     * otherwise return wxNOT_FOUND
     */
    virtual int TestToggle(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e);
};

#endif // CLFINDRESULTSSTYLER_H
