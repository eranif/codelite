#ifndef CLBUILTINTERMINALPANE_HPP
#define CLBUILTINTERMINALPANE_HPP

#include "codelite_events.h"
#include "codelite_exports.h"
#include "wxTerminalCtrl.h"

#include <wx/panel.h>

class WXDLLIMPEXP_SDK clBuiltinTerminalPane : public wxPanel
{
    wxTerminalCtrl* m_terminal = nullptr;

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void UpdateTextAttributes();

public:
    clBuiltinTerminalPane(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clBuiltinTerminalPane();
};

#endif // CLBUILTINTERMINALPANE_HPP
