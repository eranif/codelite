#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include "clEditorEditEventsHandler.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "wxTerminalAnsiEscapeHandler.hpp"
#include "wxTerminalAnsiRendererSTC.hpp"
#include "wxTerminalColourHandler.h"

#include <wx/stc/stc.h>
#include <wx/textctrl.h>

class wxTerminalCtrl;
class wxTerminalInputCtrl;

class WXDLLIMPEXP_SDK wxTerminalOutputCtrl : public wxWindow
{
    struct IndicatorRange {
    private:
        int start = wxNOT_FOUND;
        int end = wxNOT_FOUND;

    public:
        int length() const { return end - start; }
        bool is_ok() const { return length() > 0; }
        int get_start() const { return start; }
        int get_end() const { return end; }
        void reset() { start = end = wxNOT_FOUND; }

        void set_range(int s, int e)
        {
            start = s;
            end = e;
        }

        bool operator==(const IndicatorRange& other) const
        {
            return this->start == other.start && this->end == other.end;
        }

        IndicatorRange& operator=(const IndicatorRange& other)
        {
            if(this == &other) {
                return *this;
            }
            this->start = other.start;
            this->end = other.end;
            return *this;
        }
        IndicatorRange(int s, int e)
            : start(s)
            , end(e)
        {
        }
        IndicatorRange() {}
    };

    wxStyledTextCtrl* m_ctrl = nullptr;
    wxTerminalAnsiEscapeHandler m_outputHandler;
    wxTerminalAnsiRendererSTC* m_stcRenderer = nullptr;

    wxEvtHandler* m_sink = nullptr;
    wxTextAttr m_defaultAttr;
    std::unordered_map<wxString, int> m_styles;
    int m_nextStyle = 0;
    wxFont m_textFont;
    wxColour m_bgColour;
    wxColour m_textColour;
    bool m_scrollToEndQueued = false;
    wxTerminalCtrl* m_terminal = nullptr;
    clEditEventsHandler::Ptr_t m_editEvents;
    IndicatorRange m_indicatorHyperlink;

protected:
    int GetCurrentStyle();
    void DoScrollToEnd();
    void RequestScrollToEnd();
    void OnThemeChanged(clCommandEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void ApplyTheme();
    void OnKeyDown(wxKeyEvent& event);
    void Initialise(const wxFont& font = wxNullFont, const wxColour& bg_colour = *wxBLACK,
                    const wxColour& text_colour = *wxWHITE);
    void ClearIndicators();
    void OnIdle(wxIdleEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    void DoPatternClicked(const wxString& pattern);

public:
    explicit wxTerminalOutputCtrl(wxTerminalCtrl* parent, wxWindowID winid = wxNOT_FOUND, const wxFont& font = wxNullFont,
                      const wxColour& bg_colour = *wxBLACK, const wxColour& text_colour = *wxWHITE);
    explicit wxTerminalOutputCtrl(wxWindow* parent, wxWindowID winid = wxNOT_FOUND);
    virtual ~wxTerminalOutputCtrl();
    void SetInputCtrl(wxTerminalInputCtrl* input_ctrl);
    wxStyledTextCtrl* GetCtrl() { return m_ctrl; }
    void SetSink(wxEvtHandler* sink) { this->m_sink = sink; }
    wxEvtHandler* GetSink() { return m_sink; }

    // API
    void AppendText(const wxString& buffer);
    void StyleAndAppend(wxStringView buffer, wxString* window_title);
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
