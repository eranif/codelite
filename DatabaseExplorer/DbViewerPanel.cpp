//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : DbViewerPanel.cpp
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

#include "AdapterSelectDlg.h"
#include "DbSettingDialog.h"
#include "DbViewerPanel.h"
#include "SqlCommandPanel.h"
#include "globals.h"
#include "window_locker.h"

#include "DbExplorerFrame.h"
#include "SqliteDbAdapter.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "lexer_configuration.h"
#include <algorithm>
#include <wx/imaglist.h>
#include <wx/msgdlg.h>
#include <wx/wfstream.h>
#include <wx/xrc/xmlres.h>

DbViewerPanel::DbViewerPanel(wxWindow* parent, wxWindow* notebook, IManager* pManager)
    : _DbViewerPanel(parent)
{
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);

    m_pNotebook = notebook;
    m_pGlobalParent = parent;
    m_mgr = pManager;
    m_pPrevPanel = NULL;
    m_SuppressUpdate = false;

    MSWSetNativeTheme(m_treeDatabases);

    m_pDbAdapter = NULL;
    m_pConnections = new xsSerializable();
    GetSizer()->Layout();

    // replace the icons...
    BitmapLoader* bmpLoader = pManager->GetStdIcons();
    m_toolbar = new clToolBar(this);
    GetSizer()->Insert(0, m_toolbar, 0, wxEXPAND);

    m_toolbar->AddTool(XRCID("IDT_DBE_CONNECT"), _("Open connection"), bmpLoader->LoadBitmap("plus"));
    m_toolbar->AddTool(XRCID("IDT_DBE_CLOSE_CONNECTION"), _("Close selected connection"),
                       bmpLoader->LoadBitmap("minus"));
    m_toolbar->AddTool(XRCID("IDT_DBE_REFRESH"), _("Refresh View"), bmpLoader->LoadBitmap("debugger_restart"));
    m_toolbar->Realize();
    Layout();

    m_toolbar->Bind(wxEVT_TOOL, &DbViewerPanel::OnConnectClick, this, XRCID("IDT_DBE_CONNECT"));
    m_toolbar->Bind(wxEVT_TOOL, &DbViewerPanel::OnToolCloseClick, this, XRCID("IDT_DBE_CLOSE_CONNECTION"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DbViewerPanel::OnToolCloseUI, this, XRCID("IDT_DBE_CLOSE_CONNECTION"));
    m_toolbar->Bind(wxEVT_TOOL, &DbViewerPanel::OnRefreshClick, this, XRCID("IDT_DBE_REFRESH"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DbViewerPanel::OnRefreshUI, this, XRCID("IDT_DBE_REFRESH"));
    m_themeHelper = new ThemeHandlerHelper(this);
}

DbViewerPanel::~DbViewerPanel()
{
    std::for_each(m_frames.begin(), m_frames.end(), [&](DbExplorerFrame* frame) { wxDELETE(frame); });
    wxDELETE(m_themeHelper);

    m_toolbar->Unbind(wxEVT_TOOL, &DbViewerPanel::OnConnectClick, this, XRCID("IDT_DBE_CONNECT"));
    m_toolbar->Unbind(wxEVT_TOOL, &DbViewerPanel::OnToolCloseClick, this, XRCID("IDT_DBE_CLOSE_CONNECTION"));
    m_toolbar->Unbind(wxEVT_UPDATE_UI, &DbViewerPanel::OnToolCloseUI, this, XRCID("IDT_DBE_CLOSE_CONNECTION"));
    m_toolbar->Unbind(wxEVT_TOOL, &DbViewerPanel::OnRefreshClick, this, XRCID("IDT_DBE_REFRESH"));
    m_toolbar->Unbind(wxEVT_UPDATE_UI, &DbViewerPanel::OnRefreshUI, this, XRCID("IDT_DBE_REFRESH"));
    delete m_pDbAdapter;
}

void DbViewerPanel::OnConnectClick(wxCommandEvent& event)
{
    DbSettingDialog dlg(this, m_mgr->GetTheApp()->GetTopWindow());
    dlg.ShowModal();
    RefreshDbView();
}

void DbViewerPanel::OnConnectUI(wxUpdateUIEvent& event) { event.Enable(!m_pConnections->HasChildren()); }

void DbViewerPanel::OnItemActivate(wxTreeEvent& event)
{
    try {
        DbItem* item = (DbItem*)m_treeDatabases->GetItemData(event.GetItem());
        if(item) {
            wxString pagename;
            if(Table* tab = wxDynamicCast(item->GetData(), Table)) {
                pagename = CreatePanelName(tab);
                SQLCommandPanel* sqlpage = new SQLCommandPanel(m_pNotebook, tab->GetDbAdapter()->Clone(),
                                                               tab->GetParentName(), tab->GetName());
                CallAfter(&DbViewerPanel::AddEditorPage, sqlpage, pagename);
            }

            if(View* pView = wxDynamicCast(item->GetData(), View)) {
                pagename = CreatePanelName(pView);
                SQLCommandPanel* sqlpage = new SQLCommandPanel(m_pNotebook, pView->GetDbAdapter()->Clone(),
                                                               pView->GetParentName(), pView->GetName());
                CallAfter(&DbViewerPanel::AddEditorPage, sqlpage, pagename);
            }

            if(Database* db = wxDynamicCast(item->GetData(), Database)) {
                pagename = CreatePanelName(db);
                SQLCommandPanel* sqlpage =
                    new SQLCommandPanel(m_pNotebook, db->GetDbAdapter()->Clone(), db->GetName(), wxT(""));
                CallAfter(&DbViewerPanel::AddEditorPage, sqlpage, pagename);
            }
        }

    } catch(DatabaseLayerException& e) {
        ::wxMessageBox(wxString() << "Error occurred while opening SQL panel: " << e.GetErrorMessage(),
                       "Database Explorer", wxOK | wxICON_ERROR | wxCENTER);
    }
}

void DbViewerPanel::OnRefreshClick(wxCommandEvent& event)
{
    // Refresh the view
    try {
        RefreshDbView();

    } catch(DatabaseLayerException& e) {
        ::wxMessageBox(e.GetErrorMessage(), "Database Explorer", wxOK | wxCENTER | wxICON_ERROR);
    }
}

void DbViewerPanel::OnRefreshUI(wxUpdateUIEvent& event) { event.Enable(m_pConnections->HasChildren()); }

void DbViewerPanel::RefreshDbView()
{
    // Refresh all connections
    wxTreeItemId root = m_treeDatabases->GetRootItem();
    if(root.IsOk()) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_treeDatabases->GetFirstChild(root, cookie);
        while(child.IsOk()) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(child);
            if(data) {
                DbConnection* pCon = wxDynamicCast(data->GetData(), DbConnection);
                if(pCon) pCon->RefreshChildren();
                Database* pDb = wxDynamicCast(data->GetData(), Database);
                if(pDb) pDb->RefreshChildren(false);
                Table* pTab = wxDynamicCast(data->GetData(), Table);
                if(pTab) pTab->RefreshChildren();
            }
            child = m_treeDatabases->GetNextChild(root, cookie);
        }
    }
    // clear items from tree
    m_treeDatabases->DeleteAllItems();
    // create imageList for icons
    BitmapLoader* loader = clGetManager()->GetStdIcons();
    m_treeDatabases->SetBitmaps(loader->GetStandardMimeBitmapListPtr());

    int img_folder = loader->GetImageIndex(FileExtManager::TypeFolder);
    int img_table = loader->GetImageIndex(BitmapLoader::kTable);
    int img_find = loader->GetImageIndex(BitmapLoader::kFind);
    int img_database = loader->GetImageIndex(BitmapLoader::kDatabase);
    int img_column = loader->GetImageIndex(BitmapLoader::kColumn);
    wxTreeItemId totalRootID = m_treeDatabases->AddRoot(wxString::Format(wxT("Databases")), wxNOT_FOUND);

    // ---------------- load connections ----------------------------
    SerializableList::compatibility_iterator connectionNode = m_pConnections->GetFirstChildNode();
    while(connectionNode) {
        DbConnection* pDbCon = (DbConnection*)wxDynamicCast(connectionNode->GetData(), DbConnection);
        if(pDbCon) {
            wxTreeItemId rootID = m_treeDatabases->AppendItem(
                totalRootID, wxString::Format(wxT("Databases (%s)"), pDbCon->GetServerName().c_str()), img_database,
                img_database, new DbItem(pDbCon));

            // ----------------------- load databases -------------------------------
            SerializableList::compatibility_iterator dbNode = pDbCon->GetFirstChildNode();
            while(dbNode) {
                Database* pDatabase = wxDynamicCast(dbNode->GetData(), Database);
                if(pDatabase) {
                    wxTreeItemId dbID = m_treeDatabases->AppendItem(rootID, pDatabase->GetName(), img_database,
                                                                    img_database, new DbItem(pDatabase));
                    m_treeDatabases->Expand(rootID);
                    wxTreeItemId idFolder =
                        m_treeDatabases->AppendItem(dbID, wxT("Tables"), img_folder, img_folder, NULL);

                    // ----------------------------- load tables ----------------------------------
                    SerializableList::compatibility_iterator tabNode = pDatabase->GetFirstChildNode();
                    while(tabNode) {
                        Table* pTable = wxDynamicCast(tabNode->GetData(), Table);
                        if(pTable) {
                            wxTreeItemId tabID =
                                m_treeDatabases->AppendItem(idFolder, pTable->GetName(), img_table, img_table,
                                                            new DbItem(pTable)); // NULL);

                            ///////////////////////////////////////////////////////////
                            // Add the columns
                            ///////////////////////////////////////////////////////////
                            SerializableList::compatibility_iterator columnNode = pTable->GetFirstChildNode();
                            while(columnNode) {
                                Column* col = wxDynamicCast(columnNode->GetData(), Column);
                                if(col) {
                                    m_treeDatabases->AppendItem(tabID, col->FormatName().c_str(), img_column,
                                                                img_column, new DbItem(col));
                                }
                                columnNode = columnNode->GetNext();
                            }
                        }
                        tabNode = tabNode->GetNext();
                    }
                    // ----------------------------------------------------------------------------

                    idFolder = m_treeDatabases->AppendItem(dbID, wxT("Views"), img_folder, img_folder, NULL);

                    // ----------------------------- load views ----------------------------------
                    tabNode = pDatabase->GetFirstChildNode();
                    while(tabNode) {
                        View* pView = wxDynamicCast(tabNode->GetData(), View);
                        if(pView) {
                            m_treeDatabases->AppendItem(idFolder, pView->GetName(), img_find, img_find,
                                                        new DbItem(pView)); // NULL);
                        }
                        tabNode = tabNode->GetNext();
                    }
                    // ----------------------------------------------------------------------------
                }
                dbNode = dbNode->GetNext();
            }
            // -----------------------------------------------------------------------
        }
        connectionNode = connectionNode->GetNext();
    }
}

void DbViewerPanel::OnItemSelectionChange(wxTreeEvent& event) { wxUnusedVar(event); }

void DbViewerPanel::OnDnDStart(wxTreeEvent& event) { event.Skip(); }

void DbViewerPanel::OnItemRightClick(wxTreeEvent& event)
{
    event.Skip();
    if(event.GetItem().IsOk()) { m_treeDatabases->SelectItem(event.GetItem()); }
}

void DbViewerPanel::OnToolCloseClick(wxCommandEvent& event)
{
    // Close the active connection (there is only one)
    // getting selected item data
    /*wxTreeItemIdValue cookie;
    DbItem* data = (DbItem*) m_treeDatabases->GetItemData(m_treeDatabases->GetFirstChild(m_treeDatabases->GetRootItem(),
    cookie));*/

    wxTreeItemId itemId = m_treeDatabases->GetSelection();

    DbItem* data = (DbItem*)m_treeDatabases->GetItemData(itemId);
    if(data) {
        DbConnection* pCon = wxDynamicCast(data->GetData(), DbConnection);
        if(pCon) {
            wxMessageDialog dlg(this, _("Close connection?"), _("Close"), wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT);
            if(dlg.ShowModal() == wxID_YES) {
                // m_pConnections->GetChildrenList().DeleteContents(true);
                m_pConnections->GetChildrenList().DeleteObject(pCon);
                m_treeDatabases->Delete(itemId);

                RefreshDbView();
            }
        }
        /* else
                wxMessageBox( _("Please, select database connection in DbExplorer tree view."), _("Close"), wxOK |
           wxICON_WARNING ); */
    }
}

void DbViewerPanel::OnToolCloseUI(wxUpdateUIEvent& event)
{
    wxTreeItemId treeId = m_treeDatabases->GetSelection();
    if(treeId.IsOk()) {
        DbItem* data = (DbItem*)m_treeDatabases->GetItemData(treeId);
        event.Enable(data && wxDynamicCast(data->GetData(), DbConnection));
    } else {
        event.Enable(false);
    }
}

void DbViewerPanel::OnPopupClick(wxCommandEvent& evt)
{
    if(!m_selectedID.IsOk()) return;

    try {
        if(evt.GetId() == XRCID("IDR_DBVIEWER_COPY_COLUMN")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Column* col = wxDynamicCast(data->GetData(), Column);
                if(col) {
                    wxString colname = col->GetName();
                    ::CopyToClipboard(colname);
                }
            }
        } else if(evt.GetId() == XRCID("IDT_DBE_CLOSE_CONNECTION")) {
            OnToolCloseClick(evt);
            return;

        } else if(evt.GetId() == XRCID("IDT_DBE_REFRESH")) {
            OnRefreshClick(evt);
            return;

        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_ADD_DATABASE")) {
            if(m_pEditedConnection) {
                // TODO:LANG:
                wxString dbName = wxGetTextFromUser(_("Database name"), _("Add database"));
                if(!dbName.IsEmpty()) {
                    DatabaseLayerPtr pDbLayer = m_pEditedConnection->GetDbAdapter()->GetDatabaseLayer(wxT(""));
                    wxString sql = m_pEditedConnection->GetDbAdapter()->GetCreateDatabaseSql(dbName);
                    if(!sql.empty()) {

                        pDbLayer->RunQuery(sql);
                        pDbLayer->Close();

                        // TODO:LANG:
                        wxMessageBox(_("Database created successfully"));

                        RefreshDbView();
                    } else {
                        // TODO:LANG:
                        wxMessageDialog dlg(this, _("Can't create new db in this database engine!"), _("Error"),
                                            wxOK | wxICON_ERROR);
                        dlg.ShowModal();
                    }
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_DROP_DATABASE")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Database* pDb = (Database*)wxDynamicCast(data->GetData(), Database);
                if(pDb) {
                    wxString dropSQL = pDb->GetDbAdapter()->GetDropDatabaseSql(pDb);
                    if(!dropSQL.IsEmpty()) {
                        // TODO:LANG:
                        wxMessageDialog dlg(this, wxString::Format(_("Remove database '%s'?"), pDb->GetName().c_str()),
                                            _("Drop database"), wxYES_NO);
                        if(dlg.ShowModal() == wxID_YES) {
                            DatabaseLayerPtr pDbLayer = pDb->GetDbAdapter()->GetDatabaseLayer(wxT(""));
                            pDbLayer->RunQuery(dropSQL);
                            pDbLayer->Close();
                            // TODO:LANG:
                            wxMessageBox(_("Database dropped successfully"));
                            RefreshDbView();
                        }
                    }
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_CLASS_DB")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Database* pDb = (Database*)wxDynamicCast(data->GetData(), Database);
                if(pDb) {
                    pDb = (Database*)pDb->Clone();
                    // NOTE: the refresh functions must be here for propper code generation (they translate views into
                    // tables)
                    pDb->RefreshChildren(true);
                    pDb->RefreshChildrenDetails();
                    ClassGenerateDialog dlg(m_mgr->GetTheApp()->GetTopWindow(), pDb->GetDbAdapter(), pDb, m_mgr);
                    dlg.ShowModal();
                    delete pDb;
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_CLASS_TABLE")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Table* pTab = (Table*)wxDynamicCast(data->GetData(), Table);
                if(pTab) {
                    ClassGenerateDialog dlg(m_mgr->GetTheApp()->GetTopWindow(), pTab->GetDbAdapter(),
                                            (Table*)pTab->Clone(), m_mgr);
                    dlg.ShowModal();
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_DROP_TABLE")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Table* pTab = (Table*)wxDynamicCast(data->GetData(), Table);
                if(pTab) {
                    wxMessageDialog dlg(this, wxString::Format(_("Remove table '%s'?"), pTab->GetName().c_str()),
                                        _("Drop table"), wxYES_NO);
                    if(dlg.ShowModal() == wxID_YES) {
                        DatabaseLayerPtr pDbLayer = pTab->GetDbAdapter()->GetDatabaseLayer(pTab->GetParentName());
                        pDbLayer->RunQuery(pTab->GetDbAdapter()->GetDropTableSql(pTab));
                        pDbLayer->Close();

                        wxMessageBox(_("Table dropped successfully"));
                        RefreshDbView();
                    }
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_DROP_VIEW")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                View* pView = (View*)wxDynamicCast(data->GetData(), View);
                if(pView) {
                    wxMessageDialog dlg(this, wxString::Format(_("Remove view '%s'?"), pView->GetName().c_str()),
                                        _("Drop view"), wxYES_NO);
                    if(dlg.ShowModal() == wxID_YES) {
                        DatabaseLayerPtr pDbLayer = pView->GetDbAdapter()->GetDatabaseLayer(pView->GetParentName());
                        pDbLayer->RunQuery(pView->GetDbAdapter()->GetDropViewSql(pView));
                        pDbLayer->Close();

                        wxMessageBox(_("View dropped successfully"));
                        RefreshDbView();
                    }
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_SQL_TABLE")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Table* pTab = (Table*)wxDynamicCast(data->GetData(), Table);
                if(pTab) {
#ifdef __WXMSW__
                    clWindowUpdateLocker locker(m_mgr->GetEditorPaneNotebook());
#endif
                    wxString pagename = CreatePanelName(pTab);
                    if(!DoSelectPage(pagename)) {
                        SQLCommandPanel* sqlpage = new SQLCommandPanel(m_pNotebook, pTab->GetDbAdapter()->Clone(),
                                                                       pTab->GetParentName(), pTab->GetName());
                        AddEditorPage(sqlpage, pagename);
                    }
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_SQL_VIEW")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                View* pView = (View*)wxDynamicCast(data->GetData(), View);
                if(pView) {
#ifdef __WXMSW__
                    clWindowUpdateLocker locker(m_mgr->GetEditorPaneNotebook());
#endif
                    wxString pagename = CreatePanelName(pView);
                    if(!DoSelectPage(pagename)) {
                        SQLCommandPanel* sqlpage = new SQLCommandPanel(m_pNotebook, pView->GetDbAdapter()->Clone(),
                                                                       pView->GetParentName(), pView->GetName());
                        AddEditorPage(sqlpage, pagename);
                    }
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_SQL_DATABASE")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Database* pDb = (Database*)wxDynamicCast(data->GetData(), Database);
                if(pDb) {
                    wxString pagename = CreatePanelName(pDb);
                    if(!DoSelectPage(pagename)) {
                        SQLCommandPanel* sqlpage =
                            new SQLCommandPanel(m_pNotebook, pDb->GetDbAdapter()->Clone(), pDb->GetName(), wxT(""));
#ifndef __WXMSW__
                        sqlpage->Show();
#endif
                        AddEditorPage(sqlpage, pagename);
                    }
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_IMPORT_DATABASE")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Database* pDb = (Database*)wxDynamicCast(data->GetData(), Database);
                if(pDb) {
                    // TODO:LANG:
                    wxFileDialog dlg(this, _("Import database from SQL file ..."), wxGetCwd(), wxT(""),
                                     wxT("SQL Files (*.sql)|*.sql"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                    if(dlg.ShowModal() == wxID_OK) { ImportDb(dlg.GetPath(), pDb); }
                }
            }
            RefreshDbView();
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_DUMP_DATABASE")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Database* pDb = (Database*)wxDynamicCast(data->GetData(), Database);
                if(pDb) {
                    // TODO:LANG:
                    wxFileDialog dlg(this, _("Dump data into file ..."), wxT(""), pDb->GetName() + wxT(".sql"),
                                     wxT("SQL files (*.sql)|*.sql"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
                    if(dlg.ShowModal() == wxID_OK) {
                        DumpClass* dump = new DumpClass(pDb->GetDbAdapter(), pDb, dlg.GetPath());
                        dump->DumpData();
                        wxMessageBox(_("Data was saved to ") + dlg.GetPath());
                    }
                }
            }
        } else if(evt.GetId() == XRCID("IDR_DBVIEWER_EXPORT_DATABASE")) {
            DbItem* data = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
            if(data) {
                Database* pDb = (Database*)wxDynamicCast(data->GetData(), Database);
                if(pDb) {
                    wxFileDialog dlg(this, _("Export database..."), wxGetCwd(), wxT(""), wxT("SQL Files (*.sql)|*.sql"),
                                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
                    if(dlg.ShowModal() == wxID_OK) {
                        // CreateStructure
                        wxString retStr = wxT("-- SQL script created by wxDbExplorer\n\n ");
                        SerializableList::compatibility_iterator tabNode = pDb->GetFirstChildNode();
                        while(tabNode) {
                            Table* tab = wxDynamicCast(tabNode->GetData(), Table);
                            if(tab) { retStr.append(pDb->GetDbAdapter()->GetCreateTableSql(tab, true)); }
                            tabNode = tabNode->GetNext();
                        }

                        tabNode = pDb->GetFirstChildNode();
                        while(tabNode) {
                            View* view = wxDynamicCast(tabNode->GetData(), View);
                            if(view) { retStr.append(pDb->GetDbAdapter()->GetCreateViewSql(view, true)); }
                            tabNode = tabNode->GetNext();
                        }

                        tabNode = pDb->GetFirstChildNode();
                        while(tabNode) {
                            Table* tab = wxDynamicCast(tabNode->GetData(), Table);
                            if(tab) { retStr.append(pDb->GetDbAdapter()->GetAlterTableConstraintSql(tab)); }
                            tabNode = tabNode->GetNext();
                        }

                        DumpClass dump(pDb->GetDbAdapter(), pDb, dlg.GetPath());
                        dump.DumpData();

                        wxTextFile file(dlg.GetPath());
                        if(!file.Exists()) file.Create();
                        file.Open();
                        if(file.IsOpened()) {
                            file.InsertLine(retStr, 0);
                            file.Write(wxTextFileType_None, wxConvUTF8);
                            file.Close();
                        }

                        wxMessageBox(
                            wxString::Format(_("The database has been exported to '%s'."), dlg.GetPath().GetData()),
                            _("wxDbExplorer"));
                    }
                }
            }
        } else {
            evt.Skip();
        }
    } catch(DatabaseLayerException& e) {
        wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
        wxMessageDialog dlg(this, errorMessage, _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    } catch(...) {
        wxMessageDialog dlg(this, _("Unknown error."), _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    }
}

bool DbViewerPanel::ImportDb(const wxString& sqlFile, Database* pDb)
{
    DatabaseLayerPtr pDbLayer(NULL);
    LogDialog dialog(this); // TODO:Doresit parenta
    dialog.Show();

    try {
        wxFileInputStream input(sqlFile);
        wxTextInputStream text(input);
        text.SetStringSeparators(wxT(";"));
        wxString command = wxT("");

        pDbLayer = pDb->GetDbAdapter()->GetDatabaseLayer(pDb->GetName());
        pDbLayer->BeginTransaction();

        wxString useSql = pDb->GetDbAdapter()->GetUseDb(pDb->GetName());
        if(!useSql.IsEmpty()) pDbLayer->RunQuery(wxString::Format(wxT("USE %s"), pDb->GetName().c_str()));

        while(!input.Eof()) {
            wxString line = text.ReadLine();
            // dialog.AppendText(line);
            int index = line.Find(wxT("--"));
            if(index != wxNOT_FOUND) line = line.Mid(0, index);
            command.append(line);
            if(line.Find(wxT(";")) != wxNOT_FOUND) {
                dialog.AppendSeparator();
                dialog.AppendComment(wxT("Run SQL command:"));
                dialog.AppendText(command);
                pDbLayer->RunQuery(command);
                dialog.AppendComment(_("Successful!"));
                command.clear();
            }
        }
        pDbLayer->Commit();
        pDbLayer->Close();
    } catch(DatabaseLayerException& e) {
        if(pDbLayer) {
            pDbLayer->RollBack();
            pDbLayer->Close();
        }
        wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());

        dialog.AppendComment(_("Fail!"));
        dialog.AppendComment(errorMessage);
        wxMessageDialog dlg(this, errorMessage, _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    } catch(...) {
        if(pDbLayer) {
            pDbLayer->RollBack();
            pDbLayer->Close();
        }
        wxMessageDialog dlg(this, _("Unknown error."), _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    }
    dialog.EnableClose(true);
    dialog.ShowModal();

    return false;
}

void DbViewerPanel::OnPageChanged(wxBookCtrlEvent& event) { event.Skip(); }

void DbViewerPanel::OnPageClosing(wxBookCtrlEvent& event) { event.Skip(); }

wxString DbViewerPanel::CreatePanelName(Database* d) { return wxT("SQL [") + d->GetName() + wxT("]"); }

wxString DbViewerPanel::CreatePanelName(Table* t)
{
    return wxT("SQL [") + t->GetParentName() + wxT(":") + t->GetName() + wxT("]");
}

wxString DbViewerPanel::CreatePanelName(View* v)
{
    return wxT("SQL [") + v->GetParentName() + wxT(":") + v->GetName() + wxT("]");
}

void DbViewerPanel::InitStyledTextCtrl(wxStyledTextCtrl* sci)
{
    LexerConf::Ptr_t lexer = EditorConfigST::Get()->GetLexer("SQL");
    if(lexer) { lexer->Apply(sci, true); }
}

void DbViewerPanel::OnShowThumbnail(wxCommandEvent& e) { wxUnusedVar(e); }

bool DbViewerPanel::DoSelectPage(const wxString& page)
{
    wxWindow* win = m_mgr->FindPage(page);
    if(win) {
        m_mgr->SelectPage(win);
        win->SetFocus();

        // replace the query and execute it
        SQLCommandPanel* sqlPage = dynamic_cast<SQLCommandPanel*>(win);
        if(sqlPage) {
            sqlPage->SetDefaultSelect();
            sqlPage->ExecuteSql();
        }
        return true;
    }
    return false;
}

void DbViewerPanel::AddEditorPage(wxWindow* page, const wxString& name)
{
    m_SuppressUpdate = true;
    DbExplorerFrame* frame = new DbExplorerFrame(EventNotifier::Get()->TopFrame(), page, name, this);
    frame->Show();
    m_frames.insert(frame);
}

void DbViewerPanel::OnContextMenu(wxTreeEvent& event)
{
    m_pEditedDatabase = NULL;
    m_pEditedConnection = NULL;
    m_selectedID = event.GetItem();

    DbItem* item = (DbItem*)m_treeDatabases->GetItemData(m_selectedID);
    wxMenu menu;
    int c = 0;
    if(item) {
        DbConnection* conn = wxDynamicCast(item->GetData(), DbConnection);
        if(conn) {
            // Connection node
            menu.Append(XRCID("IDT_DBE_CLOSE_CONNECTION"), _("Close Connection"), _("Close Connection"));
            menu.AppendSeparator();
            menu.Append(XRCID("IDT_DBE_REFRESH"), _("Refresh Connection"), _("Refresh Connection"));
            c++;
        }
        Database* db = wxDynamicCast(item->GetData(), Database);
        if(db) {
            // menu.Append(XRCID("IDR_DBVIEWER_ADD_TABLE"),_("Add table"),_("Run SQL command for creating Table"));
            menu.Append(XRCID("IDR_DBVIEWER_SQL_DATABASE"), _("Open SQL panel"),
                        _("Open SQL command panel for the database"));
            menu.AppendSeparator();

            menu.Append(XRCID("IDR_DBVIEWER_DROP_DATABASE"), _("Drop database"),
                        _("Run SQL command for deleting Database"));

            menu.AppendSeparator();
            menu.Append(XRCID("IDR_DBVIEWER_CLASS_DB"), _("Create classes from DB"),
                        _("Create C++ classes for the database"));

            menu.AppendSeparator();
            menu.Append(XRCID("IDR_DBVIEWER_IMPORT_DATABASE"), _("Import database from file"),
                        _("Run SQL commands stored in *.sql file"));
            menu.Append(XRCID("IDR_DBVIEWER_EXPORT_DATABASE"), _("Export database to file"),
                        _("Export database CREATE SQL statements into *.sql file"));

            menu.AppendSeparator();
            menu.Append(XRCID("IDR_DBVIEWER_DUMP_DATABASE"), _("Dump data to file"),
                        _("Dump data from database into .sql file"));
            c++;

            m_pEditedDatabase = db;
        }
        Table* tab = wxDynamicCast(item->GetData(), Table);
        if(tab) {
            menu.Append(XRCID("IDR_DBVIEWER_SQL_TABLE"), _("Open SQL panel"),
                        _("Open SQL command panel for the table"));
            menu.AppendSeparator();

            menu.Append(XRCID("IDR_DBVIEWER_DROP_TABLE"), _("Drop table"), _("Run SQL command to delete the table"));
            menu.AppendSeparator();
            menu.Append(XRCID("IDR_DBVIEWER_CLASS_TABLE"), _("Create classes from Table"),
                        _("Create C++ classes for the table"));
            c++;
        }
        Column* col = wxDynamicCast(item->GetData(), Column);
        if(col) {
            menu.Append(XRCID("IDR_DBVIEWER_COPY_COLUMN"), _("Copy Column Name"), _("Copy Column Name"));
            c++;
        }

        View* vw = wxDynamicCast(item->GetData(), View);
        if(vw) {
            menu.Append(XRCID("IDR_DBVIEWER_SQL_VIEW"), _("Open SQL panel"), _("Open SQL command panel for the view"));
            menu.AppendSeparator();

            menu.Append(XRCID("IDR_DBVIEWER_DROP_VIEW"), _("Drop view"), _("Run SQL command to delete the view"));
            c++;
        }
    }

    if(c > 0) {
        menu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&DbViewerPanel::OnPopupClick, NULL, this);
        PopupMenu(&menu);
    }
}

void DbViewerPanel::RemoveFrame(DbExplorerFrame* frame)
{
    if(m_frames.count(frame)) { m_frames.erase(frame); }
}

void DbViewerPanel::OpenSQLiteFile(const wxFileName& fileName, bool openDefaultSQLPanel)
{
    try {
        IDbAdapter* pAdapt = new SQLiteDbAdapter(fileName.GetFullPath());
        wxString serverName = fileName.GetFullPath();
        DbConnection* connection = new DbConnection(pAdapt, serverName);
        AddDbConnection(connection);
        SetServer(serverName);
        RefreshDbView();
        if(openDefaultSQLPanel) {
            // Open an SQL panel view
            SQLCommandPanel* sqlpage = new SQLCommandPanel(m_pNotebook, pAdapt->Clone(), fileName.GetFullPath(), "");
            CallAfter(&DbViewerPanel::AddEditorPage, sqlpage, fileName.GetFullPath());
        }

    } catch(DatabaseLayerException& e) {
        wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
        ::wxMessageBox(errorMessage, "CodeLite", wxICON_ERROR | wxCENTER | wxOK, EventNotifier::Get()->TopFrame());
    } catch(...) {
        ::wxMessageBox(_("Unknown error."), "CodeLite", wxICON_ERROR | wxCENTER | wxOK,
                       EventNotifier::Get()->TopFrame());
    }
}
