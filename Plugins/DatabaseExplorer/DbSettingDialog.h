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

#include "DbViewerPanel.h"
#include "GUI.hpp" // Base class: _DBSettingsDialog
#include "db_explorer_settings.h"

#include <wx/wx.h>

#ifdef DBL_USE_MYSQL
#include "MySqlDbAdapter.h"
#endif

#ifdef DBL_USE_SQLITE
#include "SqliteDbAdapter.h"
#endif

#ifdef DBL_USE_POSTGRES
#include "PostgreSqlDbAdapter.h"
#endif

/*! \brief Dialog for connection to the database server. It should be changed every time, if new database server type is
 * added. */
class DbSettingDialog : public _DBSettingsDialog
{
protected:
    DbViewerPanel* m_pParent;

public:
    DbSettingDialog(DbViewerPanel* parent, wxWindow* pWindowParent);
    ~DbSettingDialog() override = default;

protected:
    void DoSaveSqliteHistory();
    wxArrayString DoLoadSqliteHistory();

    void DoSaveMySQLHistory();
    DbConnectionInfoVec DoLoadMySQLHistory();

    void DoSavePgSQLHistory();
    DbConnectionInfoVec DoLoadPgSQLHistory();

    void DoFindConnectionByName(const DbConnectionInfoVec& conns, const wxString& name);
    bool DoSQLiteItemActivated();

    // Event handlers
    virtual void OnCancelClick(wxCommandEvent& event);
    virtual void OnMySqlOkClick(wxCommandEvent& event);
    virtual void OnSqliteOkClick(wxCommandEvent& event);
    void OnHistoryClick(wxCommandEvent& event) override;
    void OnHistoryDClick(wxCommandEvent& event) override;
    virtual void OnPgOkClick(wxCommandEvent& event);
    void OnPgHistoryClick(wxCommandEvent& event) override;
    void OnPgHistoryDClick(wxCommandEvent& event) override;
    void OnItemActivated(wxListEvent& event) override;
    void OnItemKeyDown(wxListEvent& event) override;
    void OnItemSelected(wxListEvent& event) override;
    void OnDlgOK(wxCommandEvent& event) override;

    void LoadHistory();
};

#endif // DBSETTINGDIALOG_H
