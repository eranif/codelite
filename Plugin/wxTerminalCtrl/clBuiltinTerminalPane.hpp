#ifndef CLBUILTINTERMINALPANE_HPP
#define CLBUILTINTERMINALPANE_HPP

#include "Notebook.h"
#include "clToolBar.h"
#include "wxTerminalCtrl.h"

#include <wx/panel.h>

class WXDLLIMPEXP_SDK clBuiltinTerminalPane : public wxPanel
{
    wxTerminalCtrl* m_terminal = nullptr;
    clToolBar* m_toolbar = nullptr;
    Notebook* m_book = nullptr;
    std::map<wxString, wxString> m_options_map = { { "bash", "bash" }, { "CMD", "CMD" } };

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnPageChanged(wxBookCtrlEvent& event);
    void OnSetTitle(wxTerminalEvent& event);
    void UpdateTextAttributes();
    void OnNewDropdown(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void DetectTerminals();
    bool ReadTerminalOptionsFromDisk();
    void WriteTerminalOptionsToDisk();

public:
    clBuiltinTerminalPane(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clBuiltinTerminalPane();

    void Focus();
    bool IsFocused();
    wxTerminalCtrl* GetActiveTerminal();
};

#endif // CLBUILTINTERMINALPANE_HPP
