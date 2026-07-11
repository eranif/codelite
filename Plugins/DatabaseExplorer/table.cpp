//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : table.cpp
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

#include "table.h"

XS_IMPLEMENT_CLONABLE_CLASS(Table, xsSerializable);

Table::Table()
{
    m_isView = false;

    initSerializable();
}

Table::Table(const Table& obj)
    : xsSerializable(obj)
{
    this->m_name = obj.m_name;
    this->m_parentName = obj.m_parentName;
    this->m_rowCount = obj.m_rowCount;
    this->m_isView = obj.m_isView;
    // this->m_pDbAdapter = obj.m_pDbAdapter;

    initSerializable();
}

Table::Table(IDbAdapter* dbAdapter, const wxString& tableName, const wxString& parentName, bool isView)
{
    this->m_name = tableName;
    this->m_parentName = parentName;
    this->m_rowCount = 0;
    this->m_pDbAdapter = dbAdapter;
    this->m_isView = isView;

    initSerializable();
}

void Table::SetName(const wxString& name)
{
    this->m_isSaved = false;
    this->m_name = name;
}

void Table::initSerializable()
{
    XS_SERIALIZE(this->m_name, wxT("tableName"));
    XS_SERIALIZE(this->m_parentName, wxT("parentName"));
    XS_SERIALIZE_INT(this->m_rowCount, wxT("rowCount"));
    XS_SERIALIZE(this->m_lstChildItems, wxT("columns"));
    XS_SERIALIZE(this->m_isView, wxT("isView"));
}

void Table::RefreshChildren()
{
    GetChildrenList().DeleteContents(true);
    GetChildrenList().Clear();
    if (m_pDbAdapter) {
        m_pDbAdapter->GetColumns(this);
    }
}
