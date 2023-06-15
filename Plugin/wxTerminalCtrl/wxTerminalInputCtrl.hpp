#ifndef WXTERMINALINPUTCTRL_HPP
#define WXTERMINALINPUTCTRL_HPP

#include "clEditorEditEventsHandler.h"
#include "codelite_exports.h"
#include "wxTerminalHistory.hpp"

#include <wx/panel.h>
#include <wx/stc/stc.h>

class wxTerminalCtrl;
class WXDLLIMPEXP_SDK wxTerminalInputCtrl
{
    wxTerminalCtrl* m_terminal = nullptr;
    wxStyledTextCtrl* m_ctrl = nullptr;
    wxTerminalHistory m_history;
    int m_writeStartingPosition = 0;
    clEditEventsHandler::Ptr_t m_editEvents;
    
protected:
    enum CaretPos {
        HOME,
        END,
    };

protected:
    void Clear();
    void SetText(const wxString& text);
    wxString GetText() const;
    void SetCaretPos(CaretPos pos);
    void OnMenu(wxContextMenuEvent& event);
    void ShowCompletionBox();
    
public:
    wxTerminalInputCtrl(wxTerminalCtrl* parent, wxStyledTextCtrl* ctrl);
    virtual ~wxTerminalInputCtrl();
    void SetWritePositionEnd();
    void ProcessKeyDown(wxKeyEvent& event);
};

#endif // WXTERMINALINPUTCTRL_HPP
