//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : db_explorer_settings.cpp
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

#include "db_explorer_settings.h"

// -----------------------------------------------------------------
// DbExplorerSettings
// -----------------------------------------------------------------

DbExplorerSettings::DbExplorerSettings()
    : clConfigItem(DBE_CONFIG)
{
}

DbExplorerSettings::~DbExplorerSettings()
{
}

void DbExplorerSettings::SetRecentFiles(const wxArrayString& recentFiles)
{
    m_recentFiles.Clear();
    for(size_t i=0; i<recentFiles.Count(); i++) {
        if(m_recentFiles.Index(recentFiles.Item(i)) == wxNOT_FOUND) {
            m_recentFiles.Add(recentFiles.Item(i));
        }
    }
}

DbConnectionInfoVec DbExplorerSettings::GetMySQLConnections()
{
    DbConnectionInfoVec conns;
    for(size_t i=0; i<m_connections.size(); i++) {
        if(m_connections.at(i).GetConnectionType() == DbConnectionInfo::DbConnTypeMySQL) {
            conns.push_back(m_connections.at(i));
        }
    }
    return conns;
}

DbConnectionInfoVec DbExplorerSettings::GetPgSQLConnections()
{
    DbConnectionInfoVec conns;
    for(size_t i=0; i<m_connections.size(); i++) {
        if(m_connections.at(i).GetConnectionType() == DbConnectionInfo::DbConnTypePgSQL) {
            conns.push_back(m_connections.at(i));
        }
    }
    return conns;
}

void DbExplorerSettings::SetMySQLConnections(const DbConnectionInfoVec& conns)
{
    DbConnectionInfoVec pgconns = GetPgSQLConnections();
    m_connections.clear();

    m_connections.insert(m_connections.end(), pgconns.begin(), pgconns.end());
    m_connections.insert(m_connections.end(), conns.begin(), conns.end());
}

void DbExplorerSettings::SetPgSQLConnections(const DbConnectionInfoVec& conns)
{
    DbConnectionInfoVec myconns = GetMySQLConnections();
    m_connections.clear();

    m_connections.insert(m_connections.end(), myconns.begin(), myconns.end());
    m_connections.insert(m_connections.end(), conns.begin(), conns.end());
}

void DbExplorerSettings::FromJSON(const JSONItem& json)
{
    m_recentFiles = json.namedObject("m_recentFiles").toArrayString();
    m_sqlHistory  = json.namedObject("m_sqlHistory").toArrayString();
    
    // read the connections
    JSONItem arrConnections = json.namedObject("connections");
    for(int i=0; i<arrConnections.arraySize(); ++i) {
        DbConnectionInfo ci;
        ci.FromJSON( arrConnections.arrayItem(i) );
        m_connections.push_back( ci );
    }
}

JSONItem DbExplorerSettings::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_recentFiles", m_recentFiles);
    element.addProperty("m_sqlHistory",  m_sqlHistory);
    
    // add the connections array
    JSONItem arrConnections = JSONItem::createArray("connections");
    element.append(arrConnections);
    DbConnectionInfoVec::const_iterator iter = m_connections.begin();
    for(; iter != m_connections.end(); ++iter) {
        arrConnections.arrayAppend( iter->ToJSON() );
    }
    return element;
}

//---------------------------------------------------
// Database Connection info
//---------------------------------------------------

DbConnectionInfo::DbConnectionInfo()
    : clConfigItem("connection-info")
    , m_connectionType(DbConnTypeMySQL)
{
}

DbConnectionInfo::~DbConnectionInfo()
{
}

JSONItem DbConnectionInfo::ToJSON() const
{
    JSONItem element = JSONItem::createObject(GetName());
    element.addProperty("m_connectionName",  m_connectionName );
    element.addProperty("m_connectionType",  m_connectionType );
    element.addProperty("m_defaultDatabase", m_defaultDatabase);
    element.addProperty("m_password"       , m_password       );
    element.addProperty("m_server"         , m_server         );
    element.addProperty("m_port"           , m_port           );
    element.addProperty("m_username"       , m_username       );
    return element;
}

void DbConnectionInfo::FromJSON(const JSONItem& json)
{
    m_connectionName  = json.namedObject("m_connectionName").toString(m_connectionName);
    m_connectionType  = json.namedObject("m_connectionType").toInt(m_connectionType);
    m_defaultDatabase = json.namedObject("m_defaultDatabase").toString(m_defaultDatabase);
    m_password        = json.namedObject("m_password").toString(m_password);
    m_server          = json.namedObject("m_server").toString(m_server);
    m_port            = json.namedObject("m_port").toInt(m_port);
    m_username        = json.namedObject("m_username").toString(m_username);
}
