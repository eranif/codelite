#ifndef CLDOCKINGMANAGER_H
#define CLDOCKINGMANAGER_H

#include "codelite_exports.h"
#include <wx/aui/framemanager.h>

class Notebook;
class WXDLLIMPEXP_SDK clDockingManager : public wxAuiManager
{
protected:
    void OnRender(wxAuiManagerEvent& event);
    void OnButtonClicked(wxAuiManagerEvent& event);
    
    void ShowWorkspaceOpenTabMenu();
    void ShowOutputViewOpenTabMenu();
    
    /**
     * @brief show a popup menu and return the selected string
     * return an empty string if no selection was made
     */
    wxString ShowMenu(wxWindow* win, const wxArrayString& tabs, Notebook* book, bool& checked);

public:
    clDockingManager();
    virtual ~clDockingManager();
};

#endif // CLDOCKINGMANAGER_H
