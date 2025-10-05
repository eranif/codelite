//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : PostgreSqlType.cpp
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

#include "PostgreSqlType.h"
XS_IMPLEMENT_CLONABLE_CLASS(PostgreSqlType,IDbType);
PostgreSqlType::PostgreSqlType() {
	m_dbtPropertyFlags = 0;
	InitSerialize();
}

PostgreSqlType::PostgreSqlType(const PostgreSqlType& obj):IDbType(obj) {
	m_typeName = obj.m_typeName;
	m_dbtPropertyFlags = obj.m_dbtPropertyFlags;
	m_autoIncrement = obj.m_autoIncrement;
	m_notNull = obj.m_notNull;
	m_primaryKey = obj.m_primaryKey;
	m_size = obj.m_size;
	m_size2 = obj.m_size2;
	m_unique = obj.m_unique;
	m_universalType = obj.m_universalType;
	InitSerialize();
}

PostgreSqlType::PostgreSqlType(const wxString& typeName, long propertyFlags, UNIVERSAL_TYPE universalType) {
	m_typeName = typeName;
	m_dbtPropertyFlags = propertyFlags;
	m_universalType = universalType;

	m_autoIncrement = false;
	m_notNull = false;
	m_primaryKey = false;
	m_size = 0;
	m_size2 = 0;
	m_unique = false;
	InitSerialize();
}

wxString PostgreSqlType::ReturnSql() {
	wxString sql;
	sql = wxString::Format(wxT(" %s"), m_typeName.c_str());
	if ((m_dbtPropertyFlags & dbtSIZE)&&(m_dbtPropertyFlags & dbtSIZE_TWO)&&(m_size > 0)&&(m_size2>0)) sql.append(wxString::Format(wxT("(%li,%li)"),m_size, m_size2));
	else if ((m_dbtPropertyFlags & dbtSIZE)&&(m_size > 0 )) sql.append(wxString::Format(wxT("(%li)"),m_size));
	if ((m_dbtPropertyFlags & dbtNOT_NULL) && m_notNull) sql.append(wxT(" NOT NULL"));
	return sql;
}

void PostgreSqlType::InitSerialize() {
	XS_SERIALIZE(m_typeName,wxT("m_typeName"));
	XS_SERIALIZE_LONG(m_dbtPropertyFlags,wxT("m_dbtPropertyFlags"));
	XS_SERIALIZE_LONG(m_size, wxT("m_size"));
	XS_SERIALIZE_LONG(m_size2, wxT("m_size2"));
	XS_SERIALIZE(m_unique, wxT("m_unique"));
	XS_SERIALIZE(m_primaryKey, wxT("m_primaryKey"));
	XS_SERIALIZE(m_notNull, wxT("m_notNull"));
	XS_SERIALIZE(m_autoIncrement, wxT("m_autoIncrement"));
	XS_SERIALIZE_LONG(m_universalType, wxT("m_universalType"));
}


IDbType::UNIVERSAL_TYPE PostgreSqlType::GetUniversalType() {
	return (IDbType::UNIVERSAL_TYPE )m_universalType;
}
