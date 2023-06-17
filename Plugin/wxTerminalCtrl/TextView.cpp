#include "TextView.h"

#include "ColoursAndFontsManager.h"
#include "FontUtils.hpp"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "wxTerminalCtrl.h"

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/wupdlock.h>

TextView::TextView(wxTerminalCtrl* parent, wxWindowID winid, const wxFont& font, const wxColour& bg_colour,
                   const wxColour& text_colour)
    : wxWindow(parent, winid)
    , m_terminal(parent)
{
    m_textFont = font.IsOk() ? font : FontUtils::GetDefaultMonospacedFont();
    m_textColour = text_colour;
    m_bgColour = bg_colour;

    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    m_ctrl->UsePopUp(0);
    m_ctrl->SetWrapMode(wxSTC_WRAP_WORD);
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    m_ctrl->StartStyling(0);

    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Fit(this);
    m_colourHandler.SetCtrl(this);
    CallAfter(&TextView::ReloadSettings);

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &TextView::OnThemeChanged, this);
    m_ctrl->Bind(wxEVT_CHAR_HOOK, &TextView::OnKeyDown, this);
}

TextView::~TextView()
{
    m_ctrl->Unbind(wxEVT_CHAR_HOOK, &TextView::OnKeyDown, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &TextView::OnThemeChanged, this);
}

void TextView::AppendText(const wxString& buffer)
{
    m_ctrl->AppendText(buffer);
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

void TextView::SetDefaultStyle(const wxTextAttr& attr) { m_defaultAttr = attr; }

wxTextAttr TextView::GetDefaultStyle() const { return m_defaultAttr; }

long TextView::GetInsertionPoint() const { return m_ctrl->GetInsertionPoint(); }

void TextView::Replace(long from, long to, const wxString& replaceWith) { m_ctrl->Replace(from, to, replaceWith); }

wxString TextView::GetLineText(int lineNumber) const { return m_ctrl->GetLineText(lineNumber); }

void TextView::ReloadSettings() { ApplyTheme(); }

void TextView::StyleAndAppend(const wxString& buffer, wxString* window_title)
{
    m_colourHandler.Append(buffer, window_title);
}

void TextView::ShowCommandLine()
{
    m_ctrl->SetSelection(m_ctrl->GetLastPosition(), m_ctrl->GetLastPosition());
    m_ctrl->EnsureCaretVisible();
    RequestScrollToEnd();
}

void TextView::SetCaretEnd()
{
    m_ctrl->SelectNone();
    m_ctrl->SetSelection(GetLastPosition(), GetLastPosition());
    m_ctrl->SetCurrentPos(GetLastPosition());
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

wxChar TextView::GetLastChar() const { return m_ctrl->GetCharAt(m_ctrl->GetLastPosition() - 1); }

int TextView::GetCurrentStyle() { return 0; }

void TextView::Clear() { m_colourHandler.Clear(); }

void TextView::DoScrollToEnd()
{
    m_scrollToEndQueued = false;
    m_ctrl->ScrollToEnd();
}

void TextView::RequestScrollToEnd()
{
    if(m_scrollToEndQueued) {
        return;
    }
    m_scrollToEndQueued = true;
    CallAfter(&TextView::DoScrollToEnd);
}

void TextView::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

void TextView::ApplyTheme()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_ctrl);

    auto style = lexer->GetProperty(0);
    wxTextAttr defaultAttr = wxTextAttr(style.GetFgColour(), style.GetBgColour(), lexer->GetFontForStyle(0, m_ctrl));
    SetDefaultStyle(defaultAttr);

    m_colourHandler.SetDefaultStyle(defaultAttr);
    m_ctrl->Refresh();
}

void TextView::OnKeyDown(wxKeyEvent& event)
{
    // let the input control process this event
    m_terminal->GetInputCtrl()->ProcessKeyDown(event);
}
