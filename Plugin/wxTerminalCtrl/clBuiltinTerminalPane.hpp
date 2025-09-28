#ifndef CLBUILTINTERMINALPANE_HPP
#define CLBUILTINTERMINALPANE_HPP

#include "Notebook.h"
#include "clToolBar.h"
#include "wxTerminalCtrl.h"

#include <wx/panel.h>

class WXDLLIMPEXP_SDK clBuiltinTerminalPane : public wxPanel
{

public:
    clBuiltinTerminalPane(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clBuiltinTerminalPane();

    void Focus();
    bool IsFocused();

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnPageChanged(wxBookCtrlEvent& event);
    void OnSetTitle(wxTerminalEvent& event);
    void UpdateTextAttributes();
    void OnNewDropdown(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void DetectTerminals(std::map<wxString, wxString>& terminals);
    bool ReadTerminalOptionsFromDisk(std::map<wxString, wxString>& terminals);
    void WriteTerminalOptionsToDisk(const std::map<wxString, wxString>& terminals);
    std::map<wxString, wxString> GetTerminalsOptions(bool scan = false);
    void OnScanForTerminals(wxCommandEvent& event);
    void UpdateTerminalsChoice(bool scan);
    void OnIdle(wxIdleEvent& event);

private:
    wxTerminalCtrl* GetActiveTerminal();
    clToolBar* m_toolbar = nullptr;
    Notebook* m_book = nullptr;
    wxChoice* m_terminal_types = nullptr;
};

#endif // CLBUILTINTERMINALPANE_HPP
