#include "wxTerminalInputCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "wxTerminalCtrl.h"

#include <wx/bitmap.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>

wxTerminalInputCtrl::wxTerminalInputCtrl(wxTerminalCtrl* parent)
    : wxPanel(parent)
    , m_terminal(parent)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    GetSizer()->Add(m_ctrl, wxSizerFlags(1).Expand().Border(wxTOP, 2));
    m_ctrl->Bind(wxEVT_CHAR_HOOK, &wxTerminalInputCtrl::OnKeyDown, this);

    m_ctrl->AlwaysShowScrollbars(false, false);
    ApplyStyle();
    GetSizer()->Layout();
    m_editEvents.Reset(new clEditEventsHandler(m_ctrl));
}

wxTerminalInputCtrl::~wxTerminalInputCtrl() { m_ctrl->Unbind(wxEVT_CHAR_HOOK, &wxTerminalInputCtrl::OnKeyDown, this); }

void wxTerminalInputCtrl::OnKeyDown(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_NUMPAD_ENTER || event.GetKeyCode() == WXK_RETURN) {
        // Execute command
        wxString command = m_ctrl->GetText();
        m_terminal->Run(command);
        m_history.Add(command);
        m_ctrl->ClearAll();

    } else if(event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_NUMPAD_UP) {
        m_history.Up();
        m_ctrl->SetText(m_history.Get());
        SetCaretEnd();

    } else if(event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_NUMPAD_DOWN) {
        m_history.Down();
        m_ctrl->SetText(m_history.Get());
        SetCaretEnd();

    } else if((event.GetKeyCode() == 'C') && event.RawControlDown()) {
        // Generate Ctrl-C
        m_terminal->GenerateCtrlC();

    } else if((event.GetKeyCode() == 'L') && event.RawControlDown()) {
        m_terminal->ClearScreen();

    } else if((event.GetKeyCode() == 'U') && event.RawControlDown()) {
        m_terminal->ClearLine();
        SetCaretEnd();

    } else if((event.GetKeyCode() == 'D') && event.RawControlDown()) {
        m_terminal->Logout();

    } else if(event.GetKeyCode() == WXK_TAB) {
        // block it
    } else {
        event.Skip();
    }
}

void wxTerminalInputCtrl::SetCaretEnd()
{
    m_ctrl->SetSelection(m_ctrl->GetLastPosition(), m_ctrl->GetLastPosition());
    m_ctrl->SetCurrentPos(m_ctrl->GetLastPosition());
    m_ctrl->SetFocus();
}

void wxTerminalInputCtrl::Clear() { m_ctrl->ClearAll(); }

void wxTerminalInputCtrl::ApplyStyle()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->ApplySystemColours(m_ctrl);

    wxBitmap bmp;
    bmp.CreateWithDIPSize(wxSize(1, 1), GetDPIScaleFactor());
    wxMemoryDC memDC(bmp);
    wxGCDC gcdc(memDC);

    auto font = lexer->GetFontForStyle(0, m_ctrl);
    gcdc.SetFont(font);
    wxSize textSize = gcdc.GetTextExtent("Tp");
    SetSizeHints(wxNOT_FOUND, textSize.GetHeight() + 2);
}