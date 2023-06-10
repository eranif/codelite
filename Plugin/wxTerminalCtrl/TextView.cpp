#include "TextView.h"

#include "FontUtils.hpp"

#include <wx/sizer.h>
#include <wx/wupdlock.h>

TextView::TextView(wxWindow* parent, wxWindowID winid, const wxFont& font, const wxColour& bg_colour,
                   const wxColour& text_colour)
    : wxWindow(parent, winid)
{
    m_textFont = font.IsOk() ? font : FontUtils::GetDefaultMonospacedFont();
    m_textColour = text_colour;
    m_bgColour = bg_colour;

    SetSizer(new wxBoxSizer(wxVERTICAL));
#if USE_STC
    m_ctrl = new wxStyledTextCtrl(this, wxID_ANY);
    m_ctrl->SetCaretStyle(wxSTC_CARETSTYLE_BLOCK);

    int caretSlop = 1;
    int caretZone = 20;
    int caretStrict = 0;
    int caretEven = 0;
    int caretJumps = 0;

    m_ctrl->SetXCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

    caretSlop = 1;
    caretZone = 1;
    caretStrict = 4;
    caretEven = 8;
    caretJumps = 0;
    m_ctrl->SetYCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    m_ctrl->StartStyling(0);
#else
    m_ctrl = new TextCtrl_t(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_RICH2 | wxTE_PROCESS_ENTER | wxTE_NOHIDESEL | wxTE_PROCESS_TAB);
#endif

#if defined(__WXOSX__) && !USE_STC
    m_ctrl->OSXDisableAllSmartSubstitutions();
#endif

    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Fit(this);
    m_colourHandler.SetCtrl(this);
    CallAfter(&TextView::ReloadSettings);
}

TextView::~TextView() {}

void TextView::AppendText(const std::string& buffer)
{
    m_ctrl->AppendText(buffer);
#if USE_STC
    m_ctrl->SetStyling(buffer.length(), GetCurrentStyle());
#endif
    RequestScrollToEnd();
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
    SetBackgroundColour(m_bgColour);
    SetForegroundColour(m_textColour);
    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        m_ctrl->StyleSetBackground(i, m_bgColour);
        m_ctrl->StyleSetForeground(i, m_textColour);
        m_ctrl->StyleSetFont(i, m_textFont);
    }
    m_ctrl->SetCaretForeground(m_textColour);
    m_ctrl->SetBackgroundColour(m_bgColour);
    m_ctrl->SetForegroundColour(m_textColour);
    wxTextAttr defaultAttr = wxTextAttr(m_textColour, m_bgColour, m_textFont);
    SetDefaultStyle(defaultAttr);
    m_colourHandler.SetDefaultStyle(defaultAttr);
    m_ctrl->Refresh();
#else
    SetBackgroundColour(m_bgColour);
    SetForegroundColour(m_textColour);
    m_ctrl->SetBackgroundColour(m_bgColour);
    m_ctrl->SetForegroundColour(m_textColour);
    wxTextAttr defaultAttr = wxTextAttr(m_textColour, m_bgColour, m_textFont);
    SetDefaultStyle(defaultAttr);
    m_colourHandler.SetDefaultStyle(defaultAttr);
    m_ctrl->Refresh();
#endif
}

void TextView::StyleAndAppend(const std::string& buffer) { m_colourHandler << buffer; }

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
    RequestScrollToEnd();
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

void TextView::DoScrollToEnd()
{
    m_scrollToEndQueued = false;
#if USE_STC
    m_ctrl->ScrollToEnd();
#endif
}

void TextView::RequestScrollToEnd()
{
    if(m_scrollToEndQueued) {
        return;
    }
    m_scrollToEndQueued = true;
    CallAfter(&TextView::DoScrollToEnd);
}