#include "wxTerminalInputCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "wxTerminalCtrl.h"

#include <vector>
#include <wx/bitmap.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/tokenzr.h>

#define CAN_GO_BACK() (m_ctrl->GetCurrentPos() > m_writeStartingPosition)
#define CAN_DELETE() (m_ctrl->GetCurrentPos() >= m_writeStartingPosition)
#define CAN_EDIT() (m_ctrl->GetCurrentPos() >= m_writeStartingPosition)

namespace
{
class MyEventsHandler : public clEditEventsHandler
{
    wxTerminalInputCtrl* m_input_ctrl = nullptr;

public:
    MyEventsHandler(wxTerminalInputCtrl* input_ctrl, wxStyledTextCtrl* ctrl)
        : clEditEventsHandler(ctrl)
        , m_input_ctrl(input_ctrl)
    {
    }

    void OnPaste(wxCommandEvent& event) override
    {
        CHECK_FOCUS_WINDOW();
        if(!(m_stc->GetCurrentPos() >= m_input_ctrl->GetWriteStartPosition())) {
            int where = m_stc->GetLastPosition();
            m_stc->SetSelection(where, where);
            m_stc->SetCurrentPos(where);
        }
        clEditEventsHandler::OnPaste(event);
    }

    // no undo
    void OnUndo(wxCommandEvent& event) override { CHECK_FOCUS_WINDOW(); }
    // no cut
    void OnCut(wxCommandEvent& event) override { CHECK_FOCUS_WINDOW(); }
    // no redo
    void OnRedo(wxCommandEvent& event) override { CHECK_FOCUS_WINDOW(); }
};

wxString get_word(const wxString& command)
{
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
    return last_token;
}
} // namespace

wxTerminalInputCtrl::wxTerminalInputCtrl(wxTerminalCtrl* parent, wxStyledTextCtrl* ctrl)
    : m_terminal(parent)
    , m_ctrl(ctrl)
{
    m_editEvents.Reset(new MyEventsHandler(this, m_ctrl));
    m_ctrl->Bind(wxEVT_CONTEXT_MENU, &wxTerminalInputCtrl::OnMenu, this);
    m_ctrl->Bind(wxEVT_STC_CHARADDED, &wxTerminalInputCtrl::OnStcCharAdded, this);
    m_ctrl->Bind(wxEVT_STC_AUTOCOMP_COMPLETED, &wxTerminalInputCtrl::OnStcCompleted, this);

    std::vector<wxAcceleratorEntry> V;
    V.push_back(wxAcceleratorEntry{ wxACCEL_NORMAL, WXK_TAB, XRCID("ID_dircomp") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'R', XRCID("ID_command") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'U', XRCID("ID_clear_line") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'L', XRCID("ID_clear_screen") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'D', XRCID("ID_logout") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'C', XRCID("ID_ctrl_c") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'W', XRCID("ID_delete_word") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_NORMAL, WXK_RETURN, XRCID("ID_enter") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_NORMAL, WXK_NUMPAD_ENTER, XRCID("ID_enter") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_NORMAL, WXK_UP, XRCID("ID_up") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_NORMAL, WXK_NUMPAD_UP, XRCID("ID_up") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_NORMAL, WXK_DOWN, XRCID("ID_down") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_NORMAL, WXK_NUMPAD_DOWN, XRCID("ID_down") });

    wxAcceleratorEntry accel_entries[V.size()];
    for(size_t i = 0; i < V.size(); ++i) {
        accel_entries[i] = V[i];
    }
    wxAcceleratorTable accel_table(V.size(), accel_entries);

    m_ctrl->SetAcceleratorTable(accel_table);
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnTabComplete, this, XRCID("ID_dircomp"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnCommandComplete, this, XRCID("ID_command"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnClearLine, this, XRCID("ID_clear_line"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnClearScreen, this, XRCID("ID_clear_screen"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnLogout, this, XRCID("ID_logout"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnCtrlC, this, XRCID("ID_ctrl_c"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnDeleteWord, this, XRCID("ID_delete_word"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnEnter, this, XRCID("ID_enter"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnUp, this, XRCID("ID_up"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnDown, this, XRCID("ID_down"));

    EventNotifier::Get()->Bind(wxEVT_CC_CODE_COMPLETE, &wxTerminalInputCtrl::OnCodeComplete, this);
    m_history.Load();
}

wxTerminalInputCtrl::~wxTerminalInputCtrl()
{
    m_ctrl->Unbind(wxEVT_CONTEXT_MENU, &wxTerminalInputCtrl::OnMenu, this);
    m_ctrl->Unbind(wxEVT_STC_AUTOCOMP_COMPLETED, &wxTerminalInputCtrl::OnStcCompleted, this);
    m_ctrl->Unbind(wxEVT_STC_CHARADDED, &wxTerminalInputCtrl::OnStcCharAdded, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_CODE_COMPLETE, &wxTerminalInputCtrl::OnCodeComplete, this);
}

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
        m_completionType = CompletionType::WORDS;
        wxString command = GetText();
        wxString last_token = get_word(command);
        length_typed = last_token.length();
        wxString filter_text = last_token.Lower();
        // exclude the last line
        editor_text = editor_text.BeforeLast('\n');

        wxArrayString filteredWords;
        words = ::wxStringTokenize(editor_text, "\r\n \t>/\\'\"[]()<>*&^%#!@+=:,;{}|/", wxTOKEN_STRTOK);
        std::set<wxString> suggest_set;
        for(const auto& word : words) {
            wxString lc_word = word.Lower();
            if(!wxIsdigit(word[0]) &&  // Not a number
               !lc_word.empty() &&     // the word to display is not empty
               (filter_text.empty() || // the word to display contains the filter or the filter is empty
                lc_word.Contains(filter_text))) {
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
        m_completionType = CompletionType::COMMANDS;
        wxString filter = GetText();
        listItems = m_history.ForCompletion(GetText());
        length_typed = filter.length();
    } else {
        // unknown completion type
        m_completionType = CompletionType::NONE;
        return;
    }

    if(listItems.empty()) {
        m_completionType = CompletionType::NONE;
        return;
    }

    wxString selection = listItems.BeforeFirst('!');
    m_ctrl->AutoCompSetSeparator('!');
    m_ctrl->AutoCompSetAutoHide(false);
    m_ctrl->AutoCompSetMaxWidth(100);
    m_ctrl->AutoCompSetMaxHeight(4);
    m_ctrl->AutoCompShow(length_typed, listItems);
    if(!selection.empty()) {
        m_ctrl->AutoCompSelect(selection);
    }
}

namespace
{
std::unordered_set<int> DO_NOT_REFRESH_KEYS = { WXK_RETURN,       WXK_NUMPAD_ENTER, WXK_UP,    WXK_DOWN,
                                                WXK_NUMPAD_UP,    WXK_NUMPAD_DOWN,  WXK_RIGHT, WXK_LEFT,
                                                WXK_NUMPAD_RIGHT, WXK_NUMPAD_LEFT,  WXK_HOME,  WXK_END,
                                                WXK_PAGEDOWN,     WXK_PAGEUP,       WXK_ESCAPE };
}

void wxTerminalInputCtrl::ProcessKeyDown(wxKeyEvent& event)
{
    if(m_ctrl->AutoCompActive()) {
        if(event.GetKeyCode() == WXK_BACK && !CAN_GO_BACK()) {
            // don't allow to delete outside the writing zone
            m_ctrl->AutoCompCancel();
            return;
        } else if(DO_NOT_REFRESH_KEYS.count(event.GetKeyCode()) == 0 && !event.RawControlDown() &&
                  !event.ControlDown()) {
            // let the conrol process the key
            event.Skip();
        } else {
            event.Skip();
        }
        return;
    }

    m_completionType = CompletionType::NONE;
    switch(event.GetKeyCode()) {
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
        if((event.GetKeyCode() >= WXK_SPACE) & (event.GetKeyCode() < WXK_DELETE) && !CAN_EDIT() &&
           (event.GetModifiers() == wxMOD_SHIFT || event.GetModifiers() == wxMOD_NONE)) {
            // ASCII characters - in the non wanted area
            // and shift is pressed or no modifier at all
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

wxString wxTerminalInputCtrl::SetText(const wxString& text)
{
    wxString oldcmd = m_ctrl->GetTextRange(m_writeStartingPosition, m_ctrl->GetLastPosition());
    m_ctrl->Remove(m_writeStartingPosition, m_ctrl->GetLastPosition());
    m_ctrl->AppendText(text);
    SetCaretPos(CaretPos::END);
    return oldcmd;
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

void wxTerminalInputCtrl::OnStcCharAdded(wxStyledTextEvent& event) { event.Skip(); }
void wxTerminalInputCtrl::OnStcCompleted(wxStyledTextEvent& event)
{
    event.Skip();
    // user inserted text from the auto completion list
    // to give it a feel like the real terminal, execute it
    m_history.Add(GetText());
    m_terminal->Run(GetText());
}

void wxTerminalInputCtrl::OnCodeComplete(clCodeCompletionEvent& event)
{
    if(wxWindow::FindFocus() != m_ctrl) {
        event.Skip();
        return;
    }

    // ours to handle
    CallAfter(&wxTerminalInputCtrl::ShowCompletionBox, CompletionType::WORDS);
}

void wxTerminalInputCtrl::EnsureCommandLineVisible()
{
    m_ctrl->ScrollToEnd();
    m_ctrl->EnsureCaretVisible();
}

void wxTerminalInputCtrl::SwapAndExecuteCommand(const wxString& cmd)
{
    auto curcmd = SetText(cmd);
    m_terminal->Run(cmd);
    SetText(curcmd);
}

void wxTerminalInputCtrl::OnCommandComplete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_ctrl->AutoCompActive() && m_completionType == CompletionType::COMMANDS) {
        // refresh the list
        ShowCompletionBox(CompletionType::COMMANDS);
    } else if(!m_ctrl->AutoCompActive()) {
        if(!CAN_EDIT()) {
            SetCaretPos(CaretPos::END);
        }
        ShowCompletionBox(CompletionType::COMMANDS);
    }
}

void wxTerminalInputCtrl::OnTabComplete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_ctrl->AutoCompActive() && m_completionType == CompletionType::WORDS) {
        // refresh the list
        ShowCompletionBox(CompletionType::WORDS);
    } else if(!m_ctrl->AutoCompActive()) {
        if(!CAN_EDIT()) {
            SetCaretPos(CaretPos::END);
        }
        ShowCompletionBox(CompletionType::WORDS);
    }
}

void wxTerminalInputCtrl::OnClearScreen(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_terminal->ClearScreen();
}

void wxTerminalInputCtrl::OnLogout(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_terminal->Logout();
}

void wxTerminalInputCtrl::OnClearLine(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Clear();
}

void wxTerminalInputCtrl::OnCtrlC(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_terminal->GenerateCtrlC();
}

void wxTerminalInputCtrl::OnDeleteWord(wxCommandEvent& event)
{
    wxUnusedVar(event);
    int word_start_pos = m_ctrl->WordStartPosition(m_ctrl->GetCurrentPos(), true);
    if(word_start_pos > m_writeStartingPosition) {
        m_ctrl->DelWordLeft();
    } else {
        Clear();
    }
}

void wxTerminalInputCtrl::OnEnter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString command = GetText();
    m_terminal->Run(command);
    m_history.Add(command);
    m_history.Store(); // update the history
}

void wxTerminalInputCtrl::OnUp(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_history.Up();
    SetText(m_history.Get());
    EnsureCommandLineVisible();
}

void wxTerminalInputCtrl::OnDown(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_history.Down();
    SetText(m_history.Get());
    EnsureCommandLineVisible();
}