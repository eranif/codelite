#ifndef CLGOTOANYTHINGMANAGER_H
#define CLGOTOANYTHINGMANAGER_H

#include "codelite_exports.h"
#include "macros.h"
#include <wx/event.h>
#include <vector>
#include "cl_command_event.h"

class WXDLLIMPEXP_SDK clGotoAnythingManager : public wxEvtHandler
{
    std::unordered_map<wxString, int> m_coreActions;
    wxStringSet_t m_pluginActions;

    clGotoAnythingManager();
    virtual ~clGotoAnythingManager();

protected:
    void OnActionSelected(clCommandEvent& e);

public:
    static clGotoAnythingManager& Get();

    /**
     * @brief add action to the "Goto Anything"
     */
    void Add(const wxString& entry);
    /**
     * @brief delete entry by its name
     */
    void Delete(const wxString& entry);
    
    /**
     * @brief show the "Goto Anything" dialog
     */
    void ShowDialog();
    
    /**
     * @brief return list of all available actions
     */
    std::vector<wxString> GetActions() const;
};

#endif // CLGOTOANYTHINGMANAGER_H
