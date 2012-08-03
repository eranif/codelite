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
#include <outline_settings.h>
#include "iconfigtool.h"
#include "detachedpanesinfo.h"
#include <wx/menu.h>
#include <wx/log.h>
#include <wx/tokenzr.h>
#include "macros.h"
#include "workspace.h"
#include "ctags_manager.h"
#include "outline.h"
#include "fileextmanager.h"
#include <wx/busyinfo.h>
#include <wx/utils.h>
#include <wx/xrc/xmlres.h>
#include "parse_thread.h"
#include "outline_symbol_tree.h"

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
    info.SetName(wxT("Outline"));
    info.SetDescription(_("Show Current the Layout of the current file"));
    info.SetVersion(wxT("v1.0"));

    // this plugin starts as disabled by default
    info.SetEnabled(true);
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
    m_longName = _("Outline Plugin");
    m_shortName = wxT("Outline");
    
    OutlineSettings os;
    os.Load();
    
    Notebook *book = m_mgr->GetWorkspacePaneNotebook();
    if( IsPaneDetached() ) {
        // Make the window child of the main panel (which is the grand parent of the notebook)
        DockablePane *cp = new DockablePane(book->GetParent()->GetParent(), book, wxT("Outline"), wxNullBitmap, wxSize(200, 200));
        m_view = new OutlineTab(cp, m_mgr);
        cp->SetChildNoReparent(m_view);

    } else {
        m_view = new OutlineTab(book, m_mgr);
        int index = os.GetTabIndex();
        if(index == wxNOT_FOUND)
            book->AddPage(m_view, wxT("Outline"), false);
			
        else {
			size_t count = book->GetPageCount();
			if(index >= count) {
				// Invalid index
				book->AddPage(m_view, wxT("Outline"), false);
				
			} else {
				book->InsertPage(index, m_view, wxT("Outline"), false);
			}
		}
    }
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
    int index = DoFindTabIndex();
    size_t where = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_view);
    if ( where != Notebook::npos ) {
        // this window might be floating
        m_mgr->GetWorkspacePaneNotebook()->RemovePage(where);
    }
    
    // Save the real position
    OutlineSettings os;
    os.Load();
    os.SetTabIndex(index);
    os.Save();
    
    m_view->Destroy();
    m_view = NULL;
}

bool SymbolViewPlugin::IsPaneDetached()
{
    DetachedPanesInfo dpi;
    m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    wxArrayString detachedPanes = dpi.GetPanes();
    return detachedPanes.Index(wxT("Outline")) != wxNOT_FOUND;
}

int SymbolViewPlugin::DoFindTabIndex()
{
    std::vector<wxWindow*> windows;
    Notebook *book = m_mgr->GetWorkspacePaneNotebook();
    
#if !CL_USE_NATIVEBOOK

    book->GetEditorsInOrder(windows);
    
#else
    
    for(size_t i=0; i<book->GetPageCount(); i++) {
        windows.push_back( book->GetPage(i) );
    }
    
#endif
    for(size_t i=0; i<windows.size(); i++) {
        if(windows.at(i) == m_view )
            return i;
    }
    return wxNOT_FOUND;
}
