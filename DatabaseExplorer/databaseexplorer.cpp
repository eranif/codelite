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

#include <wx/aboutdlg.h>
#include "databaseexplorer.h"
#include "detachedpanesinfo.h"
#include "dockablepane.h"
#include "ErdPanel.h"
#include <wx/xrc/xmlres.h>
#include "wx/wxsf/AutoLayout.h"
#include "event_notifier.h"

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
    if(thePlugin == 0) {
        thePlugin = new DatabaseExplorer(manager);
    }
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

    EventNotifier::Get()->Connect(
        wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEventHandler(DatabaseExplorer::OnOpenWithDBE), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_SHOW_WORKSPACE_TAB, &DatabaseExplorer::OnToggleTab, this);

    if(IsDbViewDetached()) {
        DockablePane* cp =
            new DockablePane(book->GetParent()->GetParent(), book, _("DbExplorer"), wxNullBitmap, wxSize(200, 200));
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
}

DatabaseExplorer::~DatabaseExplorer() { wxSFAutoLayout::CleanUp(); }

clToolBar* DatabaseExplorer::CreateToolBar(wxWindow* parent)
{
    // Create the toolbar to be used by the plugin
    clToolBar* tb(NULL);
    return tb;
}

void DatabaseExplorer::CreatePluginMenu(wxMenu* pluginsMenu)
{

    // You can use the below code a snippet:
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, XRCID("dbe_about"), _("About..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("Database Explorer"), menu);
    m_mgr->GetTheApp()->Connect(
        XRCID("dbe_about"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DatabaseExplorer::OnAbout), NULL, this);
}

void DatabaseExplorer::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void DatabaseExplorer::UnPlug()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_TREE_ITEM_FILE_ACTIVATED, clCommandEventHandler(DatabaseExplorer::OnOpenWithDBE), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_SHOW_WORKSPACE_TAB, &DatabaseExplorer::OnToggleTab, this);
    int index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_dbViewerPanel);
    if(index != wxNOT_FOUND) {
        m_mgr->GetWorkspacePaneNotebook()->RemovePage(index);
    }
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

    wxString desc = _("Cross-platform database plugin designed for managing data, ERD and code generation.\n\n");
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
    const wxFileName& filename = e.GetFileName();
    if(filename.GetExt() == wxT("erd")) {
        e.Skip(false);
        DoOpenFile(filename);
    }
}

void DatabaseExplorer::DoOpenFile(const wxFileName& filename)
{
    if(filename.GetExt() == wxT("erd")) {
        // try to determine used database adapter
        IDbAdapter* adapter = NULL;
        IDbAdapter::TYPE type = IDbAdapter::atUNKNOWN;

        wxSFDiagramManager mgr;
        mgr.AcceptShape(wxT("All"));
        mgr.SetRootItem(new ErdInfo());

        if(mgr.DeserializeFromXml(filename.GetFullPath())) {
            ErdInfo* info = wxDynamicCast(mgr.GetRootItem(), ErdInfo);

            if(info) type = info->GetAdapterType();

            switch(type) {
            case IDbAdapter::atSQLITE:
#ifdef DBL_USE_SQLITE
                adapter = new SQLiteDbAdapter();
#endif
                break;

            case IDbAdapter::atMYSQL:
#ifdef DBL_USE_MYSQL
                adapter = new MySqlDbAdapter();
#endif
                break;

            case IDbAdapter::atPOSTGRES:
//#ifdef DBL_USE_POSTGRES
                adapter = new PostgreSqlDbAdapter();
//#endif
                break;

            default:
                break;
            }

            if(adapter) {
                ErdPanel* panel = new ErdPanel(m_mgr->GetEditorPaneNotebook(), adapter, NULL);
                m_mgr->AddEditorPage(panel, wxString::Format(wxT("ERD [%s]"), filename.GetFullName().c_str()));
                panel->LoadERD(filename.GetFullPath());
                return;
            }
        }
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
        m_mgr->GetWorkspacePaneNotebook()->InsertPage(0, m_dbViewerPanel, _("DbExplorer"), true);
    } else {
        int where = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(_("DbExplorer"));
        if(where != wxNOT_FOUND) {
            m_mgr->GetWorkspacePaneNotebook()->RemovePage(where);
        }
    }
}
