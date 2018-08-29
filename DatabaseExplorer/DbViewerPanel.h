//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : DbViewerPanel.h
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

#ifndef DBEXPLORERPANEL_H
#define DBEXPLORERPANEL_H

#include "../Interfaces/imanager.h"
#include "GUI.h" // Base class: _DbExplorerPanel
#include "clToolBar.h"
#include "theme_handler_helper.h"
// database
#include <wx/dblayer/include/DatabaseLayer.h>

//#ifdef DBL_USE_MYSQL
//#include <wx/dblayer/include/MysqlDatabaseLayer.h>
//#endif

#include <wx/wxsf/wxShapeFramework.h>

// wx classes
#include <wx/aui/aui.h>
#include <wx/hashmap.h>
#include <wx/textdlg.h>
#include <wx/txtstrm.h>
#include <wx/wx.h>

// other
#include "IDbItem.h"

#include "DnDTableShape.h"
#include "IDbAdapter.h"
#include "column.h"
#include "database.h"
#include "dbconnection.h"
#include "dumpclass.h"
#include "table.h"
#include "view.h"
//#include "MySqlDbAdapter.h"
#include "ClassGenerateDialog.h"
#include "LogDialog.h"
#include "dbitem.h"
#include <unordered_set>

class DbExplorerFrame;
WX_DECLARE_HASH_MAP(wxString, wxTreeItemId, wxStringHash, wxStringEqual, TableHashMap);

class MainBook;
/*! \brief Main application panel. It show database tree and can open special editors. */
class DbViewerPanel : public _DbViewerPanel
{
    std::unordered_set<DbExplorerFrame*> m_frames;

protected:
    virtual void OnContextMenu(wxTreeEvent& event);

private:
    xsSerializable* m_pConnections;
    IDbAdapter* m_pDbAdapter;
    wxWindow* m_pGlobalParent;
    wxString m_server;
    wxTreeItemId m_selectedID;
    wxWindow* m_pNotebook;
    IManager* m_mgr;
    TableHashMap m_hashTables;
    Database* m_pEditedDatabase;
    DbConnection* m_pEditedConnection;
    wxWindow* m_pPrevPanel;
    bool m_SuppressUpdate;
    ThemeHandlerHelper* m_themeHelper;
    clToolBar* m_toolbar;

public:
    DbViewerPanel(wxWindow* parent, wxWindow* notebook, IManager* pManager);
    virtual ~DbViewerPanel();
    void SetDbAdapter(IDbAdapter* dbAdapter) { m_pDbAdapter = dbAdapter; }

    void AddDbConnection(DbConnection* pDbCon) { m_pConnections->AddChild(pDbCon); }
    void SetServer(wxString& server) { m_server = server; }
    void OpenSQLiteFile(const wxFileName& fileName, bool openDefaultSQLPanel);
    
    virtual void OnConnectClick(wxCommandEvent& event);
    virtual void OnConnectUI(wxUpdateUIEvent& event);
    virtual void OnItemActivate(wxTreeEvent& event);
    virtual void OnRefreshClick(wxCommandEvent& event);
    virtual void OnRefreshUI(wxUpdateUIEvent& event);
    virtual void OnItemSelectionChange(wxTreeEvent& event);
    virtual void OnDnDStart(wxTreeEvent& event);
    virtual void OnItemRightClick(wxTreeEvent& event);
    virtual void OnToolCloseClick(wxCommandEvent& event);
    virtual void OnToolCloseUI(wxUpdateUIEvent& event);
    virtual void OnShowThumbnail(wxCommandEvent& e);

    void OnPageClosing(wxBookCtrlEvent& event);
    void OnPageChanged(wxBookCtrlEvent& event);

    void OnPopupClick(wxCommandEvent& evt);
    void RefreshDbView();
    void RemoveFrame(DbExplorerFrame* frame);
    static void InitStyledTextCtrl(wxStyledTextCtrl* sci);

protected:
    enum PanelType { Sql };
    bool ImportDb(const wxString& sqlFile, Database* pDb);
    wxString CreatePanelName(Table* t);
    wxString CreatePanelName(View* v);
    wxString CreatePanelName(Database* d);
    bool DoSelectPage(const wxString& page);
    void AddEditorPage(wxWindow* page, const wxString& name);
};

#endif // DBEXPLORERPANEL_H
