#include "TextView.h"
#include <wx/sizer.h>
#include "wxTerminalOptions.h"
#include <wx/wupdlock.h>

TextView::TextView(wxWindow* parent, wxWindowID winid)
    : wxWindow(parent, winid)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
#if USE_STC
    m_ctrl = new wxStyledTextCtrl(this, wxID_ANY);
    m_ctrl->SetCaretStyle(wxSTC_CARETSTYLE_BLOCK);
    m_ctrl->SetYCaretPolicy(wxSTC_CARET_STRICT | wxSTC_CARET_SLOP, 4);
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    m_ctrl->StartStyling(0);
    m_ctrl->SetWrapMode(wxSTC_WRAP_CHAR);
#else
    m_ctrl = new TextCtrl_t(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_ENTER | wxTE_NOHIDESEL | wxTE_PROCESS_TAB);
#endif

#if defined(__WXOSX__) && !USE_STC
    m_ctrl->OSXDisableAllSmartSubstitutions();
#endif

    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Fit(this);
    ReloadSettings();
    m_colourHandler.SetCtrl(this);
}

TextView::~TextView() {}

void TextView::AppendText(const wxString& buffer)
{
    m_ctrl->AppendText(buffer);
#if USE_STC
    m_ctrl->SetStyling(buffer.Length(), GetCurrentStyle());
#endif
}

long TextView::GetLastPosition() const { return m_ctrl->GetLastPosition(); }

wxString TextView::GetRange(int from, int to) const { return m_ctrl->GetRange(from, to); }

bool TextView::PositionToXY(long pos, long* x, long* y) const { return m_ctrl->PositionToXY(pos, x, y); }

long TextView::XYToPosition(long x, long y) const { return m_ctrl->XYToPosition(x, y); }

void TextView::Remove(long from, long to) { return m_ctrl->Remove(from, to); }

void TextView::SetInsertionPoint(long pos) { m_ctrl->SetInsertionPoint(pos); }

void TextView::SelectNone() { m_ctrl->SelectNone(); }

void TextView::SetInsertionPointEnd() { m_ctrl->SetInsertionPointEnd(); }

int TextView::GetNumberOfLines() const { return m_ctrl->GetNumberOfLines(); }

void TextView::SetDefaultStyle(const wxTextAttr& attr)
{
#if USE_STC
    m_defaultAttr = attr;
    m_ctrl->StartStyling(m_ctrl->GetLastPosition());
#else
    m_ctrl->SetDefaultStyle(attr);
#endif
}

wxTextAttr TextView::GetDefaultStyle() const
{
#if USE_STC
    return m_defaultAttr;
#else
    return m_ctrl->GetDefaultStyle();
#endif
}

bool TextView::IsEditable() const { return m_ctrl->IsEditable(); }

long TextView::GetInsertionPoint() const { return m_ctrl->GetInsertionPoint(); }

void TextView::Replace(long from, long to, const wxString& replaceWith) { m_ctrl->Replace(from, to, replaceWith); }

wxString TextView::GetLineText(int lineNumber) const { return m_ctrl->GetLineText(lineNumber); }

void TextView::SetEditable(bool b) { m_ctrl->SetEditable(b); }

void TextView::ReloadSettings()
{
#if USE_STC
    wxFont font = wxTerminalOptions::Get().GetFont();
    wxColour textColour = wxTerminalOptions::Get().GetTextColour();
    wxColour bgColour = wxTerminalOptions::Get().GetBgColour();
    SetBackgroundColour(bgColour);
    SetForegroundColour(textColour);
    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        m_ctrl->StyleSetBackground(i, bgColour);
        m_ctrl->StyleSetForeground(i, textColour);
        m_ctrl->StyleSetFont(i, font);
    }
    m_ctrl->SetCaretForeground(textColour);
    m_ctrl->SetBackgroundColour(bgColour);
    m_ctrl->SetForegroundColour(textColour);
    wxTextAttr defaultAttr = wxTextAttr(textColour, bgColour, font);
    SetDefaultStyle(defaultAttr);
    m_colourHandler.SetDefaultStyle(defaultAttr);
    m_ctrl->Refresh();
#else
    wxFont font = wxTerminalOptions::Get().GetFont();
    wxColour textColour = wxTerminalOptions::Get().GetTextColour();
    wxColour bgColour = wxTerminalOptions::Get().GetBgColour();
    SetBackgroundColour(bgColour);
    SetForegroundColour(textColour);
    m_ctrl->SetBackgroundColour(bgColour);
    m_ctrl->SetForegroundColour(textColour);
    wxTextAttr defaultAttr = wxTextAttr(textColour, bgColour, font);
    SetDefaultStyle(defaultAttr);
    m_colourHandler.SetDefaultStyle(defaultAttr);
    m_ctrl->Refresh();
#endif
}

void TextView::StyleAndAppend(const wxString& buffer) { m_colourHandler << buffer; }

void TextView::Focus() { m_ctrl->SetFocus(); }

void TextView::ShowCommandLine()
{
#if USE_STC
    m_ctrl->SetSelection(m_ctrl->GetLastPosition(), m_ctrl->GetLastPosition());
    m_ctrl->EnsureCaretVisible();
#else
    m_ctrl->SetInsertionPointEnd();
    m_ctrl->ShowPosition(m_ctrl->GetLastPosition());
#endif
}

void TextView::SetCommand(long from, const wxString& command)
{
#if USE_STC
    m_ctrl->SetSelection(from, GetLastPosition());
    m_ctrl->ReplaceSelection(command);
#else
    m_ctrl->Replace(from, GetLastPosition(), command);
#endif
}

void TextView::SetCaretEnd()
{
#if USE_STC
    m_ctrl->SetSelection(GetLastPosition(), GetLastPosition());
    m_ctrl->SetCurrentPos(GetLastPosition());
    m_ctrl->SetFocus();
#else
    SelectNone();
    SetInsertionPointEnd();
    CallAfter(&TextView::Focus);
#endif
}

int TextView::Truncate()
{
    if(GetNumberOfLines() > 1000) {
        // Start removing lines from the top
        long linesToRemove = (GetNumberOfLines() - 1000);
        long startPos = 0;
        long endPos = XYToPosition(0, linesToRemove);
        this->Remove(startPos, endPos);
        return endPos - startPos;
    }
    return 0;
}

wxChar TextView::GetLastChar() const
{
#if USE_STC
    return m_ctrl->GetCharAt(m_ctrl->GetLastPosition());
#else
    wxString text = m_ctrl->GetRange(m_ctrl->GetLastPosition() - 1, m_ctrl->GetLastPosition());
    return text[0];
#endif
}

int TextView::GetCurrentStyle()
{
#if USE_STC
#if 0
    wxString key;
    wxFont f = m_defaultAttr.GetFont();
    const wxColour& bg = m_defaultAttr.GetBackgroundColour();
    const wxColour& textColour = m_defaultAttr.GetTextColour();
    key << bg.GetAsString() << ":" << textColour.GetAsString() << ":" << (f.GetWeight() == wxFONTWEIGHT_BOLD);
    if(m_styles.count(key)) { return m_styles[key]; }
    ++m_nextStyle;

    m_ctrl->StyleSetBackground(m_nextStyle, bg);
    m_ctrl->StyleSetForeground(m_nextStyle, textColour);
    m_ctrl->StyleSetFont(m_nextStyle, f);
    return m_nextStyle;
#endif
    return 0;
#else
    return wxNOT_FOUND;
#endif
}

void TextView::Clear()
{
#if USE_STC
    m_ctrl->ClearAll();
    m_ctrl->ClearDocumentStyle();
    m_ctrl->StartStyling(0);
#endif
}
