//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : databaseexplorer.cpp
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

#include "ErdPanel.h"
#include "SqlCommandPanel.h"
#include "clKeyboardManager.h"
#include "databaseexplorer.h"
#include "detachedpanesinfo.h"
#include "dockablepane.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "wx/wxsf/AutoLayout.h"
#include <wx/aboutdlg.h>
#include <wx/xrc/xmlres.h>

//#ifdef DBL_USE_MYSQL
#include "MySqlDbAdapter.h"
//#endif

//#ifdef DBL_USE_SQLITE
#include "SqliteDbAdapter.h"
//#endif

//#ifdef DBL_USE_POSTGRES
#include "PostgreSqlDbAdapter.h"
//#endif

#define DBE_VERSION "0.5.3 Beta"

static DatabaseExplorer* thePlugin = NULL;

DbViewerPanel* DatabaseExplorer::m_dbViewerPanel = NULL;

IManager* DatabaseExplorer::GetManager() { return thePlugin->m_mgr; }

enum wxbuildinfoformat { short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if(format == long_f) {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new DatabaseExplorer(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Peter Janků, Michal Bližňák, Tomas Bata University in Zlin, Czech Republic (www.fai.utb.cz)"));
    info.SetName(_("DatabaseExplorer"));
    info.SetDescription(_("DatabaseExplorer for CodeLite"));
    info.SetVersion(DBE_VERSION);
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

DatabaseExplorer::DatabaseExplorer(IManager* manager)
    : IPlugin(manager)
{

    // create tab (possibly detached)
    Notebook* book = m_mgr->GetWorkspacePaneNotebook();
    wxWindow* editorBook = m_mgr->GetEditorPaneNotebook();

    EventNotifier::Get()->Connect(wxEVT_TREE_ITEM_FILE_ACTIVATED,
                                  clCommandEventHandler(DatabaseExplorer::OnOpenWithDBE), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &DatabaseExplorer::OnToggleTab, this);

    if(IsDbViewDetached()) {
        DockablePane* cp = new DockablePane(book->GetParent()->GetParent(), book, _("DbExplorer"), false, wxNullBitmap,
                                            wxSize(200, 200));
        m_dbViewerPanel = new DbViewerPanel(cp, editorBook, m_mgr);
        cp->SetChildNoReparent(m_dbViewerPanel);

    } else {

        m_dbViewerPanel = new DbViewerPanel(book, editorBook, m_mgr);
        // size_t index = GetSettings().GetSvnTabIndex();
        // if(index == Notebook::npos)
        book->AddPage(m_dbViewerPanel, _("DbExplorer"), false);
        // else
        //	book->InsertPage(index, m_dbViewerPanel, svnCONSOLE_TEXT, false);
    }
    m_mgr->AddWorkspaceTab(_("DbExplorer"));

    // configure autolayout algorithns
    wxSFAutoLayout layout;

    wxSFLayoutHorizontalTree* pHTreeAlg =
        wxDynamicCast(layout.GetAlgorithm(wxT("Horizontal Tree")), wxSFLayoutHorizontalTree);
    if(pHTreeAlg) pHTreeAlg->SetHSpace(200);

    wxSFLayoutVerticalTree* pVTreeAlg =
        wxDynamicCast(layout.GetAlgorithm(wxT("Vertical Tree")), wxSFLayoutVerticalTree);
    if(pVTreeAlg) pVTreeAlg->SetVSpace(75);

    m_longName = _("DatabaseExplorer for CodeLite");
    m_shortName = wxT("DatabaseExplorer");

    clKeyboardManager::Get()->AddGlobalAccelerator("wxEVT_EXECUTE_SQL", "Ctrl-J", _("Execute SQL"));
    wxTheApp->Bind(wxEVT_MENU, &DatabaseExplorer::OnExecuteSQL, this, XRCID("wxEVT_EXECUTE_SQL"));
}

DatabaseExplorer::~DatabaseExplorer() { wxSFAutoLayout::CleanUp(); }

void DatabaseExplorer::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void DatabaseExplorer::CreatePluginMenu(wxMenu* pluginsMenu)
{

    // You can use the below code a snippet:
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, XRCID("dbe_about"), _("About..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    item = new wxMenuItem(menu, XRCID("wxEVT_EXECUTE_SQL"), _("Execute SQL"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Database Explorer"), menu);
    m_mgr->GetTheApp()->Connect(XRCID("dbe_about"), wxEVT_COMMAND_MENU_SELECTED,
                                wxCommandEventHandler(DatabaseExplorer::OnAbout), NULL, this);
}

void DatabaseExplorer::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void DatabaseExplorer::OnExecuteSQL(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // Pass the command to the active frame
}

void DatabaseExplorer::UnPlug()
{
    EventNotifier::Get()->Disconnect(wxEVT_TREE_ITEM_FILE_ACTIVATED,
                                     clCommandEventHandler(DatabaseExplorer::OnOpenWithDBE), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_SHOW_WORKSPACE_TAB, &DatabaseExplorer::OnToggleTab, this);
    int index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_dbViewerPanel);
    if(index != wxNOT_FOUND) { m_mgr->GetWorkspacePaneNotebook()->RemovePage(index); }
    wxTheApp->Unbind(wxEVT_MENU, &DatabaseExplorer::OnExecuteSQL, this, XRCID("wxEVT_EXECUTE_SQL"));
    wxDELETE(m_dbViewerPanel);
}

bool DatabaseExplorer::IsDbViewDetached()
{
    DetachedPanesInfo dpi;
    m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
    wxArrayString detachedPanes = dpi.GetPanes();

    return detachedPanes.Index(_("DbExplorer")) != wxNOT_FOUND;
}

void DatabaseExplorer::OnAbout(wxCommandEvent& e)
{
    wxString version = wxString::Format(DBE_VERSION);
    wxString desc = _("Cross platform database explorer\n\n");
    desc << wxbuildinfo(long_f) << wxT("\n\n");
    
    wxAboutDialogInfo info;
    info.SetName(_("DatabaseExplorer"));
    info.SetVersion(version);
    info.SetDescription(desc);
    info.SetCopyright(_("2011 - 2015 (C) Tomas Bata University, Zlin, Czech Republic"));
    info.SetWebSite(_("http://www.fai.utb.cz"));
    info.AddDeveloper(wxT("Peter Janků"));
    info.AddDeveloper(wxT("Michal Bližňák"));

    wxAboutBox(info);
}

void DatabaseExplorer::OnOpenWithDBE(clCommandEvent& e)
{
    // get the file name
    e.Skip();
    if(FileExtManager::IsFileType(e.GetFileName(), FileExtManager::TypeDatabase)) {
        e.Skip(false);
        // Open the databse file
        DoOpenFile(e.GetFileName());
    }
}

void DatabaseExplorer::DoOpenFile(const wxFileName& filename)
{
    if(m_dbViewerPanel) {
        m_dbViewerPanel->OpenSQLiteFile(filename, true);
    }
}

void DatabaseExplorer::OnToggleTab(clCommandEvent& event)
{
    if(event.GetString() != _("DbExplorer")) {
        event.Skip();
        return;
    }

    if(event.IsSelected()) {
        // show it
        clGetManager()->GetWorkspacePaneNotebook()->AddPage(m_dbViewerPanel, _("DbExplorer"), true);
    } else {
        int where = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(_("DbExplorer"));
        if(where != wxNOT_FOUND) { clGetManager()->GetWorkspacePaneNotebook()->RemovePage(where); }
    }
}
