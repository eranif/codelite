//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : db_explorer_settings.h
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

#ifndef DBEXPLORERSETTINGS_H
#define DBEXPLORERSETTINGS_H

#include "serialized_object.h"
#include "cl_config.h"

#define DBE_CONFIG      "database-explorer"
#define DBE_CONFIG_FILE "database-explorer.conf"

class DbConnectionInfo : public clConfigItem
{
protected:
    wxString m_connectionName;
    wxString m_server;
    int      m_port;
    wxString m_password;
    wxString m_username;
    wxString m_defaultDatabase;
    int      m_connectionType;

public:
    enum {
        DbConnTypeMySQL,
        DbConnTypePgSQL
    };

public:
    DbConnectionInfo();
    virtual ~DbConnectionInfo();

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    void SetConnectionName(const wxString& connectionName) {
        this->m_connectionName = connectionName;
    }
    void SetPassword(const wxString& password) {
        this->m_password = password;
    }
    void SetServer(const wxString& server) {
        this->m_server = server;
    }
    void SetPort(const int port) {
        this->m_port = port;
    }
    void SetUsername(const wxString& username) {
        this->m_username = username;
    }
    const wxString& GetConnectionName() const {
        return m_connectionName;
    }
    const wxString& GetPassword() const {
        return m_password;
    }
    const wxString& GetServer() const {
        return m_server;
    }
    const int  GetPort() const {
        return m_port;
    }
    const wxString& GetUsername() const {
        return m_username;
    }
    void SetDefaultDatabase(const wxString& defaultDatabase) {
        this->m_defaultDatabase = defaultDatabase;
    }
    const wxString& GetDefaultDatabase() const {
        return m_defaultDatabase;
    }
    void SetConnectionType(int connectionType) {
        this->m_connectionType = connectionType;
    }
    int GetConnectionType() const {
        return m_connectionType;
    }

    bool IsValid() const {
        return  !m_connectionName.IsEmpty() &&
                !m_server.IsEmpty() &&
                !m_username.IsEmpty() &&
                !m_password.IsEmpty();
    }
};

typedef std::vector<DbConnectionInfo> DbConnectionInfoVec;

class DbExplorerSettings : public clConfigItem
{
protected:
    wxArrayString       m_recentFiles;
    DbConnectionInfoVec m_connections;
    wxArrayString       m_sqlHistory;

public:
    DbExplorerSettings();
    virtual ~DbExplorerSettings();

    void SetRecentFiles(const wxArrayString& recentFiles);
    const wxArrayString& GetRecentFiles() const {
        return m_recentFiles;
    }

    DbConnectionInfoVec GetPgSQLConnections();
    void SetPgSQLConnections(const DbConnectionInfoVec& conns);

    DbConnectionInfoVec GetMySQLConnections();
    void SetMySQLConnections(const DbConnectionInfoVec& conns);

    void SetSqlHistory(const wxArrayString& sqlHistory) {
        this->m_sqlHistory = sqlHistory;
    }
    const wxArrayString& GetSqlHistory() const {
        return m_sqlHistory;
    }
    virtual void FromJSON(const JSONItem &json);
    virtual JSONItem ToJSON() const;
};

#endif // DBEXPLORERSETTINGS_H
