#include "wxTerminalInputCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "StringUtils.h"
#include "TextView.h"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "wxTerminalCtrl.h"

#include <vector>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/tokenzr.h>

#if wxTERMINAL_USE_2_CTRLS
#define CAN_GO_BACK() (true)
#define CAN_DELETE() (true)
#define CAN_EDIT() (true)
#else
#define CAN_GO_BACK() (m_ctrl->GetCurrentPos() > m_writeStartingPosition)
#define CAN_DELETE() (m_ctrl->GetCurrentPos() >= m_writeStartingPosition)
#define CAN_EDIT() (m_ctrl->GetCurrentPos() >= m_writeStartingPosition)
#endif

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
#if !wxTERMINAL_USE_2_CTRLS
        if(!(m_stc->GetCurrentPos() >= m_input_ctrl->GetWriteStartPosition())) {
            int where = m_stc->GetLastPosition();
            m_stc->SetSelection(where, where);
            m_stc->SetCurrentPos(where);
        }
#endif
        clEditEventsHandler::OnPaste(event);
    }

    // no undo
    void OnUndo(wxCommandEvent& event) override { CHECK_FOCUS_WINDOW(); }
    // no cut
    void OnCut(wxCommandEvent& event) override { CHECK_FOCUS_WINDOW(); }
    // no redo
    void OnRedo(wxCommandEvent& event) override { CHECK_FOCUS_WINDOW(); }
};
} // namespace

wxTerminalInputCtrl::wxTerminalInputCtrl(wxTerminalCtrl* parent, wxStyledTextCtrl* ctrl)
    : m_terminal(parent)
{
#if wxTERMINAL_USE_2_CTRLS
    m_ctrl = new wxStyledTextCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    m_ctrl->AlwaysShowScrollbars(false, false);
    m_ctrl->SetWrapMode(wxSTC_WRAP_WORD);
    m_ctrl->SetCaretStyle(wxSTC_CARETSTYLE_BLOCK);
    ApplyTheme();
    // calculate the height
    wxClientDC dc(m_ctrl);
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    dc.SetFont(lexer->GetFontForStyle(0, m_ctrl));
    m_ctrl->SetSizeHints(wxNOT_FOUND, dc.GetTextExtent("Tp").GetHeight() + 4);

    // hook key events
    m_ctrl->Bind(wxEVT_CHAR_HOOK, &wxTerminalInputCtrl::ProcessKeyDown, this);
    // add the control the parent's size (at the bottom)
    parent->GetSizer()->Add(m_ctrl, wxSizerFlags(0).Expand());

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &wxTerminalInputCtrl::OnThemeChanged, this);
    m_ctrl->SetWordChars(":~abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$/.-");
    m_ctrl->Bind(wxEVT_STC_CHARADDED, &wxTerminalInputCtrl::OnStcCharAdded, this);
#else
    m_ctrl = ctrl;
#endif
    m_editEvents.Reset(new MyEventsHandler(this, m_ctrl));
    m_ctrl->Bind(wxEVT_CONTEXT_MENU, &wxTerminalInputCtrl::OnMenu, this);
    m_ctrl->Bind(wxEVT_STC_AUTOCOMP_COMPLETED, &wxTerminalInputCtrl::OnStcCompleted, this);

    std::vector<wxAcceleratorEntry> V;
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'R', XRCID("ID_command") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'U', XRCID("ID_clear_line") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'L', XRCID("ID_clear_screen") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'D', XRCID("ID_logout") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'C', XRCID("ID_ctrl_c") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'W', XRCID("ID_delete_word") });

    wxAcceleratorEntry accel_entries[V.size()];
    for(size_t i = 0; i < V.size(); ++i) {
        accel_entries[i] = V[i];
    }
    wxAcceleratorTable accel_table(V.size(), accel_entries);

    m_ctrl->SetAcceleratorTable(accel_table);
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnCommandComplete, this, XRCID("ID_command"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnClearLine, this, XRCID("ID_clear_line"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnClearScreen, this, XRCID("ID_clear_screen"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnLogout, this, XRCID("ID_logout"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnCtrlC, this, XRCID("ID_ctrl_c"));
    m_ctrl->Bind(wxEVT_MENU, &wxTerminalInputCtrl::OnDeleteWord, this, XRCID("ID_delete_word"));
    m_history.Load();
}

wxTerminalInputCtrl::~wxTerminalInputCtrl()
{
    m_ctrl->Unbind(wxEVT_CONTEXT_MENU, &wxTerminalInputCtrl::OnMenu, this);
    m_ctrl->Unbind(wxEVT_STC_AUTOCOMP_COMPLETED, &wxTerminalInputCtrl::OnStcCompleted, this);
#if wxTERMINAL_USE_2_CTRLS
    m_ctrl->Unbind(wxEVT_STC_CHARADDED, &wxTerminalInputCtrl::OnStcCharAdded, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &wxTerminalInputCtrl::OnThemeChanged, this);
#endif
}

void wxTerminalInputCtrl::ShowCompletionBox(CompletionType type)
{
    m_ctrl->AutoCompCancel();

    wxArrayString words;
    int length_typed = 0;
    wxString listItems;
    if(type == CompletionType::COMMANDS) {
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
    case WXK_NUMPAD_ENTER:
    case WXK_RETURN:
        OnEnter();
        break;
    case WXK_UP:
    case WXK_NUMPAD_UP:
        OnUp();
        break;
    case WXK_DOWN:
    case WXK_NUMPAD_DOWN:
        OnDown();
        break;
    case WXK_TAB:
        OnTabComplete();
        return;
    default: {
        event.Skip();
    } break;
    }
}

void wxTerminalInputCtrl::SetWritePositionEnd()
{
#if !wxTERMINAL_USE_2_CTRLS
    m_writeStartingPosition = m_ctrl->GetLastPosition();
#endif
}

void wxTerminalInputCtrl::Clear()
{
#if wxTERMINAL_USE_2_CTRLS
    m_ctrl->ClearAll();
#else
    m_ctrl->Remove(m_writeStartingPosition, m_ctrl->GetLastPosition());
    SetCaretPos(CaretPos::END);
#endif
}

wxString wxTerminalInputCtrl::SetText(const wxString& text)
{
    wxString oldcmd;
#if wxTERMINAL_USE_2_CTRLS
    oldcmd = m_ctrl->GetText();
    m_ctrl->SetText(text);
    SetCaretPos(CaretPos::END);
#else
    oldcmd = m_ctrl->GetTextRange(m_writeStartingPosition, m_ctrl->GetLastPosition());
    m_ctrl->Remove(m_writeStartingPosition, m_ctrl->GetLastPosition());
    m_ctrl->AppendText(text);
    SetCaretPos(CaretPos::END);
#endif
    return oldcmd;
}

wxString wxTerminalInputCtrl::GetText() const
{
#if wxTERMINAL_USE_2_CTRLS
    return m_ctrl->GetText();
#else
    return m_ctrl->GetTextRange(m_writeStartingPosition, m_ctrl->GetLastPosition());
#endif
}

void wxTerminalInputCtrl::SetCaretPos(wxTerminalInputCtrl::CaretPos pos)
{
    int where = 0;
    switch(pos) {
    case wxTerminalInputCtrl::CaretPos::END:
        where = m_ctrl->GetLastPosition();
        break;
    case wxTerminalInputCtrl::CaretPos::HOME:
#if wxTERMINAL_USE_2_CTRLS
        where = 0;
#else
        where = m_writeStartingPosition;
#endif
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
    if(m_completionType == CompletionType::COMMANDS) {
        m_completionType = CompletionType::NONE;
        // user inserted text from the auto completion list
        // to give it a feel like the real terminal, execute it
        m_history.Add(GetText());
        m_terminal->Run(GetText());
#if wxTERMINAL_USE_2_CTRLS
        m_ctrl->ClearAll();
#endif
    }
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

void wxTerminalInputCtrl::OnEnter()
{
    wxString command = GetText();
    m_terminal->Run(command);
    m_history.Add(command);
    m_history.Store(); // update the history
#if wxTERMINAL_USE_2_CTRLS
    m_ctrl->ClearAll();
#endif
}

void wxTerminalInputCtrl::OnUp()
{
    m_history.Up();
    SetText(m_history.Get());
    EnsureCommandLineVisible();
}

void wxTerminalInputCtrl::OnDown()
{
    m_history.Down();
    SetText(m_history.Get());
    EnsureCommandLineVisible();
}

void wxTerminalInputCtrl::OnTabComplete()
{
#if wxTERMINAL_USE_2_CTRLS
    wxString oldcmd = GetText();
    wxString filter =
        m_ctrl->GetTextRange(m_ctrl->WordStartPosition(m_ctrl->GetCurrentPos(), true), m_ctrl->GetCurrentPos());
    SwapAndExecuteCommand(wxString() << "compgen -f " << filter);
    SetText(oldcmd);
    SetCaretPos(CaretPos::END);
    m_waitingForCompgenOutput = true;
#endif
}

void wxTerminalInputCtrl::SetFocus() { m_ctrl->CallAfter(&wxStyledTextCtrl::SetFocus); }

#if wxTERMINAL_USE_2_CTRLS
void wxTerminalInputCtrl::ApplyTheme()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    auto font = lexer->GetFontForStyle(0, m_ctrl);
    auto style = lexer->GetProperty(0);
    bool is_dark = lexer->IsDark();
    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        m_ctrl->StyleSetBackground(i, wxColour(style.GetBgColour()).ChangeLightness(is_dark ? 110 : 90));
        m_ctrl->StyleSetForeground(i, style.GetFgColour());
        m_ctrl->StyleSetFont(i, font);
    }
    m_ctrl->SetCaretPeriod(0); // no blinking
    m_ctrl->Refresh();
}

void wxTerminalInputCtrl::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}
#endif

wxString wxTerminalInputCtrl::GetWordBack()
{
#if wxTERMINAL_USE_2_CTRLS
    auto ctrl = m_ctrl;
#else
    auto ctrl = m_terminal->GetView()->GetCtrl();
#endif
    int curpos = ctrl->GetCurrentPos();
    return ctrl->GetTextRange(ctrl->WordStartPosition(curpos, true), curpos);
}

void wxTerminalInputCtrl::NotifyTerminalOutput()
{
    if(!m_waitingForCompgenOutput) {
        return;
    }

    wxString prefix = GetWordBack();
    if(prefix.empty()) {
        return;
    }

    const wxString LINE_PREFIX = "compgen -f ";
    auto ctrl = m_terminal->GetView()->GetCtrl();
    int last_line = ctrl->LineFromPosition(ctrl->GetLastPosition());

    wxArrayString lines;
    lines.reserve(last_line);
    while(last_line >= 0) {
        wxString line = ctrl->GetLine(last_line);
        line.Trim().Trim(false);

        if(line.StartsWith(LINE_PREFIX) || line.empty()) {
            break;
        }

        lines.insert(lines.begin(), line);
        --last_line;
    }

    if(lines.empty()) {
        return;
    }

    // we got something, consume the flag
    m_waitingForCompgenOutput = false;

    wxString match = StringUtils::FindCommonPrefix(lines);
    // if we reached here and match is not empty - we found a single match
    if(match.empty()) {
        // try omitting the first (it might be the prompt)
        if(lines.size() > 1) {
            lines.pop_back();
            match = StringUtils::FindCommonPrefix(lines);
            if(match.empty()) {
                return;
            }
        }
    }

    if(!match.StartsWith(prefix)) {
        return;
    }

    int start_pos = m_ctrl->WordStartPosition(m_ctrl->GetCurrentPos(), true);
    int end_pos = m_ctrl->GetCurrentPos();

    // only replace the currently typed text if the match len is greater than what
    // is already typed in the terminal
    size_t cur_typed_text_len = end_pos - start_pos;
    if(match.length() <= cur_typed_text_len) {
        return;
    }

    m_ctrl->Replace(start_pos, end_pos, match);
    SetCaretPos(CaretPos::END);
    SetFocus();
}
