#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include "wxTerminalColourHandler.h"

#include <wx/stc/stc.h>
#include <wx/textctrl.h>

class TextView : public wxWindow
{
    TextCtrl_t* m_ctrl = nullptr;
    wxTerminalColourHandler m_colourHandler;
    wxEvtHandler* m_sink = nullptr;
    wxTextAttr m_defaultAttr;
    std::unordered_map<wxString, int> m_styles;
    int m_nextStyle = 0;
    wxFont m_textFont;
    wxColour m_bgColour;
    wxColour m_textColour;
    bool m_scrollToEndQueued = false;

protected:
    int GetCurrentStyle();
    void DoScrollToEnd();
    void RequestScrollToEnd();

public:
    TextView(wxWindow* parent, wxWindowID winid = wxNOT_FOUND, const wxFont& font = wxNullFont,
             const wxColour& bg_colour = *wxBLACK, const wxColour& text_colour = *wxWHITE);
    virtual ~TextView();
    void Focus();
    TextCtrl_t* GetCtrl() { return m_ctrl; }
    void SetSink(wxEvtHandler* sink) { this->m_sink = sink; }
    wxEvtHandler* GetSink() { return m_sink; }

    // API
    void AppendText(const std::string& buffer);
    void StyleAndAppend(const std::string& buffer);
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
    void SetAttributes(const wxColour& bg_colour, const wxColour& text_colour, const wxFont& font)
    {
        m_textColour = text_colour;
        m_bgColour = bg_colour;
        m_textFont = font;
    }
};

#endif // TEXTVIEW_H
