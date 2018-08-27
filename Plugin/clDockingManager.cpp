#include "clDockingManager.h"
#include "imanager.h"
#include "globals.h"
#include "Notebook.h"
#include <wx/menu.h>
#include <wx/aui/floatpane.h>
#include <map>
#include "codelite_events.h"
#include "event_notifier.h"
#include "detachedpanesinfo.h"
#include "editor_config.h"

#define WORKSPACE_VIEW "Workspace View"
#define OUTPUT_VIEW "Output View"

clDockingManager::clDockingManager()
{
    Bind(wxEVT_AUI_PANE_BUTTON, &clDockingManager::OnButtonClicked, this);
    Bind(wxEVT_AUI_RENDER, &clDockingManager::OnRender, this);
    Bind(wxEVT_AUI_PANE_ACTIVATED, &clDockingManager::OnAuiPaneActivated, this);
}

clDockingManager::~clDockingManager()
{
    Unbind(wxEVT_AUI_PANE_BUTTON, &clDockingManager::OnButtonClicked, this);
    Unbind(wxEVT_AUI_RENDER, &clDockingManager::OnRender, this);
    Unbind(wxEVT_AUI_PANE_ACTIVATED, &clDockingManager::OnAuiPaneActivated, this);
}

void clDockingManager::OnRender(wxAuiManagerEvent& event) { event.Skip(); }

void clDockingManager::OnButtonClicked(wxAuiManagerEvent& event)
{
    event.Skip();
    // if(event.GetButton() != wxAUI_BUTTON_PIN) {
    //     event.Skip();
    //     return;
    // }
    // event.Veto();
    // event.Skip(false);
    // 
    // if(event.GetPane()->name == OUTPUT_VIEW) {
    //     CallAfter(&clDockingManager::ShowOutputViewOpenTabMenu);
    // } else {
    //     CallAfter(&clDockingManager::ShowWorkspaceOpenTabMenu);
    // }
}

void clDockingManager::OnAuiPaneActivated(wxAuiManagerEvent& e)
{
    e.Skip();

    // If a pane is floating, copy the accelerator table to it.
    const auto pane = e.GetPane();
    if(pane && pane->frame && pane->IsFloating()) {
        // wxGetRealTopLevelParent(pane->frame) doesn't work since the detached pane
        // has IsTopLevel() == true, so wxGetRealTopLevelParent() returns the parameter.
        const auto parent = ::clGetManager()->GetTheApp()->GetTopWindow();
        if(parent) {
            const auto accelTable = parent->GetAcceleratorTable();
            if(accelTable) {
                pane->frame->SetAcceleratorTable(*accelTable);
            }
        }
    }
}

void clDockingManager::ShowWorkspaceOpenTabMenu()
{
    // Override the "pin" button to show a window list
    bool show;
    wxString selection = ShowMenu(GetPane(WORKSPACE_VIEW).window,
                                  clGetManager()->GetWorkspaceTabs(),
                                  clGetManager()->GetWorkspacePaneNotebook(),
                                  show);
    CHECK_COND_RET(!selection.IsEmpty());
    clCommandEvent event(wxEVT_SHOW_WORKSPACE_TAB);
    event.SetSelected(show).SetString(selection);
    EventNotifier::Get()->AddPendingEvent(event);
}

void clDockingManager::ShowOutputViewOpenTabMenu()
{
    bool show;
    wxString selection = ShowMenu(
        GetPane(OUTPUT_VIEW).window, clGetManager()->GetOutputTabs(), clGetManager()->GetOutputPaneNotebook(), show);
    CHECK_COND_RET(!selection.IsEmpty());
    clCommandEvent event(wxEVT_SHOW_OUTPUT_TAB);
    event.SetSelected(show).SetString(selection);
    EventNotifier::Get()->AddPendingEvent(event);
}

wxString clDockingManager::ShowMenu(wxWindow* win, const wxArrayString& tabs, Notebook* book, bool& checked)
{
    DetachedPanesInfo dpi;
    EditorConfigST::Get()->ReadObject("DetachedPanesList", &dpi);

    std::map<int, wxString> tabsIds;
    wxMenu menu(_("Toggle Tabs"));
    for(size_t i = 0; i < tabs.size(); ++i) {
        int tabId = ::wxNewId();
        const wxString& label = tabs.Item(i);
        tabsIds.insert(std::make_pair(tabId, label));
        wxMenuItem* item = new wxMenuItem(&menu, tabId, label, "", wxITEM_CHECK);
        menu.Append(item);
        item->Check((book->GetPageIndex(label) != wxNOT_FOUND));

        // Output pane does not support "detach"
        if((book != clGetManager()->GetOutputPaneNotebook()) && dpi.GetPanes().Index(label) != wxNOT_FOUND) {
            item->Enable(false);
        }
    }

    int sel = win->GetPopupMenuSelectionFromUser(menu);
    if(sel == wxID_NONE) return "";
    const wxString& label = tabsIds.find(sel)->second;
    checked = (book->GetPageIndex(label) == wxNOT_FOUND);
    return label;
}
