#include "clDockingManager.h"

#include "Notebook.h"
#include "codelite_events.h"
#include "detachedpanesinfo.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

#include <map>
#include <wx/aui/floatpane.h>
#include <wx/menu.h>

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
