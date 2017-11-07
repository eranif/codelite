#include "GotoAnythingDlg.h"
#include "clGotoAnythingManager.h"
#include "clKeyboardManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include <algorithm>
#include <queue>
#include <wx/xrc/xmlres.h>
#include <wx/menu.h>

clGotoAnythingManager::clGotoAnythingManager()
{
    Initialise();
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
}

clGotoAnythingManager::~clGotoAnythingManager()
{
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
}

void clGotoAnythingManager::Add(const clGotoEntry& entry)
{
    Delete(entry);
    m_actions[entry.GetDesc()] = entry;
}

void clGotoAnythingManager::Delete(const clGotoEntry& entry)
{
    if(m_actions.count(entry.GetDesc())) {
        m_actions.erase(entry.GetDesc());
    }
}

clGotoAnythingManager& clGotoAnythingManager::Get()
{
    static clGotoAnythingManager manager;
    return manager;
}

void clGotoAnythingManager::OnActionSelected(clCommandEvent& e)
{
    e.Skip();
    if(m_actions.count(e.GetString())) {
        e.Skip(false);

        // Trigger the action
        wxCommandEvent evtAction(wxEVT_MENU, m_actions[e.GetString()].GetResourceID());
        EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evtAction);
    }
}

void clGotoAnythingManager::ShowDialog()
{
    GotoAnythingDlg dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

std::vector<clGotoEntry> clGotoAnythingManager::GetActions() const
{
    std::vector<clGotoEntry> actions;
    std::for_each(
        m_actions.begin(), m_actions.end(), [&](const std::unordered_map<wxString, clGotoEntry>::value_type& vt) {
            actions.push_back(vt.second);
        });
    std::sort(actions.begin(), actions.end(), [&](const clGotoEntry& a, const clGotoEntry& b) {
        return a.GetDesc() < b.GetDesc();
    });
    return actions;
}

void clGotoAnythingManager::Initialise()
{
    // Register the common actions
    m_actions.clear();

    wxMenuBar* mb = EventNotifier::Get()->TopFrame()->GetMenuBar();
    if(!mb) return;

    // Get list of menu entries
    std::queue<std::pair<wxString, wxMenu*> > q;
    for(size_t i = 0; i < mb->GetMenuCount(); ++i) {
        q.push(std::make_pair("", mb->GetMenu(i)));
    }

    while(!q.empty()) {
        wxMenu* menu = q.front().second;
        wxString prefix = q.front().first;
        q.pop();

        const wxMenuItemList& L = menu->GetMenuItems();
        wxMenuItemList::const_iterator iter = L.begin();
        for(; iter != L.end(); ++iter) {
            wxMenuItem* menuItem = *iter;
            if(menuItem->GetSubMenu()) {
                wxString labelText = menuItem->GetItemLabelText();
                if((labelText == "Recent Files") || (labelText == "Recent Workspaces")) {
                    continue;
                }
                q.push(std::make_pair(menuItem->GetItemLabelText() + " > ", menuItem->GetSubMenu()));
            } else if((menuItem->GetId() != wxNOT_FOUND) && (menuItem->GetId() != wxID_SEPARATOR)) {
                clGotoEntry entry;
                wxString desc = menuItem->GetItemLabelText();
                entry.SetDesc(prefix + desc);
                if(menuItem->GetAccel()) {
                    entry.SetKeyboardShortcut(menuItem->GetAccel()->ToString());
                }
                entry.SetResourceID(menuItem->GetId());
                if(!entry.GetDesc().IsEmpty()) {
                    // Dont add empty entries
                    m_actions[entry.GetDesc()] = entry;
                }
            }
        }
    }
}
