#include "wxTerminalInputCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "StringUtils.h"
#include "clIdleEventThrottler.hpp"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "wxTerminalCtrl.h"
#include "wxTerminalOutputCtrl.hpp"

#include <vector>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/menu.h>
#include <wx/tokenzr.h>
#include <wx/uiaction.h>

#define CAN_GO_BACK() (true)
#define CAN_DELETE() (true)
#define CAN_EDIT() (true)
#define IS_CCBOX_ACTIVE() wxCodeCompletionBoxManager::Get().IsShown()

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
        clEditEventsHandler::OnPaste(event);
    }

    // no undo
    void OnUndo(wxCommandEvent& event) override { CHECK_FOCUS_WINDOW(); }
    // no cut
    void OnCut(wxCommandEvent& event) override { CHECK_FOCUS_WINDOW(); }
    // no redo
    void OnRedo(wxCommandEvent& event) override { CHECK_FOCUS_WINDOW(); }
};

constexpr int MARKER_ARROWS = 2;
constexpr int MARING_MARKER_ID = 2;
} // namespace

wxTerminalInputCtrl::wxTerminalInputCtrl(wxTerminalCtrl* parent)
    : m_terminal(parent)
{
    m_ctrl = new wxStyledTextCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    m_ctrl->AlwaysShowScrollbars(false, false);
    m_ctrl->SetWrapMode(wxSTC_WRAP_WORD);
    m_ctrl->SetCaretStyle(wxSTC_CARETSTYLE_BLOCK);

    for (int i = 0; i < wxSTC_MAX_MARGIN; ++i) {
        m_ctrl->SetMarginWidth(i, 0);
    }

    // define the marker margin
    m_ctrl->SetMarginType(MARING_MARKER_ID, wxSTC_MARGIN_SYMBOL);
    m_ctrl->SetMarginMask(MARING_MARKER_ID, ~(wxSTC_MASK_FOLDERS));
    m_ctrl->SetMarginWidth(MARING_MARKER_ID, 16);

    // define the marker
    m_ctrl->MarkerDefine(MARKER_ARROWS, wxSTC_MARK_ARROWS);
    m_ctrl->MarkerAdd(m_ctrl->GetCurrentLine(), MARKER_ARROWS);
    ApplyTheme();

    // calculate the height
    wxClientDC dc(m_ctrl);
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    dc.SetFont(lexer->GetFontForStyle(0, m_ctrl));
    m_ctrl->SetSizeHints(wxNOT_FOUND, dc.GetTextExtent("Tp").GetHeight() + 4);

    // hook key events
    m_ctrl->Bind(wxEVT_CHAR_HOOK, &wxTerminalInputCtrl::ProcessKeyDown, this);

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &wxTerminalInputCtrl::OnThemeChanged, this);
    m_ctrl->SetWordChars(R"#(\:~abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$/.-)#");

    m_editEvents = std::make_unique<MyEventsHandler>(this, m_ctrl);
    m_ctrl->Bind(wxEVT_CONTEXT_MENU, &wxTerminalInputCtrl::OnMenu, this);
    m_ctrl->Bind(wxEVT_IDLE, &wxTerminalInputCtrl::OnIdle, this);
    EventNotifier::Get()->Bind(wxEVT_CCBOX_SELECTION_MADE, &wxTerminalInputCtrl::OnCCBoxSelected, this);

    std::vector<wxAcceleratorEntry> V;
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'R', XRCID("ID_command") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'U', XRCID("ID_clear_line") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'L', XRCID("ID_clear_screen") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'D', XRCID("ID_logout") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'C', XRCID("ID_ctrl_c") });
    V.push_back(wxAcceleratorEntry{ wxACCEL_RAW_CTRL, (int)'W', XRCID("ID_delete_word") });

    wxAcceleratorTable accel_table(V.size(), V.data());

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
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SELECTION_MADE, &wxTerminalInputCtrl::OnCCBoxSelected, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &wxTerminalInputCtrl::OnThemeChanged, this);
    m_ctrl->Unbind(wxEVT_CONTEXT_MENU, &wxTerminalInputCtrl::OnMenu, this);
    m_ctrl->Unbind(wxEVT_IDLE, &wxTerminalInputCtrl::OnIdle, this);
}

void wxTerminalInputCtrl::ShowCompletionBox(CompletionType type)
{
    wxArrayString words;
    int length_typed = 0;
    wxArrayString items;
    if (type == CompletionType::COMMANDS) {
        m_completionType = CompletionType::COMMANDS;
        wxString filter = GetText();
        items = m_history.ForCompletion(GetText());
        length_typed = filter.length();
    } else {
        // unknown completion type
        m_completionType = CompletionType::NONE;
        return;
    }

    if (items.empty()) {
        m_completionType = CompletionType::NONE;
        return;
    }

    wxCodeCompletionBoxEntry::Vec_t V;
    V.reserve(items.size());
    for (const auto& item : items) {
        V.push_back(wxCodeCompletionBoxEntry::New(item, wxNullBitmap, nullptr));
    }

    // display the box
    int width = m_ctrl->GetSize().GetWidth() - (clSystemSettings::GetMetric(wxSYS_VSCROLL_X) * 2);
    if (width < 0) {
        width = wxNOT_FOUND;
    }

    wxCodeCompletionBoxManager::Get().ShowCompletionBox(
        m_ctrl, V,
        wxCodeCompletionBox::kRefreshOnKeyType | wxCodeCompletionBox::kNoShowingEvent |
            wxCodeCompletionBox::kAlwaysShow,
        m_completionType == CompletionType::COMMANDS ? 0 : wxNOT_FOUND, this, wxSize(width, wxNOT_FOUND));
}

void wxTerminalInputCtrl::ProcessKeyDown(wxKeyEvent& event)
{
    if (IS_CCBOX_ACTIVE()) {
        event.Skip();
        return;
    }

    m_completionType = CompletionType::NONE;
    switch (event.GetKeyCode()) {
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
        if (event.GetModifiers() == 0) {
            OnTabComplete();
        } else {
            event.Skip();
        }
        return;
    default: {
        event.Skip();
    } break;
    }
}

void wxTerminalInputCtrl::SetWritePositionEnd() { m_writeStartingPosition = m_ctrl->GetLastPosition(); }

void wxTerminalInputCtrl::Clear()
{
    m_ctrl->ClearAll(); // restore the marker
    m_ctrl->MarkerAdd(m_ctrl->GetCurrentLine(), MARKER_ARROWS);
}

wxString wxTerminalInputCtrl::SetText(const wxString& text)
{
    wxString oldcmd;
    oldcmd = m_ctrl->GetText();
    m_ctrl->SetText(text);
    SetCaretPos(CaretPos::END);
    return oldcmd;
}

wxString wxTerminalInputCtrl::GetText() const { return m_ctrl->GetText(); }

void wxTerminalInputCtrl::SetCaretPos(wxTerminalInputCtrl::CaretPos pos)
{
    int where = 0;
    switch (pos) {
    case wxTerminalInputCtrl::CaretPos::END:
        where = m_ctrl->GetLastPosition();
        break;
    case wxTerminalInputCtrl::CaretPos::HOME:
        where = 0;
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
            if (!CAN_GO_BACK()) {
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
            if (m_ctrl->CanCopy()) {
                m_ctrl->Copy();
            }
        },
        wxID_COPY);
    m_ctrl->PopupMenu(&menu);
}

void wxTerminalInputCtrl::UpdateTextDeleted(int num)
{
    if (m_writeStartingPosition < num) {
        return;
    }
    m_writeStartingPosition -= num;
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
    if (IS_CCBOX_ACTIVE() && m_completionType == CompletionType::COMMANDS) {
        // refresh the list
        ShowCompletionBox(CompletionType::COMMANDS);
    } else if (!IS_CCBOX_ACTIVE()) {
        if (!CAN_EDIT()) {
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
    if (word_start_pos > m_writeStartingPosition) {
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
    m_ctrl->ClearAll();
    // restore the marker
    m_ctrl->MarkerAdd(m_ctrl->GetCurrentLine(), MARKER_ARROWS);
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
    wxString oldcmd = GetText();
    wxString filter =
        m_ctrl->GetTextRange(m_ctrl->WordStartPosition(m_ctrl->GetCurrentPos(), true), m_ctrl->GetCurrentPos());
    SwapAndExecuteCommand(wxString() << "compgen -f " << filter);
    SetText(oldcmd);
    SetCaretPos(CaretPos::END);
    m_waitingForCompgenOutput = true;
}

void wxTerminalInputCtrl::SetFocus() { m_ctrl->CallAfter(&wxStyledTextCtrl::SetFocus); }

void wxTerminalInputCtrl::ApplyTheme()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    lexer->Apply(m_ctrl);
    m_ctrl->SetCaretPeriod(0); // no blinking
    m_ctrl->MarkerSetForeground(MARKER_ARROWS, lexer->GetProperty(0).GetFgColour());
    m_ctrl->Refresh();
}

void wxTerminalInputCtrl::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    ApplyTheme();
}

wxString wxTerminalInputCtrl::GetWordBack()
{
    auto ctrl = m_ctrl;
    int curpos = ctrl->GetCurrentPos();
    return ctrl->GetTextRange(ctrl->WordStartPosition(curpos, true), curpos);
}

void wxTerminalInputCtrl::NotifyTerminalOutput()
{
    if (!m_waitingForCompgenOutput) {
        return;
    }

    wxString prefix = GetWordBack();
    if (prefix.empty()) {
        return;
    }

    const wxString LINE_PREFIX = "compgen -f ";
    auto ctrl = m_terminal->GetView()->GetCtrl();
    int last_line = ctrl->LineFromPosition(ctrl->GetLastPosition());

    wxCodeCompletionBoxEntry::Vec_t completions;
    while (last_line >= 0) {
        wxString line = ctrl->GetLine(last_line);
        --last_line;
        line.Trim().Trim(false);

        if (line.Contains(LINE_PREFIX) || line.empty()) {
            break;
        }

        if (line.Contains(" ") || line.length() < prefix.length()) {
            // don't present lines with spaces or shorter than the prefix
            continue;
        }

        completions.push_back(wxCodeCompletionBoxEntry::New(line));
    }

    if (completions.empty()) {
        return;
    }

    // we got something, consume the flag
    m_waitingForCompgenOutput = false;
    int start_pos = m_ctrl->WordStartPosition(m_ctrl->GetCurrentPos(), true);
    int end_pos = m_ctrl->GetCurrentPos();

    m_completionType = CompletionType::FOLDERS;
    // display the box
    int width = m_ctrl->GetSize().GetWidth() - (clSystemSettings::GetMetric(wxSYS_VSCROLL_X) * 2);
    if (width < 0) {
        width = wxNOT_FOUND;
    }
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(
        m_ctrl, completions,
        wxCodeCompletionBox::kNoShowingEvent | wxCodeCompletionBox::kInsertSingleMatch |
            wxCodeCompletionBox::kAlwaysShow,
        m_completionType == CompletionType::COMMANDS ? 0 : wxNOT_FOUND, this, wxSize(width, wxNOT_FOUND));
}

void wxTerminalInputCtrl::OnCCBoxSelected(clCodeCompletionEvent& event)
{
    if (event.GetEventObject() != this) {
        event.Skip();
        return;
    }

    switch (m_completionType) {
    case CompletionType::COMMANDS:
        m_ctrl->ClearAll();
        // restore the marker
        m_ctrl->MarkerAdd(m_ctrl->GetCurrentLine(), MARKER_ARROWS);
        // user inserted text from the auto completion list
        // to give it a feel like the real terminal, execute it
        m_history.Add(event.GetEntry()->GetInsertText());
        m_terminal->Run(event.GetEntry()->GetInsertText());
        break;
    case CompletionType::FOLDERS: {
        int start_pos = m_ctrl->WordStartPosition(m_ctrl->GetCurrentPos(), true);
        int end_pos = m_ctrl->GetCurrentPos();
        m_ctrl->Replace(start_pos, end_pos, event.GetEntry()->GetInsertText());
        SetCaretPos(CaretPos::END);
        SetFocus();
    } break;
    default:
        break;
    }
    m_completionType = CompletionType::NONE;
}

void wxTerminalInputCtrl::OnIdle(wxIdleEvent& event)
{
    event.Skip();

    static clIdleEventThrottler event_throttler{ 200 };
    if (!event_throttler.CanHandle()) {
        return;
    }

    int curline = m_ctrl->GetCurrentLine();
    if (m_ctrl->IsShown() && m_ctrl->MarkerGet(curline) == 0) {
        m_ctrl->MarkerAdd(curline, MARKER_ARROWS);
    }
}

void wxTerminalInputCtrl::SimulateKeyEvent(const wxKeyEvent& event)
{
#ifdef __WXMSW__
    wxUIActionSimulator sim;
    auto pt = m_ctrl->PointFromPosition(m_ctrl->GetLastPosition());
    // click at the end of the text control
    sim.MouseMove(m_ctrl->GetScreenPosition() + wxPoint(pt.x, 10));
    sim.MouseClick(wxMOUSE_BTN_LEFT);
    wxYield();
    // no one processed this simulation yet, do it now
    m_ctrl->SetFocus();
    sim.Char(event.GetKeyCode(), event.GetModifiers());
#else
    wxUnusedVar(event);
    m_ctrl->SetFocus();
#endif
}

void wxTerminalInputCtrl::Paste()
{
    CHECK_PTR_RET(m_ctrl);
    CHECK_COND_RET(m_ctrl->CanPaste());
    m_ctrl->SetFocus();
    m_ctrl->SetInsertionPointEnd();
    m_ctrl->Paste();
}
