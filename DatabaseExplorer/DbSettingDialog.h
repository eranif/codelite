//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : DbSettingDialog.h
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

#ifndef DBSETTINGDIALOG_H
#define DBSETTINGDIALOG_H

#include "GUI.h" // Base class: _DBSettingsDialog
#include <wx/wx.h>
#include "DbViewerPanel.h"
#include "db_explorer_settings.h"

#ifdef DBL_USE_MYSQL
#include "MySqlDbAdapter.h"
#endif

#ifdef DBL_USE_SQLITE
#include "SqliteDbAdapter.h"
#endif

#ifdef DBL_USE_POSTGRES
#include "PostgreSqlDbAdapter.h"
#endif

/*! \brief Dialog for connection to the database server. It should be change everytime, if new database server typ is
 * add. */
class DbSettingDialog : public _DBSettingsDialog
{
protected:
    DbViewerPanel* m_pParent;

public:
    DbSettingDialog(DbViewerPanel* parent, wxWindow* pWindowParent);
    virtual ~DbSettingDialog();

protected:
    void DoSaveSqliteHistory();
    wxArrayString DoLoadSqliteHistory();

    void DoSaveMySQLHistory();
    DbConnectionInfoVec DoLoadMySQLHistory();

    void DoSavePgSQLHistory();
    DbConnectionInfoVec DoLoadPgSQLHistory();

    void DoFindConnectionByName(const DbConnectionInfoVec& conns, const wxString& name);

    // Event handlers
    virtual void OnCancelClick(wxCommandEvent& event);
    virtual void OnMySqlOkClick(wxCommandEvent& event);
    virtual void OnSqliteOkClick(wxCommandEvent& event);
    virtual void OnHistoryClick(wxCommandEvent& event);
    virtual void OnHistoryDClick(wxCommandEvent& event);
    virtual void OnPgOkClick(wxCommandEvent& event);
    virtual void OnPgHistoryClick(wxCommandEvent& event);
    virtual void OnPgHistoryDClick(wxCommandEvent& event);
    virtual void OnItemActivated(wxListEvent& event);
    virtual void OnItemKeyDown(wxListEvent& event);
    virtual void OnItemSelected(wxListEvent& event);
    virtual void OnDlgOK(wxCommandEvent& event);

    void LoadHistory();
};

#endif // DBSETTINGDIALOG_H
