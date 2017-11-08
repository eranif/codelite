#include "GotoAnythingDlg.h"
#include "bitmap_loader.h"
#include "clGotoAnythingManager.h"
#include "clKeyboardManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include <algorithm>
#include <queue>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

clGotoAnythingManager::clGotoAnythingManager()
{
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
}

clGotoAnythingManager::~clGotoAnythingManager()
{
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
}

void clGotoAnythingManager::Add(const clGotoEntry& entry)
{
    static wxBitmap defaultBitmap = clGetManager()->GetStdIcons()->LoadBitmap("placeholder");

    Delete(entry);
    m_pluginActions[entry.GetDesc()] = entry;
    clGotoEntry& e = m_pluginActions[entry.GetDesc()];
    if(!e.GetBitmap().IsOk()) { e.SetBitmap(defaultBitmap); }
}

void clGotoAnythingManager::Delete(const clGotoEntry& entry)
{
    if(m_pluginActions.count(entry.GetDesc())) { m_pluginActions.erase(entry.GetDesc()); }
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
    } else if(m_pluginActions.count(e.GetString())) {
        e.Skip(false);

        // Trigger the action
        wxCommandEvent evtAction(wxEVT_MENU, m_pluginActions[e.GetString()].GetResourceID());
        EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evtAction);
    }
}

void clGotoAnythingManager::ShowDialog()
{
    GotoAnythingDlg dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

std::vector<clGotoEntry> clGotoAnythingManager::GetActions()
{
    Initialise();
    std::vector<clGotoEntry> actions;
    std::for_each(
        m_actions.begin(), m_actions.end(),
        [&](const std::unordered_map<wxString, clGotoEntry>::value_type& vt) { actions.push_back(vt.second); });
    std::for_each(
        m_pluginActions.begin(), m_pluginActions.end(),
        [&](const std::unordered_map<wxString, clGotoEntry>::value_type& vt) { actions.push_back(vt.second); });
    std::sort(actions.begin(), actions.end(),
              [&](const clGotoEntry& a, const clGotoEntry& b) { return a.GetDesc() < b.GetDesc(); });
    return actions;
}

void clGotoAnythingManager::Initialise()
{
    // Register the core actions
    m_actions.clear();

    wxMenuBar* mb = EventNotifier::Get()->TopFrame()->GetMenuBar();
    if(!mb) return;
    clDEBUG() << "clGotoAnythingManager::Initialise called." << (wxUIntPtr)this << clEndl;
    // Get list of menu entries
    std::queue<std::pair<wxString, wxMenu*> > q;
    for(size_t i = 0; i < mb->GetMenuCount(); ++i) {
        q.push(std::make_pair("", mb->GetMenu(i)));
    }

    static wxBitmap defaultBitmap = clGetManager()->GetStdIcons()->LoadBitmap("placeholder");
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
                if((labelText == "Recent Files") || (labelText == "Recent Workspaces")) { continue; }
                q.push(std::make_pair(menuItem->GetItemLabelText() + " > ", menuItem->GetSubMenu()));
            } else if((menuItem->GetId() != wxNOT_FOUND) && (menuItem->GetId() != wxID_SEPARATOR)) {
                clGotoEntry entry;
                wxString desc = menuItem->GetItemLabelText();
                entry.SetDesc(prefix + desc);
                if(menuItem->GetAccel()) { entry.SetKeyboardShortcut(menuItem->GetAccel()->ToString()); }
                entry.SetResourceID(menuItem->GetId());
                entry.SetBitmap(menuItem->GetBitmap().IsOk() ? menuItem->GetBitmap() : defaultBitmap);
                if(!entry.GetDesc().IsEmpty()) {
                    // Dont add empty entries
                    m_actions[entry.GetDesc()] = entry;
                }
            }
        }
    }
}
