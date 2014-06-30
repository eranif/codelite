//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : database.cpp
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

#include "database.h"
#include "table.h"
#include "IDbAdapter.h"
XS_IMPLEMENT_CLONABLE_CLASS(Database,xsSerializable);

Database::Database()
{
    m_name = wxT("");
    m_pDbAdapter = NULL;
}

Database::Database(const Database& obj):xsSerializable(obj)
{
    m_name = obj.m_name;
    m_pDbAdapter = obj.m_pDbAdapter;
}

Database::Database(IDbAdapter* dbAdapter,const wxString& dbName)
{
    m_name = dbName;
    if (dbAdapter) {
        m_pDbAdapter = dbAdapter;
        m_pDbAdapter->GetTables(this, false);

        SerializableList::compatibility_iterator tabNode = GetFirstChildNode();
        while(tabNode) {
            Table* pTab = wxDynamicCast(tabNode->GetData(), Table);
            if (pTab) {
                m_pDbAdapter->GetColumns(pTab);
            }
            tabNode = tabNode->GetNext();
        }
        m_pDbAdapter->GetViews(this);
    }
}

Database::~Database()
{
    //delete this->tables;
}
void Database::RefreshChildren( bool includeViews)
{
    GetChildrenList().DeleteContents(true);
    GetChildrenList().Clear();
    if (m_pDbAdapter) {
        m_pDbAdapter->GetTables(this, includeViews);

        SerializableList::compatibility_iterator tabNode = GetFirstChildNode();
        while(tabNode) {
            Table* pTab = wxDynamicCast(tabNode->GetData(), Table);
            if (pTab) {
                m_pDbAdapter->GetColumns(pTab);
            }
            tabNode = tabNode->GetNext();
        }
        m_pDbAdapter->GetViews(this);
    }
}

void Database::RefreshChildrenDetails()
{
    SerializableList::compatibility_iterator tabNode = GetFirstChildNode();
    while(tabNode) {
        Table* pTab = wxDynamicCast(tabNode->GetData(),Table);
        if (pTab) pTab->RefreshChildren();
        tabNode = tabNode->GetNext();
    }
}
