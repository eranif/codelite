#ifndef CLBUILTINTERMINALPANE_HPP
#define CLBUILTINTERMINALPANE_HPP

#include "clToolBar.h"
#include "codelite_events.h"
#include "codelite_exports.h"
#include "wxTerminalCtrl.h"

#include <wx/aui/auibook.h>
#include <wx/panel.h>

class WXDLLIMPEXP_SDK clBuiltinTerminalPane : public wxPanel
{
    wxTerminalCtrl* m_terminal = nullptr;
    clToolBar* m_toolbar = nullptr;
    wxAuiNotebook* m_book = nullptr;

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnPageChanged(wxAuiNotebookEvent& event);
    void OnSetTitle(wxTerminalEvent& event);
    void UpdateTextAttributes();
    void OnNewDropdown(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);

public:
    clBuiltinTerminalPane(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clBuiltinTerminalPane();

    void Focus();
    wxTerminalCtrl* GetActiveTerminal();
};

#endif // CLBUILTINTERMINALPANE_HPP
