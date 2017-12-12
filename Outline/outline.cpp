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

#include "cl_command_event.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "detachedpanesinfo.h"
#include "event_notifier.h"
#include "fileextmanager.h"
#include "iconfigtool.h"
#include "macros.h"
#include "outline.h"
#include "outline_settings.h"
#include "outline_symbol_tree.h"
#include "parse_thread.h"
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
    if(thePlugin == 0) { thePlugin = new SymbolViewPlugin(manager); }
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

    OutlineSettings os;
    os.Load();

    Notebook* book = m_mgr->GetWorkspacePaneNotebook();
    if(IsPaneDetached()) {
        // Make the window child of the main panel (which is the grand parent of the notebook)
        DockablePane* cp =
            new DockablePane(book->GetParent()->GetParent(), book, _("Outline"), false, wxNullBitmap, wxSize(200, 200));
        m_view = new OutlineTab(cp, m_mgr);
        cp->SetChildNoReparent(m_view);
        m_view->m_isEnabled = true; // Enabled when detached
    } else {
        m_view = new OutlineTab(book, m_mgr);
        book->AddPage(m_view, _("Outline"), false);
    }
    EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &SymbolViewPlugin::OnToggleTab, this);
    m_mgr->AddWorkspaceTab(_("Outline"));
    m_mgr->GetWorkspacePaneNotebook()->Bind(wxEVT_BOOK_PAGE_CHANGED, &SymbolViewPlugin::OnPageChanged, this);
}

SymbolViewPlugin::~SymbolViewPlugin() { thePlugin = NULL; }

clToolBar* SymbolViewPlugin::CreateToolBar(wxWindow* parent) { return NULL; }

void SymbolViewPlugin::CreatePluginMenu(wxMenu* pluginsMenu) { wxUnusedVar(pluginsMenu); }

void SymbolViewPlugin::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_SHOW_WORKSPACE_TAB, &SymbolViewPlugin::OnToggleTab, this);
    m_mgr->GetWorkspacePaneNotebook()->Unbind(wxEVT_BOOK_PAGE_CHANGED, &SymbolViewPlugin::OnPageChanged, this);
    int where = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_view);
    if(where != wxNOT_FOUND) {
        // this window might be floating
        m_mgr->GetWorkspacePaneNotebook()->RemovePage(where);
    }

    m_view->Destroy();
    m_view = NULL;
}

bool SymbolViewPlugin::IsPaneDetached()
{
    DetachedPanesInfo dpi;
    m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    wxArrayString detachedPanes = dpi.GetPanes();
    return detachedPanes.Index(_("Outline")) != wxNOT_FOUND;
}

int SymbolViewPlugin::DoFindTabIndex() { return m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_view); }

void SymbolViewPlugin::OnToggleTab(clCommandEvent& event)
{
    if(event.GetString() != _("Outline")) {
        event.Skip();
        return;
    }

    if(event.IsSelected()) {
        // show it
        m_mgr->GetWorkspacePaneNotebook()->AddPage(m_view, _("Outline"), false);
    } else {
        int where = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(_("Outline"));
        if(where != wxNOT_FOUND) { m_mgr->GetWorkspacePaneNotebook()->RemovePage(where); }
    }
}

void SymbolViewPlugin::OnPageChanged(wxBookCtrlEvent& e)
{
    e.Skip();
    if(m_view->IsShown()) {
        m_view->m_isEnabled = true;
    } else if(IsPaneDetached()) {
        m_view->m_isEnabled = true;
    } else {
        m_view->m_isEnabled = false;
        int sel = m_mgr->GetWorkspacePaneNotebook()->GetSelection();
        if(sel != wxNOT_FOUND) {
            wxString seletionText = m_mgr->GetWorkspacePaneNotebook()->GetPageText(sel);
            bool oldState = m_view->m_isEnabled;
            m_view->m_isEnabled = (seletionText == _("Outline"));
            if(oldState != m_view->m_isEnabled) {
                // Refresh the view
                m_view->EditorChanged();
            }
        } else {
            // the page is detached
            m_view->m_isEnabled = true; // just mark as active
        }
    }
}
