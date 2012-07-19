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

#include <set>
#include <wx/app.h>
#include <wx/wupdlock.h>
#include "event_notifier.h"
#include <wx/settings.h>
#include <wx/menu.h>
#include <wx/log.h>
#include <wx/tokenzr.h>
#include "macros.h"
#include "detachedpanesinfo.h"
#include "workspace.h"
#include "ctags_manager.h"
#include "symbolview.h"
#include "fileextmanager.h"
#include <wx/busyinfo.h>
#include <wx/utils.h>
#include <wx/xrc/xmlres.h>
#include "parse_thread.h"
#include "sv_symbol_tree.h"

//--------------------------------------------
//Plugin Interface
//--------------------------------------------

static SymbolViewPlugin* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new SymbolViewPlugin(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("Outline Plugin"));
	info.SetDescription(_("Show Current the Layout of the current file"));
	info.SetVersion(wxT("v1.0"));

	// this plugin starts as disabled by default
	info.SetEnabled(false);
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}


//--------------------------------------------
//Constructors/Destructors
//--------------------------------------------

SymbolViewPlugin::SymbolViewPlugin(IManager *manager)
		: IPlugin(manager)
{
	m_longName = _("Symbols View Plugin");
	m_shortName = wxT("SymbolView");
    
    Notebook *book = m_mgr->GetWorkspacePaneNotebook();
    m_view = new SymbolViewTabPanel(book, m_mgr);
    book->AddPage(m_view, _("Outline"), false);
}

SymbolViewPlugin::~SymbolViewPlugin()
{
	thePlugin = NULL;
}

clToolBar *SymbolViewPlugin::CreateToolBar(wxWindow *parent)
{
	return NULL;
}

void SymbolViewPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxUnusedVar(pluginsMenu);
}

void SymbolViewPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
        
	} else if (type == MenuTypeFileExplorer) {
        
	} else if (type == MenuTypeFileView_Workspace) {
        
	} else if (type == MenuTypeFileView_Project) {
        
	} else if (type == MenuTypeFileView_Folder) {
        
	} else if (type == MenuTypeFileView_File) {
	}
}

void SymbolViewPlugin::UnPlug()
{
    size_t index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(_("Outline"));
    if (index != Notebook::npos) {
		// this window might be floating
        m_mgr->GetWorkspacePaneNotebook()->RemovePage(index, false);
    }
    m_view->Destroy();
	m_view = NULL;
}
