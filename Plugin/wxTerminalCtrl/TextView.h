#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include "clEditorEditEventsHandler.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wxTerminalColourHandler.h"

#include <wx/stc/stc.h>
#include <wx/textctrl.h>

class wxTerminalCtrl;
class WXDLLIMPEXP_SDK TextView : public wxWindow
{
    wxStyledTextCtrl* m_ctrl = nullptr;
    wxTerminalColourHandler m_colourHandler;
    wxEvtHandler* m_sink = nullptr;
    wxTextAttr m_defaultAttr;
    std::unordered_map<wxString, int> m_styles;
    int m_nextStyle = 0;
    wxFont m_textFont;
    wxColour m_bgColour;
    wxColour m_textColour;
    bool m_scrollToEndQueued = false;
    wxTerminalCtrl* m_terminal = nullptr;

protected:
    int GetCurrentStyle();
    void DoScrollToEnd();
    void RequestScrollToEnd();
    void OnThemeChanged(clCommandEvent& event);
    void ApplyTheme();
    void OnKeyDown(wxKeyEvent& event);
    void OnMenu(wxContextMenuEvent& event);

public:
    TextView(wxTerminalCtrl* parent, wxWindowID winid = wxNOT_FOUND, const wxFont& font = wxNullFont,
             const wxColour& bg_colour = *wxBLACK, const wxColour& text_colour = *wxWHITE);
    virtual ~TextView();
    wxStyledTextCtrl* GetCtrl() { return m_ctrl; }
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
    void Replace(long from, long to, const wxString& replaceWith);
    wxString GetLineText(int lineNumber) const;
    void ReloadSettings();
    void ShowCommandLine();
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
