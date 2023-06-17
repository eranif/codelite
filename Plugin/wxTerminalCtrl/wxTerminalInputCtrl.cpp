#include "wxTerminalInputCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "wxTerminalCtrl.h"

#include <wx/bitmap.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/tokenzr.h>

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
    m_ctrl->Bind(wxEVT_CONTEXT_MENU, &wxTerminalInputCtrl::OnMenu, this);
    m_history.Load();
}

wxTerminalInputCtrl::~wxTerminalInputCtrl() { m_ctrl->Unbind(wxEVT_CONTEXT_MENU, &wxTerminalInputCtrl::OnMenu, this); }

void wxTerminalInputCtrl::ShowCompletionBox(CompletionType type)
{
    m_ctrl->AutoCompCancel();
    wxArrayString words;
    int length_typed = 0;
    wxString listItems;
    if(type == CompletionType::WORDS) {
        int start_pos = wxNOT_FOUND;
        int end_pos = wxNOT_FOUND;
        wxString editor_text = m_ctrl->GetText();

        wxString command = GetText();
        wxString last_token;
        for(auto iter = command.rbegin(); iter != command.rend(); ++iter) {
            auto ch = *iter;
            if(ch == ' ' || ch == '\t' || ch == '/'
#ifdef __WXMSW__
               || ch == '\\'
#endif
            ) {
                break;
            } else {
                last_token.insert(last_token.begin(), ch);
            }
        }

        length_typed = last_token.length();
        wxString filter_text = last_token.Lower();
        // exclude the last line
        editor_text = editor_text.BeforeLast('\n');

        wxArrayString filteredWords;
        words = ::wxStringTokenize(editor_text, "\r\n \t>/\\'\"[]()<>*&^%#!@+=:,;{}|/", wxTOKEN_STRTOK);
        std::set<wxString> suggest_set;
        for(const auto& word : words) {
            wxString lc_word = word.Lower();
            if(!wxIsdigit(word[0]) && (filter_text.empty() || lc_word.Contains(filter_text)) && !lc_word.empty()) {
                suggest_set.insert(word);
            }
        }
        if(suggest_set.empty()) {
            return;
        }

        for(const auto& entry : suggest_set) {
            listItems << entry << "!";
        }
        listItems.RemoveLast();
    } else if(type == CompletionType::COMMANDS) {
        wxString filter = GetText();
        listItems = m_history.ForCompletion(GetText());
        length_typed = filter.length();
    } else {
        // unknown completion type
        return;
    }

    if(listItems.empty()) {
        return;
    }

    m_ctrl->AutoCompSetSeparator('!');
    m_ctrl->AutoCompSetAutoHide(false);
    m_ctrl->AutoCompSetMaxWidth(100);
    m_ctrl->AutoCompSetMaxHeight(4);
    m_ctrl->AutoCompShow(length_typed, listItems);
}

#define CAN_GO_BACK() (m_ctrl->GetCurrentPos() > m_writeStartingPosition)
#define CAN_DELETE() (m_ctrl->GetCurrentPos() >= m_writeStartingPosition)
#define CAN_EDIT() (m_ctrl->GetCurrentPos() >= m_writeStartingPosition)

void wxTerminalInputCtrl::ProcessKeyDown(wxKeyEvent& event)
{
    if(m_ctrl->AutoCompActive()) {
        if(event.GetKeyCode() == WXK_BACK && !CAN_GO_BACK()) {
            // don't allow to delete outside the writing zone
            m_ctrl->AutoCompCancel();
            return;
        } else if(event.RawControlDown() && event.GetKeyCode() == 'R') {
            // refresh the list
            ShowCompletionBox(CompletionType::COMMANDS);
            return;
        }
        event.Skip();
        return;
    }

    if(event.RawControlDown()) {
        switch(event.GetKeyCode()) {
        case 'C':
            m_terminal->GenerateCtrlC();
            return;
        case 'W': {
            int word_start_pos = m_ctrl->WordStartPosition(m_ctrl->GetCurrentPos(), true);
            if(word_start_pos > m_writeStartingPosition) {
                m_ctrl->DelWordLeft();
            } else {
                Clear();
            }
            return;
        }
        case 'D':
            m_terminal->Logout();
            return;
        case 'L':
            m_terminal->ClearScreen();
            return;
        case 'U':
            Clear();
            return;
        case 'R':
            ShowCompletionBox(CompletionType::COMMANDS);
            return;
        default:
            break;
        }
    }

    switch(event.GetKeyCode()) {
    case WXK_NUMPAD_ENTER:
    case WXK_RETURN: {
        wxString command = GetText();
        m_terminal->Run(command);
        m_history.Add(command);
        m_history.Store(); // update the history
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
        ShowCompletionBox(CompletionType::WORDS);
        return;
    case WXK_LEFT:
    case WXK_NUMPAD_LEFT:
    case WXK_BACK:
        if(CAN_GO_BACK()) {
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

void wxTerminalInputCtrl::OnMenu(wxContextMenuEvent& event)
{
    wxMenu menu;
    menu.Append(wxID_COPY);
    menu.Append(wxID_PASTE);

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            if(!CAN_GO_BACK()) {
                SetCaretPos(CaretPos::END);
            }
            int where = m_ctrl->GetLastPosition();
            m_ctrl->SetSelection(where, where);
            m_ctrl->SetCurrentPos(where);
            m_ctrl->Paste();
        },
        wxID_PASTE);
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            if(m_ctrl->CanCopy()) {
                m_ctrl->Copy();
            }
        },
        wxID_COPY);
    m_ctrl->PopupMenu(&menu);
}

void wxTerminalInputCtrl::UpdateTextDeleted(int num)
{
    if(m_writeStartingPosition < num) {
        return;
    }
    m_writeStartingPosition -= num;
}
