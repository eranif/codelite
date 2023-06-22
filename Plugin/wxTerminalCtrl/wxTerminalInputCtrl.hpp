#ifndef WXTERMINALINPUTCTRL_HPP
#define WXTERMINALINPUTCTRL_HPP

#include "clEditorEditEventsHandler.h"
#include "codelite_events.h"
#include "codelite_exports.h"
#include "wxTerminalHistory.hpp"

#include <wx/panel.h>
#include <wx/stc/stc.h>

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
        WORDS,
        COMMANDS,
    };

protected:
    wxTerminalCtrl* m_terminal = nullptr;
    wxStyledTextCtrl* m_ctrl = nullptr;
    wxTerminalHistory m_history;
    int m_writeStartingPosition = 0;
    clEditEventsHandler::Ptr_t m_editEvents;
    CompletionType m_completionType = CompletionType::NONE;

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
    void OnStcCharAdded(wxStyledTextEvent& event);
    void OnStcCompleted(wxStyledTextEvent& event);
    void OnCodeComplete(clCodeCompletionEvent& event);
    void EnsureCommandLineVisible();

    // terminal special actions
    void OnCommandComplete(wxCommandEvent& event);
    void OnTabComplete(wxCommandEvent& event);
    void OnClearScreen(wxCommandEvent& event);
    void OnLogout(wxCommandEvent& event);
    void OnClearLine(wxCommandEvent& event);
    void OnCtrlC(wxCommandEvent& event);
    void OnDeleteWord(wxCommandEvent& event);
    void OnEnter(wxCommandEvent& event);
    void OnUp(wxCommandEvent& event);
    void OnDown(wxCommandEvent& event);

public:
    wxTerminalInputCtrl(wxTerminalCtrl* parent, wxStyledTextCtrl* ctrl);
    virtual ~wxTerminalInputCtrl();
    void SetWritePositionEnd();
    void UpdateTextDeleted(int num);
    void ProcessKeyDown(wxKeyEvent& event);
    int GetWriteStartPosition() const { return m_writeStartingPosition; }
};

#endif // WXTERMINALINPUTCTRL_HPP
