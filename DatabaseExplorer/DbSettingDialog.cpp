//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : DbSettingDialog.cpp
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

#include "DbSettingDialog.h"
#include "db_explorer_settings.h"
#include "editor_config.h"
#include "windowattrmanager.h"
#include "globals.h"

#include <wx/dblayer/include/DatabaseLayer.h>

#ifdef DBL_USE_MYSQL
#include <wx/dblayer/include/MysqlDatabaseLayer.h>
#endif

#ifdef DBL_USE_SQLITE
#include <wx/dblayer/include/SqliteDatabaseLayer.h>
#endif

#include <wx/dblayer/include/DatabaseErrorCodes.h>
#include <wx/dblayer/include/DatabaseLayerException.h>

DbSettingDialog::DbSettingDialog(DbViewerPanel* parent, wxWindow* pWindowParent)
    : _DBSettingsDialog(pWindowParent)
{

    m_pParent = parent;
    m_listCtrlRecentFiles->InsertColumn(0, _("File name"));
    m_listCtrlRecentFiles->SetColumnWidth(0, 600);
    m_filePickerSqlite->SetFocus();

    LoadHistory();
#ifndef DBL_USE_MYSQL
    m_MySqlPanel->Enable(false);
#endif
#ifndef DBL_USE_POSTGRES
    m_PostgrePanel->Enable(false);
#endif
    SetName("DbSettingDialog");
    WindowAttrManager::Load(this);
    CentreOnParent();
}

DbSettingDialog::~DbSettingDialog() {}

void DbSettingDialog::OnCancelClick(wxCommandEvent& event) { event.Skip(); }

void DbSettingDialog::OnMySqlOkClick(wxCommandEvent& event)
{
#ifdef DBL_USE_MYSQL
    try {
        // MysqlDatabaseLayer *DbLayer = new
        // MysqlDatabaseLayer(m_txServer->GetValue(),wxT(""),m_txUserName->GetValue(),m_txPassword->GetValue());
        IDbAdapter* adapt =
            new MySqlDbAdapter(m_txServer->GetValue(), m_txUserName->GetValue(), m_txPassword->GetValue());

        wxString serverName = m_txServer->GetValue();
        m_pParent->AddDbConnection(new DbConnection(adapt, serverName));

        m_pParent->SetServer(serverName);
    } catch(DatabaseLayerException& e) {
        wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
        wxMessageDialog dlg(this, errorMessage, _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    } catch(...) {
        wxMessageDialog dlg(this, _("Unknown error."), _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    }
#else
    wxMessageBox(_("MySQL connection is not supported."), _("DB Error"), wxOK | wxICON_WARNING);
#endif
}

void DbSettingDialog::OnSqliteOkClick(wxCommandEvent& event)
{
#ifdef DBL_USE_SQLITE
    try {

        // SqliteDatabaseLayer *DbLayer = new SqliteDatabaseLayer(m_filePickerSqlite->GetPath());
        IDbAdapter* pAdapt = new SQLiteDbAdapter(m_filePickerSqlite->GetPath());

        // m_pParent->SetDbLayer(DbLayer);
        wxString serverName = m_filePickerSqlite->GetPath();
        m_pParent->AddDbConnection(new DbConnection(pAdapt, serverName));

        m_pParent->SetServer(serverName);

    } catch(DatabaseLayerException& e) {
        wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
        wxMessageDialog dlg(this, errorMessage, _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    } catch(...) {
        wxMessageDialog dlg(this, _("Unknown error."), _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    }
#else
    wxMessageBox(_("SQLite connection is not supported."), _("DB Error"), wxOK | wxICON_WARNING);
#endif
}

void DbSettingDialog::OnHistoryClick(wxCommandEvent& event)
{
    wxUnusedVar(event);
#ifdef DBL_USE_MYSQL
    DoFindConnectionByName(DoLoadMySQLHistory(), m_listBox2->GetStringSelection());
#endif
}

void DbSettingDialog::OnHistoryDClick(wxCommandEvent& event) { event.Skip(); }

void DbSettingDialog::LoadHistory()
{
    // recent sqlite files
    wxArrayString files = DoLoadSqliteHistory();

    m_listCtrlRecentFiles->DeleteAllItems();
    for(size_t i = 0; i < files.Count(); i++) {
        int idx = AppendListCtrlRow(m_listCtrlRecentFiles);
        SetColumnText(m_listCtrlRecentFiles, idx, 0, files.Item(i));
    }
#ifdef DBL_USE_MYSQL
    DbConnectionInfoVec mySqlConns = DoLoadMySQLHistory();
    m_listBox2->Clear();
    for(size_t i = 0; i < mySqlConns.size(); i++) {
        m_listBox2->Append(mySqlConns.at(i).GetConnectionName());
    }
#endif

#ifdef DBL_USE_POSTGRES
    DbConnectionInfoVec pgSqlConns = DoLoadPgSQLHistory();
    m_listBoxPg->Clear();
    for(size_t i = 0; i < pgSqlConns.size(); i++) {
        m_listBoxPg->Append(pgSqlConns.at(i).GetConnectionName());
    }
#endif
}

void DbSettingDialog::OnPgOkClick(wxCommandEvent& event)
{
#ifdef DBL_USE_POSTGRES
    try {
        // MysqlDatabaseLayer *DbLayer = new
        // MysqlDatabaseLayer(m_txServer->GetValue(),wxT(""),m_txUserName->GetValue(),m_txPassword->GetValue());
        long portNumber = 0;
        m_txPgPort->GetValue().ToLong(&portNumber);
        IDbAdapter* adapt = new PostgreSqlDbAdapter(m_txPgServer->GetValue(),
                                                    portNumber,
                                                    m_txPgDatabase->GetValue(),
                                                    m_txPgUserName->GetValue(),
                                                    m_txPgPassword->GetValue());

        wxString serverName = m_txPgServer->GetValue();
        m_pParent->AddDbConnection(new DbConnection(adapt, serverName));

        m_pParent->SetServer(serverName);
    } catch(DatabaseLayerException& e) {
        wxString errorMessage = wxString::Format(_("Error (%d): %s"), e.GetErrorCode(), e.GetErrorMessage().c_str());
        wxMessageDialog dlg(this, errorMessage, _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    } catch(...) {
        wxMessageDialog dlg(this, _("Unknown error."), _("DB Error"), wxOK | wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    }
#else
    wxMessageBox(_("PostgreSQL connection is not supported."), _("DB Error"), wxOK | wxICON_WARNING);
#endif
}

void DbSettingDialog::OnPgHistoryClick(wxCommandEvent& event)
{
    wxUnusedVar(event);
#ifdef DBL_USE_POSTGRES
    DoFindConnectionByName(DoLoadPgSQLHistory(), m_listBoxPg->GetStringSelection());
#endif
}

void DbSettingDialog::OnPgHistoryDClick(wxCommandEvent& event) { event.Skip(); }

void DbSettingDialog::OnItemActivated(wxListEvent& event)
{
    wxCommandEvent dummy;

    long selecteditem = -1;
    selecteditem = m_listCtrlRecentFiles->GetNextItem(selecteditem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    m_filePickerSqlite->SetPath(GetColumnText(m_listCtrlRecentFiles, (int)selecteditem, 0));
    OnSqliteOkClick(dummy);
    Close();
}

void DbSettingDialog::OnItemKeyDown(wxListEvent& event)
{
    if(event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {
        m_listCtrlRecentFiles->DeleteItem(event.GetItem());
        DoSaveSqliteHistory();

    } else {
        event.Skip();
    }
}

void DbSettingDialog::OnItemSelected(wxListEvent& event)
{
    long selecteditem = -1;
    selecteditem = m_listCtrlRecentFiles->GetNextItem(selecteditem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    m_filePickerSqlite->SetPath(GetColumnText(m_listCtrlRecentFiles, (int)selecteditem, 0));
}

void DbSettingDialog::OnDlgOK(wxCommandEvent& event)
{
    // Dispatch the OK click
    int selection = m_notebook2->GetSelection();
    switch(selection) {
    case 0: // Sqlite
        OnSqliteOkClick(event);
        break;
    case 1: // MySQL
        OnMySqlOkClick(event);
        break;
    case 2: // Pg
        OnPgOkClick(event);
        break;
    default:
        break;
    }

    // Save all the connections
    DoSaveSqliteHistory();

#ifdef DBL_USE_MYSQL
    DoSaveMySQLHistory();
#endif

#ifdef DBL_USE_POSTGRES
    DoSavePgSQLHistory();
#endif
    event.Skip();
}

void DbSettingDialog::DoSaveSqliteHistory()
{
    // Save the recent opened files
    clConfig config(DBE_CONFIG_FILE);
    DbExplorerSettings settings;
    config.ReadItem(&settings);

    wxArrayString files = settings.GetRecentFiles();

    wxString filename = m_filePickerSqlite->GetPath();
    filename.Trim().Trim(false);
    if(filename.IsEmpty()) return;

    files.Insert(filename, 0);
    settings.SetRecentFiles(files);
    config.WriteItem(&settings);
}

wxArrayString DbSettingDialog::DoLoadSqliteHistory()
{
    clConfig config(DBE_CONFIG_FILE);
    DbExplorerSettings settings;
    config.ReadItem(&settings);
    return settings.GetRecentFiles();
}

DbConnectionInfoVec DbSettingDialog::DoLoadMySQLHistory()
{
    clConfig config(DBE_CONFIG_FILE);
    DbExplorerSettings settings;
    config.ReadItem(&settings);
    return settings.GetMySQLConnections();
}

DbConnectionInfoVec DbSettingDialog::DoLoadPgSQLHistory()
{
    clConfig config(DBE_CONFIG_FILE);
    DbExplorerSettings settings;
    config.ReadItem(&settings);
    return settings.GetPgSQLConnections();
}

void DbSettingDialog::DoSaveMySQLHistory()
{
    clConfig config(DBE_CONFIG_FILE);
    DbExplorerSettings settings;
    config.ReadItem(&settings);
    DbConnectionInfoVec mysql = settings.GetMySQLConnections();

    DbConnectionInfo conn;
    conn.SetConnectionType(DbConnectionInfo::DbConnTypeMySQL);
    conn.SetDefaultDatabase(wxT(""));
    conn.SetConnectionName(m_txName->GetValue());
    conn.SetPassword(m_txPassword->GetValue());
    conn.SetServer(m_txServer->GetValue());
    conn.SetUsername(m_txUserName->GetValue());

    if(!conn.IsValid()) return;

    // remove any connection with this name
    DbConnectionInfoVec::iterator iter = mysql.begin();
    for(; iter != mysql.end(); ++iter) {
        if(iter->GetConnectionName() == conn.GetConnectionName()) {
            mysql.erase(iter);
            break;
        }
    }

    mysql.insert(mysql.begin(), conn);
    settings.SetMySQLConnections(mysql);
    config.WriteItem(&settings);
}

void DbSettingDialog::DoSavePgSQLHistory()
{
    clConfig config(DBE_CONFIG_FILE);
    DbExplorerSettings settings;
    config.ReadItem(&settings);
    DbConnectionInfoVec pgconns = settings.GetPgSQLConnections();

    long port = 0;
    DbConnectionInfo conn;
    conn.SetConnectionType(DbConnectionInfo::DbConnTypePgSQL);
    conn.SetConnectionName(m_txPgName->GetValue());
    conn.SetDefaultDatabase(m_txPgDatabase->GetValue());
    conn.SetPassword(m_txPgPassword->GetValue());
    conn.SetServer(m_txPgServer->GetValue());
    m_txPgPort->GetValue().ToLong(&port);
    conn.SetPort(port);
    conn.SetUsername(m_txPgUserName->GetValue());

    if(!conn.IsValid()) return;

    // remove any connection with this name
    DbConnectionInfoVec::iterator iter = pgconns.begin();
    for(; iter != pgconns.end(); iter++) {
        if(iter->GetConnectionName() == conn.GetConnectionName()) {
            pgconns.erase(iter);
            break;
        }
    }

    pgconns.insert(pgconns.begin(), conn);
    settings.SetPgSQLConnections(pgconns);
    config.WriteItem(&settings);
}

void DbSettingDialog::DoFindConnectionByName(const DbConnectionInfoVec& conns, const wxString& name)
{
    for(size_t i = 0; i < conns.size(); i++) {
        if(conns.at(i).GetConnectionName() == name) {
            // we found the selected connection
            if(conns.at(i).GetConnectionType() == DbConnectionInfo::DbConnTypeMySQL) {
                // populate the MySQL fields
                m_txName->SetValue(conns.at(i).GetConnectionName());
                m_txServer->SetValue(conns.at(i).GetServer());
                m_txUserName->SetValue(conns.at(i).GetUsername());
                m_txPassword->SetValue(conns.at(i).GetPassword());

            } else {
                // populate the PgSQL fields
                m_txPgName->SetValue(conns.at(i).GetConnectionName());
                m_txPgServer->SetValue(conns.at(i).GetServer());
                m_txPgPort->SetValue(wxString::Format(wxT("%i"), conns.at(i).GetPort()));
                m_txPgUserName->SetValue(conns.at(i).GetUsername());
                m_txPgPassword->SetValue(conns.at(i).GetPassword());
                m_txPgDatabase->SetValue(conns.at(i).GetDefaultDatabase());
            }
            return;
        }
    }
}
