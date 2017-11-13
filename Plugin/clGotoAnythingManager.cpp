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
#include "cl_command_event.h"
#include "codelite_events.h"

clGotoAnythingManager::clGotoAnythingManager()
{
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
}

clGotoAnythingManager::~clGotoAnythingManager()
{
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
}

clGotoAnythingManager& clGotoAnythingManager::Get()
{
    static clGotoAnythingManager manager;
    return manager;
}

void clGotoAnythingManager::OnActionSelected(clGotoEvent& e)
{
    e.Skip();
    if(m_actions.count(e.GetString())) {
        // Trigger the action
        wxCommandEvent evtAction(wxEVT_MENU, m_actions[e.GetString()].GetResourceID());
        EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(evtAction);
    }
}

void clGotoAnythingManager::ShowDialog()
{
    // Let the plugins know that we are about to display the actions
    clGotoEvent evtShowing(wxEVT_GOTO_ANYTHING_SHOWING);
    evtShowing.SetEntries(GetActions());
    EventNotifier::Get()->ProcessEvent(evtShowing);
    
    // Let the plugins sort the content
    clGotoEvent evtSort(wxEVT_GOTO_ANYTHING_SORT_NEEDED);
    evtSort.GetEntries().swap(evtShowing.GetEntries());
    EventNotifier::Get()->ProcessEvent(evtSort);

    std::vector<clGotoEntry> entries = evtSort.GetEntries();
    GotoAnythingDlg dlg(EventNotifier::Get()->TopFrame(), entries);
    dlg.ShowModal();
}

std::vector<clGotoEntry> clGotoAnythingManager::GetActions()
{
    Initialise();
    std::vector<clGotoEntry> actions;
    std::for_each(
        m_actions.begin(), m_actions.end(),
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
    clDEBUG() << "clGotoAnythingManager::Initialise called." << (wxUIntPtr) this << clEndl;
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
                entry.SetBitmap(menuItem->GetBitmap().IsOk() ? menuItem->GetBitmap() : defaultBitmap);
                if(!entry.GetDesc().IsEmpty()) {
                    // Dont add empty entries
                    m_actions[entry.GetDesc()] = entry;
                }
            }
        }
    }
}
