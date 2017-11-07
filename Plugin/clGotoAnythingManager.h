#ifndef CLGOTOANYTHINGMANAGER_H
#define CLGOTOANYTHINGMANAGER_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "macros.h"
#include <map>
#include <vector>
#include <wx/event.h>

class WXDLLIMPEXP_SDK clGotoEntry
{
private:
    wxString m_desc;
    wxString m_keyboardShortcut;
    int m_resourceID;

public:
    clGotoEntry(const wxString& desc, const wxString& shortcut, int id)
        : m_desc(desc)
        , m_keyboardShortcut(shortcut)
        , m_resourceID(id)
    {
    }
    clGotoEntry()
        : m_resourceID(wxNOT_FOUND)
    {
    }

    void SetDesc(const wxString& desc) { this->m_desc = desc; }
    void SetKeyboardShortcut(const wxString& keyboardShortcut) { this->m_keyboardShortcut = keyboardShortcut; }
    void SetResourceID(int resourceID) { this->m_resourceID = resourceID; }
    const wxString& GetDesc() const { return m_desc; }
    const wxString& GetKeyboardShortcut() const { return m_keyboardShortcut; }
    int GetResourceID() const { return m_resourceID; }
};

class WXDLLIMPEXP_SDK clGotoAnythingManager : public wxEvtHandler
{
    std::unordered_map<wxString, clGotoEntry> m_actions;

    clGotoAnythingManager();
    virtual ~clGotoAnythingManager();

protected:
    void OnActionSelected(clCommandEvent& e);

private:
    void Initialise();
    
public:
    static clGotoAnythingManager& Get();

    /**
     * @brief add action to the "Goto Anything"
     */
    void Add(const clGotoEntry& entry);
    /**
     * @brief delete entry by its name
     */
    void Delete(const clGotoEntry& entry);

    /**
     * @brief show the "Goto Anything" dialog
     */
    void ShowDialog();

    /**
     * @brief return list of all available actions
     */
    std::vector<clGotoEntry> GetActions() const;
};

#endif // CLGOTOANYTHINGMANAGER_H
