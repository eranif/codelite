#include "GotoAnythingDlg.h"
#include "bitmap_loader.h"
#include "clGotoAnythingManager.h"
#include "clKeyboardManager.h"
#include "clWorkspaceManager.h"
#include "cl_command_event.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"
#include <algorithm>
#include <queue>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/stc/stc.h>

clGotoAnythingManager::clGotoAnythingManager()
{
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
    EventNotifier::Get()->Bind(wxEVT_GOTO_ANYTHING_SHOWING, &clGotoAnythingManager::OnShowing, this);
}

clGotoAnythingManager::~clGotoAnythingManager()
{
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &clGotoAnythingManager::OnActionSelected, this);
    EventNotifier::Get()->Unbind(wxEVT_GOTO_ANYTHING_SHOWING, &clGotoAnythingManager::OnShowing, this);
}

clGotoAnythingManager& clGotoAnythingManager::Get()
{
    static clGotoAnythingManager manager;
    return manager;
}

void clGotoAnythingManager::OnActionSelected(clGotoEvent& e)
{
    e.Skip();
    // Trigger the action
    const clGotoEntry& entry = e.GetEntry();
    if(entry.GetResourceID() != wxID_ANY) {
        wxCommandEvent evtAction(wxEVT_MENU, entry.GetResourceID());
        if(entry.IsCheckable()) {
            evtAction.SetInt(entry.IsChecked() ? 0 : 1); // Set the opposite value
        }
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

    wxFrame* mainFrame = EventNotifier::Get()->TopFrame();
    wxMenuBar* mb = mainFrame->GetMenuBar();
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

        // Call this to ensure that any checkable items are marked as "checked" if needed
        menu->UpdateUI(mainFrame->GetEventHandler());

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
                if(menuItem->IsCheck()) {
                    entry.SetFlags(clGotoEntry::kItemCheck);
                    entry.SetChecked(menuItem->IsChecked());
                }
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

void clGotoAnythingManager::DoAddCurrentTabActions(clGotoEntry::Vec_t& V)
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        {
            clGotoEntry entry(_("Current Tab > Close Tabs To The Right"), "", XRCID("close_tabs_to_the_right"));
            V.push_back(entry);
        }
        {
            clGotoEntry entry(_("Current Tab > Close Other Tabs"), "", XRCID("close_other_tabs"));
            V.push_back(entry);
        }
        {
            clGotoEntry entry(_("Current Tab > Duplicate Tab"), "", XRCID("duplicate_tab"));
            V.push_back(entry);
        }
        {
            clGotoEntry entry(_("Current Tab > Make it Read Only"), "", XRCID("mark_readonly"));
            // This a checkable item
            entry.SetCheckable(true);
            entry.SetChecked(editor->GetCtrl()->GetReadOnly());
            V.push_back(entry);
        }
        {
            clGotoEntry entry(_("Current Tab > Detach"), "", XRCID("ID_DETACH_EDITOR"));
            V.push_back(entry);
        }
        {
            clGotoEntry entry(_("Current Tab > Open Shell Here"), "", XRCID("open_shell_from_filepath"));
            V.push_back(entry);
        }
        {
            clGotoEntry entry(_("Current Tab > Open in File Explorer"), "", XRCID("open_file_explorer"));
            V.push_back(entry);
        }
        {
            clGotoEntry entry(_("Current Tab > Copy file name"), "", XRCID("copy_file_name_only"));
            V.push_back(entry);
        }
        {
            clGotoEntry entry(_("Current Tab > Copy fullpath (name, extension and directory)"), "",
                              XRCID("copy_file_name"));
            V.push_back(entry);
        }
        {
            clGotoEntry entry(_("Current Tab > Copy Path only (directory part only)"), "", XRCID("copy_file_path"));
            V.push_back(entry);
        }
        if(clWorkspaceManager::Get().IsWorkspaceOpened()) {
            clGotoEntry entry(_("Current Tab > Copy full path relative to workspace"), "",
                              XRCID("copy_file_relative_path_to_workspace"));
            V.push_back(entry);
        }
    }
}

void clGotoAnythingManager::OnShowing(clGotoEvent& e)
{
    e.Skip();
    // Add core actions which are not part of the menu bar
    clGotoEntry::Vec_t& V = e.GetEntries();
    DoAddCurrentTabActions(V);
}
