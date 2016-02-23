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
#include "theme_handler_helper.h"
// database
#include <wx/dblayer/include/DatabaseLayer.h>

//#ifdef DBL_USE_MYSQL
//#include <wx/dblayer/include/MysqlDatabaseLayer.h>
//#endif

#include <wx/wxsf/wxShapeFramework.h>

// wx classes
#include <wx/wx.h>
#include <wx/hashmap.h>
#include <wx/aui/aui.h>
#include <wx/textdlg.h>
#include <wx/txtstrm.h>

// other
#include "IDbItem.h"

#include "DnDTableShape.h"
#include "dbconnection.h"
#include "database.h"
#include "table.h"
#include "column.h"
#include "view.h"
#include "dumpclass.h"
#include "IDbAdapter.h"
//#include "MySqlDbAdapter.h"
#include "dbitem.h"
#include "LogDialog.h"
#include "ClassGenerateDialog.h"

WX_DECLARE_HASH_MAP(wxString, wxTreeItemId, wxStringHash, wxStringEqual, TableHashMap);

class MainBook;
/*! \brief Main application panel. It show database tree and can open special editors. */
class DbViewerPanel : public _DbViewerPanel
{
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
    wxSFThumbnail* m_pThumbnail;
    wxArrayString m_pagesAdded;
    wxWindow* m_pPrevPanel;
    bool m_SuppressUpdate;
    ThemeHandlerHelper* m_themeHelper;

public:
    DbViewerPanel(wxWindow* parent, wxWindow* notebook, IManager* pManager);
    virtual ~DbViewerPanel();
    void SetDbAdapter(IDbAdapter* dbAdapter) { m_pDbAdapter = dbAdapter; }

    void AddDbConnection(DbConnection* pDbCon) { m_pConnections->AddChild(pDbCon); }

    void SetServer(wxString& server) { m_server = server; }

    void SetThumbnail(wxSFShapeCanvas* canvas) { m_pThumbnail->SetCanvas(canvas); }

    virtual void OnConnectClick(wxCommandEvent& event);
    virtual void OnConnectUI(wxUpdateUIEvent& event);
    virtual void OnItemActivate(wxTreeEvent& event);
    virtual void OnRefreshClick(wxCommandEvent& event);
    virtual void OnRefreshUI(wxUpdateUIEvent& event);
    virtual void OnItemSelectionChange(wxTreeEvent& event);
    virtual void OnERDClick(wxCommandEvent& event);
    virtual void OnDnDStart(wxTreeEvent& event);
    virtual void OnItemRightClick(wxTreeEvent& event);
    virtual void OnToolCloseClick(wxCommandEvent& event);
    virtual void OnToolCloseUI(wxUpdateUIEvent& event);
    virtual void OnShowThumbnail(wxCommandEvent& e);

    void OnPageClosing(wxBookCtrlEvent& event);
    void OnPageChanged(wxBookCtrlEvent& event);

    void OnERDSelected(wxAuiToolBarEvent& event);

    void OnPopupClick(wxCommandEvent& evt);
    void RefreshDbView();
    static void InitStyledTextCtrl(wxStyledTextCtrl* sci);

protected:
    enum PanelType { Sql, Erd };
    bool ImportDb(const wxString& sqlFile, Database* pDb);
    wxString CreatePanelName(Table* t, PanelType type);
    wxString CreatePanelName(View* v, PanelType type);
    wxString CreatePanelName(Database* d, PanelType type);
    bool DoSelectPage(const wxString& page);
    void AddEditorPage(wxWindow* page, const wxString& name);
};

#endif // DBEXPLORERPANEL_H
