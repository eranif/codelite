#ifndef CLPROFILEHANDLER_H
#define CLPROFILEHANDLER_H

#include <wx/event.h>
#include "codelite_exports.h"
#include "wxStringHash.h"
#include "macros.h"
#include "cl_command_event.h"
#include "Notebook.h"

class WXDLLIMPEXP_SDK clProfileHandler : public wxEvtHandler
{
    wxStringSet_t m_cxxOutputTabs;
    wxStringSet_t m_cxxWorkspaceTabs;
    wxStringSet_t m_cxxOutputTabsToRestore;
    wxStringSet_t m_cxxWorkspaceTabsToRestore;

protected:
    clProfileHandler();
    virtual ~clProfileHandler();

    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent& e);
    void OnGoingDown(clCommandEvent& e);
    void HandleWorkspaceTabs(bool show);
    void HandleOutputTabs(bool show);

    bool IsPageExistsInBook(Notebook* book, const wxString& label) const;
    void RestoreTabs(wxStringSet_t& tabs, wxEventType eventType);
    void HideTabs(const wxStringSet_t& candidates, Notebook* book, wxEventType eventType, wxStringSet_t& tabsHidden);

public:
    static clProfileHandler& Get();
};

#endif // CLPROFILEHANDLER_H
