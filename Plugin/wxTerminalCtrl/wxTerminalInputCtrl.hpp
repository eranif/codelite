#ifndef WXTERMINALINPUTCTRL_HPP
#define WXTERMINALINPUTCTRL_HPP

#include "clEditorEditEventsHandler.h"
#include "codelite_events.h"
#include "codelite_exports.h"
#include "wxCodeCompletionBoxManager.h"
#include "wxTerminalHistory.hpp"

#include <wx/panel.h>
#include <wx/stc/stc.h>
#include <wx/timer.h>

class wxTerminalCtrl;
class WXDLLIMPEXP_SDK wxTerminalInputCtrl : public wxEvtHandler
{
protected:
    enum CaretPos {
        HOME,
        END,
    };

    enum CompletionType {
        NONE = -1,
        COMMANDS,
        FOLDERS,
    };

protected:
    wxTerminalCtrl* m_terminal = nullptr;
    wxStyledTextCtrl* m_ctrl = nullptr;
    wxTerminalHistory m_history;
    int m_writeStartingPosition = 0;
    clEditEventsHandler::Ptr_t m_editEvents;
    CompletionType m_completionType = CompletionType::NONE;
    bool m_waitingForCompgenOutput = false;

protected:
    void Clear();
    /// set the current command to be executed, returning
    /// the last command
    wxString SetText(const wxString& text);
    wxString GetText() const;
    void SwapAndExecuteCommand(const wxString& cmd);
    void SetCaretPos(wxTerminalInputCtrl::CaretPos pos);
    void OnMenu(wxContextMenuEvent& event);
    void ShowCompletionBox(CompletionType type);
    void OnCCBoxSelected(clCodeCompletionEvent& event);
    void EnsureCommandLineVisible();
    wxString GetWordBack();

    // terminal special actions
    void OnCommandComplete(wxCommandEvent& event);
    void OnClearScreen(wxCommandEvent& event);
    void OnLogout(wxCommandEvent& event);
    void OnClearLine(wxCommandEvent& event);
    void OnCtrlC(wxCommandEvent& event);
    void OnDeleteWord(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);

    // no events (called from ProcessKeyDown)
    void OnEnter();
    void OnUp();
    void OnDown();
    void OnTabComplete();

public:
    wxTerminalInputCtrl(wxTerminalCtrl* parent);
    virtual ~wxTerminalInputCtrl();
    void SetWritePositionEnd();
    void UpdateTextDeleted(int num);
    void ProcessKeyDown(wxKeyEvent& event);
    void SimulateKeyEvent(const wxKeyEvent& event);
    int GetWriteStartPosition() const { return m_writeStartingPosition; }
    void SetFocus();
    bool IsFocused() const { return m_ctrl->HasFocus(); }

    /// called by the terminal when output has arrived
    void NotifyTerminalOutput();

    void ApplyTheme();
    void OnThemeChanged(clCommandEvent& event);
    void Paste();

    wxStyledTextCtrl* GetCtrl() { return m_ctrl; }
};

#endif // WXTERMINALINPUTCTRL_HPP
