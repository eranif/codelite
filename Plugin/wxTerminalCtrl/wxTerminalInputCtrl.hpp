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
    void SetText(const wxString& text);
    wxString GetText() const;
    void SetCaretPos(CaretPos pos);
    void OnMenu(wxContextMenuEvent& event);
    void ShowCompletionBox(CompletionType type);
    void OnStcCharAdded(wxStyledTextEvent& event);
    void OnStcCompleted(wxStyledTextEvent& event);
    void OnCodeComplete(clCodeCompletionEvent& event);
    void EnsureCommandLineVisible();

public:
    wxTerminalInputCtrl(wxTerminalCtrl* parent, wxStyledTextCtrl* ctrl);
    virtual ~wxTerminalInputCtrl();
    void SetWritePositionEnd();
    void UpdateTextDeleted(int num);
    void ProcessKeyDown(wxKeyEvent& event);
    int GetWriteStartPosition() const { return m_writeStartingPosition; }
};

#endif // WXTERMINALINPUTCTRL_HPP
