//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : column.cpp
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

#include "column.h"

XS_IMPLEMENT_CLONABLE_CLASS(Column,xsSerializable);

Column::Column()
{
	m_pType = NULL;
	initSerializable();
}

Column::~Column()
{
	delete m_pType;
}

Column::Column(const Column& obj):xsSerializable(obj)
{
	m_pType = (IDbType*) obj.m_pType->Clone();
	m_name = obj.m_name;
	m_parentName = obj.m_parentName;
	initSerializable();
}

Column::Column(const wxString& name,
                     const wxString& parentName,
                     IDbType* type)
{
	this->m_name = name;
	this->m_parentName = parentName;
	this->m_pType = type;
	initSerializable();
}

void Column::Edit(wxString& name,
                     wxString& parentName,
                     IDbType* type)
{
	this->m_pType = NULL;
	this->m_name = name;
	this->m_parentName = parentName;
	if (this->m_pType) delete this->m_pType;
	this->m_pType = type;

}

void Column::initSerializable()
{
	XS_SERIALIZE(m_name,wxT("m_name"));
	XS_SERIALIZE(m_parentName,wxT("m_parentName"));
	XS_SERIALIZE_DYNAMIC_OBJECT(m_pType, wxT("type"));
}
