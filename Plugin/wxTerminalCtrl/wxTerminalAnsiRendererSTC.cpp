#include "wxTerminalAnsiRendererSTC.hpp"

#include "clAnsiEscapeCodeHandler.hpp"

namespace
{
struct EditorEnabler {
    wxStyledTextCtrl* m_ctrl = nullptr;
    EditorEnabler(wxStyledTextCtrl* ctrl)
        : m_ctrl(ctrl)
    {
        m_ctrl->SetEditable(true);
    }
    ~EditorEnabler() { m_ctrl->SetEditable(false); }
};
} // namespace

wxTerminalAnsiRendererSTC::wxTerminalAnsiRendererSTC(wxStyledTextCtrl* ctrl)
    : m_ctrl(ctrl)
{
    m_ctrl->SetVirtualSpaceOptions(wxSTC_VS_USERACCESSIBLE);
    m_stcStyleProvider = new wxSTCStyleProvider(m_ctrl);
    m_defaultAttr = m_stcStyleProvider->GetDefaultStyle();
}

wxTerminalAnsiRendererSTC::~wxTerminalAnsiRendererSTC() { wxDELETE(m_stcStyleProvider); }

void wxTerminalAnsiRendererSTC::Bell() {}
void wxTerminalAnsiRendererSTC::Backspace() {}

void wxTerminalAnsiRendererSTC::InsertText(const wxString& str)
{
    EditorEnabler d{ m_ctrl };
    SetInsertionPoint();
    int curpos = m_ctrl->GetCurrentPos();
    m_ctrl->InsertText(curpos, str);
    int newpos = curpos + str.length();

    // style the newly added text
    m_ctrl->StartStyling(curpos);
    m_ctrl->SetStyling(str.length(), m_curstyle);

    m_pos.y = m_ctrl->LineFromPosition(newpos);
    m_pos.x = newpos - m_ctrl->PositionFromLine(newpos);
    m_ctrl->ScrollToEnd();
}

void wxTerminalAnsiRendererSTC::Tab() { InsertText("\t"); }

void wxTerminalAnsiRendererSTC::LineFeed() { InsertText("\n"); }

void wxTerminalAnsiRendererSTC::FormFeed()
{
    EditorEnabler d{ m_ctrl };
    m_pos = { 0, 0 };
    SetInsertionPoint();
    m_ctrl->ClearAll();
}

void wxTerminalAnsiRendererSTC::CarriageReturn()
{
    EditorEnabler d{ m_ctrl };
    m_pos.x = 0;
    SetInsertionPoint();
}

void wxTerminalAnsiRendererSTC::AddString(wxStringView str) { InsertText(wxString(str.data(), str.length())); }

void wxTerminalAnsiRendererSTC::MoveCaret(long n, wxDirection direction)
{
    switch(direction) {
    case wxRIGHT:
        m_pos.x += n;
        break;
    case wxLEFT:
        m_pos.x -= n;
        if(m_pos.x < 0) {
            m_pos.x = 0;
        }
        break;
    case wxUP:
        m_pos.y -= n;
        if(m_pos.y < 0) {
            m_pos.y = 0;
        }
        break;
    case wxDOWN:
        m_pos.y += n;
        if(m_pos.y >= m_ctrl->GetLineCount()) {
            m_pos.y = m_ctrl->GetLineCount() - 1;
        }
        break;
    default:
        break;
    }
}

void wxTerminalAnsiRendererSTC::SetCaretX(long n) { m_pos.x = wxMax(n - 1, 0); }

void wxTerminalAnsiRendererSTC::SetCaretY(long n) { m_pos.y = wxMax(n - 1, 0); }

void wxTerminalAnsiRendererSTC::ClearLine(size_t dir)
{
    EditorEnabler d{ m_ctrl };
    if(dir & wxRIGHT && dir & wxLEFT) {
        // clear entire line
        SetInsertionPoint();
        int curpos = m_ctrl->GetCurrentPos();
        int curline = m_ctrl->LineFromPosition(curpos);
        int start_pos = m_ctrl->PositionFromLine(curline);
        m_ctrl->DeleteRange(start_pos, curpos - start_pos);
        m_pos.x = 0;
        SetInsertionPoint();

    } else if(dir & wxRIGHT) {
        SetInsertionPoint();
        int curpos = m_ctrl->GetCurrentPos();
        int curline = m_ctrl->LineFromPosition(curpos);
        int end_pos = m_ctrl->LineLength(curline);
        m_ctrl->DeleteRange(curpos, end_pos - curpos);

    } else if(dir & wxLEFT) {
        SetInsertionPoint();
        int curpos = m_ctrl->GetCurrentPos();
        int curline = m_ctrl->LineFromPosition(curpos);
        int start_pos = m_ctrl->PositionFromLine(curline);
        m_ctrl->DeleteRange(start_pos, curpos - start_pos);
    }
}

void wxTerminalAnsiRendererSTC::ClearDisplay(size_t dir)
{
    EditorEnabler d{ m_ctrl };
    if(dir & wxUP && dir & wxDOWN) {
        FormFeed();
    }
}

void wxTerminalAnsiRendererSTC::SetWindowTitle(wxStringView window_title)
{
    m_windowTitle = wxString(window_title.data(), window_title.length());
}

int wxTerminalAnsiRendererSTC::GetInsertionPoint() const
{
    int pos = m_ctrl->PositionFromLine(m_pos.y);
    pos += m_pos.x;
    return pos;
}

void wxTerminalAnsiRendererSTC::SetInsertionPoint()
{
    int pos = GetInsertionPoint();
    m_ctrl->SetSelection(pos, pos);
    m_ctrl->SetCurrentPos(pos);
}

void wxTerminalAnsiRendererSTC::ResetStyle()
{
    m_defaultAttr = m_stcStyleProvider->GetDefaultStyle();
    m_curAttr = m_defaultAttr;
    m_curstyle = 0;
}

void wxTerminalAnsiRendererSTC::SetTextColour(const wxColour& col)
{
    wxTerminalAnsiRendererInterface::SetTextColour(col);
    m_curstyle = m_stcStyleProvider->GetStyle(m_curAttr.GetTextColour(), m_curAttr.GetBackgroundColour());
}

void wxTerminalAnsiRendererSTC::SetTextBgColour(const wxColour& col)
{
    wxUnusedVar(col);
    // wxTerminalAnsiRendererInterface::SetTextBgColour(col);
    // m_curstyle = m_stcStyleProvider->GetStyle(m_curAttr.GetTextColour(), m_curAttr.GetBackgroundColour());
}

void wxTerminalAnsiRendererSTC::SetTextFont(const wxFont& font) { wxTerminalAnsiRendererInterface::SetTextFont(font); }

void wxTerminalAnsiRendererSTC::Clear()
{
    wxTerminalAnsiRendererInterface::Clear();
    m_curstyle = 0;
    m_stcStyleProvider->Clear();
}