#include "GotoAnythingDlg.h"
#include "clGotoAnythingManager.h"
#include "clKeyboardManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include <algorithm>
#include <wx/xrc/xmlres.h>

clGotoAnythingManager::clGotoAnythingManager()
{
    // Register the common actions
    MenuItemDataMap_t accels;
    clKeyboardManager::Get()->GetAllAccelerators(accels);

    std::for_each(accels.begin(), accels.end(), [&](const MenuItemDataMap_t::value_type& vt) {
        const MenuItemData& itemData = vt.second;
        if(itemData.parentMenu != "Edit" && !itemData.parentMenu.IsEmpty()) {
            m_actions[itemData.action] =
                clGotoEntry(itemData.action, itemData.accel, wxXmlResource::GetXRCID(itemData.resourceID, wxNOT_FOUND));
        }
    });
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
    if(m_actions.count(entry.GetDesc())) { m_actions.erase(entry.GetDesc()); }
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
        m_actions.begin(), m_actions.end(),
        [&](const std::unordered_map<wxString, clGotoEntry>::value_type& vt) { actions.push_back(vt.second); });
    std::sort(actions.begin(), actions.end(),
              [&](const clGotoEntry& a, const clGotoEntry& b) { return a.GetDesc() < b.GetDesc(); });
    return actions;
}
