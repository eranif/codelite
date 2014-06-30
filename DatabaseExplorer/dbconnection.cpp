//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : dbconnection.cpp
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

#include "dbconnection.h"

XS_IMPLEMENT_CLONABLE_CLASS(DbConnection,xsSerializable);

DbConnection::DbConnection()
{
	m_pDbAdapter = NULL;
}
DbConnection::DbConnection(IDbAdapter* dbAdapter, const wxString& serverName)
{
	m_serverName = serverName;
	m_pDbAdapter = dbAdapter;	
	Load();
}

DbConnection::DbConnection(const DbConnection& obj) : xsSerializable(obj)
{
	m_serverName = obj.m_serverName;
	m_pDbAdapter = obj.m_pDbAdapter;
}
DbConnection::~DbConnection()
{
	if (m_pDbAdapter) {
        delete m_pDbAdapter;
        m_pDbAdapter = NULL;
    }
}

void DbConnection::Load()
{
	if (m_pDbAdapter)
		if (m_pDbAdapter->CanConnect()){		
			m_pDbAdapter->GetDatabases(this);		
			}
}
void DbConnection::RefreshChildren()
{
	GetChildrenList().DeleteContents(true);
	GetChildrenList().Clear();
	Load();	
}

