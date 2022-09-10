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

clDockingManager::clDockingManager() { Bind(wxEVT_AUI_RENDER, &clDockingManager::OnRender, this); }

clDockingManager::~clDockingManager() { Unbind(wxEVT_AUI_RENDER, &clDockingManager::OnRender, this); }

void clDockingManager::OnRender(wxAuiManagerEvent& event) { event.Skip(); }
