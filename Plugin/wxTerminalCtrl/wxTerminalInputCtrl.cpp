#include "wxTerminalInputCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "wxTerminalCtrl.h"

#include <wx/bitmap.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>

namespace
{
class MyEventsHandler : public clEditEventsHandler
{
public:
    MyEventsHandler(wxStyledTextCtrl* ctrl)
        : clEditEventsHandler(ctrl)
    {
    }

    void OnPaste(wxCommandEvent& event) override
    {
        CHECK_FOCUS_WINDOW();
        int where = m_stc->GetLastPosition();
        m_stc->SetSelection(where, where);
        m_stc->SetCurrentPos(where);
        clEditEventsHandler::OnPaste(event);
    }
};
} // namespace
wxTerminalInputCtrl::wxTerminalInputCtrl(wxTerminalCtrl* parent, wxStyledTextCtrl* ctrl)
    : m_terminal(parent)
    , m_ctrl(ctrl)
{
    m_editEvents.Reset(new MyEventsHandler(m_ctrl));
}

wxTerminalInputCtrl::~wxTerminalInputCtrl() {}

#define CAN_EDIT() (m_ctrl->GetCurrentPos() > m_writeStartingPosition)
#define CAN_DELETE() (m_ctrl->GetCurrentPos() >= m_writeStartingPosition)
void wxTerminalInputCtrl::ProcessKeyDown(wxKeyEvent& event)
{
    if(event.RawControlDown() && event.GetKeyCode() == 'C') {
        m_terminal->GenerateCtrlC();
        return;
    } else if(event.RawControlDown() && event.GetKeyCode() == 'D') {
        m_terminal->Logout();
        return;
    } else if(event.RawControlDown() && event.GetKeyCode() == 'L') {
        m_terminal->ClearScreen();
        return;
    } else if(event.RawControlDown() && event.GetKeyCode() == 'U') {
        Clear();
        return;
    }

    switch(event.GetKeyCode()) {
    case WXK_NUMPAD_ENTER:
    case WXK_RETURN: {
        wxString command = GetText();
        m_terminal->Run(command);
        m_history.Add(command);
        event.Skip();
    } break;
    case WXK_UP:
    case WXK_NUMPAD_UP:
        m_history.Up();
        SetText(m_history.Get());
        break;
    case WXK_DOWN:
    case WXK_NUMPAD_DOWN:
        m_history.Down();
        SetText(m_history.Get());
        break;
    case WXK_TAB:
        // m_terminal->SendTab();
        return;
    case WXK_LEFT:
    case WXK_NUMPAD_LEFT:
    case WXK_BACK:
        if(CAN_EDIT()) {
            event.Skip();
        }
        break;
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
        if(CAN_DELETE()) {
            event.Skip();
        }
        break;
    case WXK_RIGHT:
    case WXK_NUMPAD_RIGHT:
        event.Skip();
        break;
    case WXK_HOME:
        SetCaretPos(CaretPos::HOME);
        break;
    default: {
        // all other cases: if the position is not within the write area,
        // move the caret to write area and continue
        if(!CAN_EDIT()) {
            SetCaretPos(CaretPos::END);
        }
        event.Skip();
    } break;
    }
}

void wxTerminalInputCtrl::SetWritePositionEnd() { m_writeStartingPosition = m_ctrl->GetLastPosition(); }

void wxTerminalInputCtrl::Clear()
{
    m_ctrl->Remove(m_writeStartingPosition, m_ctrl->GetLastPosition());
    SetCaretPos(CaretPos::END);
}

void wxTerminalInputCtrl::SetText(const wxString& text)
{
    m_ctrl->Remove(m_writeStartingPosition, m_ctrl->GetLastPosition());
    m_ctrl->AppendText(text);
    SetCaretPos(CaretPos::END);
}

wxString wxTerminalInputCtrl::GetText() const
{
    return m_ctrl->GetTextRange(m_writeStartingPosition, m_ctrl->GetLastPosition());
}

void wxTerminalInputCtrl::SetCaretPos(wxTerminalInputCtrl::CaretPos pos)
{
    int where = 0;
    switch(pos) {
    case wxTerminalInputCtrl::CaretPos::END:
        where = m_ctrl->GetLastPosition();
        break;
    case wxTerminalInputCtrl::CaretPos::HOME:
        where = m_writeStartingPosition;
        break;
    }
    m_ctrl->SetSelection(where, where);
    m_ctrl->SetCurrentPos(where);
}