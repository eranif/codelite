#ifndef CLGOTOANYTHINGMANAGER_H
#define CLGOTOANYTHINGMANAGER_H

#include "clGotoEntry.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "macros.h"

#include <map>
#include <vector>
#include <wx/bitmap.h>
#include <wx/event.h>

class WXDLLIMPEXP_SDK clGotoAnythingManager : public wxEvtHandler
{
    std::unordered_map<wxString, clGotoEntry> m_actions;

    clGotoAnythingManager();
    virtual ~clGotoAnythingManager();

protected:
    void OnActionSelected(clGotoEvent& e);
    void OnShowing(clGotoEvent& e);

    void DoAddCurrentTabActions(clGotoEntry::Vec_t& V);

public:
    static clGotoAnythingManager& Get();

    /**
     * @brief fill the gotomanager with all the menu entries
     */
    void Initialise();

    /**
     * @brief show the "Goto Anything" dialog
     */
    void ShowDialog();

    /**
     * @brief return list of all available actions
     */
    std::vector<clGotoEntry> GetActions();
};

#endif // CLGOTOANYTHINGMANAGER_H
