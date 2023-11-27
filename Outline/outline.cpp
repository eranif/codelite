//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : symbolview.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "outline.h"

#include "cl_command_event.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "detachedpanesinfo.h"
#include "event_notifier.h"
#include "fileextmanager.h"
#include "iconfigtool.h"
#include "macros.h"
#include "workspace.h"

#include <set>
#include <wx/app.h>
#include <wx/busyinfo.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

//--------------------------------------------
// Plugin Interface
//--------------------------------------------

static SymbolViewPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new SymbolViewPlugin(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("Outline"));
    info.SetDescription(_("Show Current the Layout of the current file"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

//--------------------------------------------
// Constructors/Destructors
//--------------------------------------------

SymbolViewPlugin::SymbolViewPlugin(IManager* manager)
    : IPlugin(manager)
{
    m_longName = _("Outline Plugin");
    m_shortName = wxT("Outline");

    m_view = new OutlineTab(m_mgr->BookGet(PaneId::SIDE_BAR));
    m_mgr->BookAddPage(PaneId::SIDE_BAR, m_view, _("Outline"),
                       clLoadSidebarBitmap("outline-button", clGetManager()->BookGet(PaneId::SIDE_BAR)));
    m_mgr->AddWorkspaceTab(_("Outline"));
}

SymbolViewPlugin::~SymbolViewPlugin() { thePlugin = NULL; }

void SymbolViewPlugin::CreateToolBar(clToolBarGeneric* toolbar) { wxUnusedVar(toolbar); }

void SymbolViewPlugin::CreatePluginMenu(wxMenu* pluginsMenu) { wxUnusedVar(pluginsMenu); }

void SymbolViewPlugin::UnPlug()
{
    if(!m_mgr->BookDeletePage(PaneId::SIDE_BAR, m_view)) {
        // failed to delete, delete it manually
        m_view->Destroy();
    }
    m_view = nullptr;
}
