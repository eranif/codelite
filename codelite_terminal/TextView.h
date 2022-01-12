#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include "codelite_exports.h"
#include "wxTerminalColourHandler.h"

#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include <wxStringHash.h>

#ifdef __WXGTK3__
#define USE_STC 1
#else
#define USE_STC 0
#endif

#if USE_STC
typedef wxStyledTextCtrl TextCtrl_t;
#else
typedef wxTextCtrl TextCtrl_t;
#endif

class WXDLLIMPEXP_SDK TextView : public wxWindow
{
    TextCtrl_t* m_ctrl = nullptr;
    wxTerminalColourHandler m_colourHandler;
    wxEvtHandler* m_sink = nullptr;
    wxTextAttr m_defaultAttr;
    std::unordered_map<wxString, int> m_styles;
    int m_nextStyle = 0;

protected:
    int GetCurrentStyle();

public:
    TextView(wxWindow* parent, wxWindowID winid = wxNOT_FOUND);
    virtual ~TextView();
    void Focus();
    wxWindow* GetCtrl() { return m_ctrl; }
    void SetSink(wxEvtHandler* sink) { this->m_sink = sink; }
    wxEvtHandler* GetSink() { return m_sink; }

    // API
    void AppendText(const wxString& buffer);
    void StyleAndAppend(const wxString& buffer);
    long GetLastPosition() const;
    wxString GetRange(int from, int to) const;
    bool PositionToXY(long pos, long* x, long* y) const;
    long XYToPosition(long x, long y) const;
    void Remove(long from, long to);
    void SetInsertionPoint(long pos);
    void SetInsertionPointEnd();
    long GetInsertionPoint() const;
    void SelectNone();
    int GetNumberOfLines() const;
    void SetDefaultStyle(const wxTextAttr& attr);
    wxTextAttr GetDefaultStyle() const;
    bool IsEditable() const;
    void SetEditable(bool b);
    void Replace(long from, long to, const wxString& replaceWith);
    wxString GetLineText(int lineNumber) const;
    void ReloadSettings();
    void ShowCommandLine();
    void SetCommand(long from, const wxString& command);
    void SetCaretEnd();
    int Truncate();
    wxChar GetLastChar() const;
    void Clear();
};

#endif // TEXTVIEW_H
