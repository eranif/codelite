#ifndef WXTERMINALINPUTCTRL_HPP
#define WXTERMINALINPUTCTRL_HPP

#include "clEditorEditEventsHandler.h"
#include "codelite_exports.h"
#include "wxTerminalHistory.hpp"

#include <wx/panel.h>
#include <wx/stc/stc.h>

class wxTerminalCtrl;
class WXDLLIMPEXP_SDK wxTerminalInputCtrl : public wxPanel
{
protected:
    friend class wxTerminalCtrl;
    wxStyledTextCtrl* m_ctrl = nullptr;
    wxTerminalCtrl* m_terminal = nullptr;
    wxTerminalHistory m_history;
    clEditEventsHandler::Ptr_t m_editEvents;

protected:
    void OnKeyDown(wxKeyEvent& event);
    void Clear();
    void ApplyStyle();

public:
    wxTerminalInputCtrl(wxTerminalCtrl* parent);
    virtual ~wxTerminalInputCtrl();
    void SetCaretEnd();
};

#endif // WXTERMINALINPUTCTRL_HPP
