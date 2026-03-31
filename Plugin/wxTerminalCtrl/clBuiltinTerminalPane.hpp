#ifndef CLBUILTINTERMINALPANE_HPP
#define CLBUILTINTERMINALPANE_HPP

#include "Notebook.h"
#include "clWorkspaceEvent.hpp"
#include "event_notifier.h"
#include "terminal_event.h"

#include <vector>
#include <wx/aui/auibar.h>
#include <wx/choice.h>
#include <wx/panel.h>

class TerminalView;
class WXDLLIMPEXP_SDK clBuiltinTerminalPane : public wxPanel
{
public:
    clBuiltinTerminalPane(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clBuiltinTerminalPane();

protected:
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnPageChanged(wxBookCtrlEvent& event);
    void OnSetTitle(wxTerminalEvent& event);
    void OnNew(wxCommandEvent& event);
    void DetectTerminals(std::map<wxString, wxString>& terminals);
    bool ReadTerminalOptionsFromDisk(std::map<wxString, wxString>& terminals);
    void WriteTerminalOptionsToDisk(const std::map<wxString, wxString>& terminals);
    std::map<wxString, wxString> GetTerminalsOptions(bool scan = false);
    void OnScanForTerminals(wxCommandEvent& event);
    void UpdateTerminalsChoice(bool scan);
    void OnCtrlR(wxCommandEvent& e);
    void OnCtrlU(wxCommandEvent& e);
    void OnCtrlL(wxCommandEvent& e);
    void OnCtrlD(wxCommandEvent& e);
    void OnCtrlC(wxCommandEvent& e);
    void OnCtrlW(wxCommandEvent& e);
    void OnCtrlZ(wxCommandEvent& e);
    void OnAltF(wxCommandEvent& e);
    void OnAltB(wxCommandEvent& e);
    void OnCtrlA(wxCommandEvent& e);
    void OnCtrlE(wxCommandEvent& e);

private:
    TerminalView* GetActiveTerminal();

    wxAuiToolBar* m_toolbar = nullptr;
    Notebook* m_book = nullptr;
    wxChoice* m_terminal_types = nullptr;
    std::vector<std::pair<EventFilterCallbackToken, wxEventType>> m_tokens;
};

#endif // CLBUILTINTERMINALPANE_HPP
