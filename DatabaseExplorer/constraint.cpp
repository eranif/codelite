//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : constraint.cpp
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

#include "constraint.h"
XS_IMPLEMENT_CLONABLE_CLASS(Constraint,xsSerializable);
Constraint::Constraint()
{
	m_type = foreignKey;
	m_onDelete = restrict;
	m_onUpdate = restrict;
	InitSerializable();
}
Constraint::Constraint(const Constraint& obj):xsSerializable(obj)
{
	m_name = obj.m_name;
	m_localColumn = obj.m_localColumn;
	m_type = obj.m_type;
	m_refTable = obj.m_refTable;
	m_refCol = obj.m_refCol;
	m_onDelete = obj.m_onDelete;
	m_onUpdate = obj.m_onUpdate;
	InitSerializable();
	
}
Constraint::Constraint(const wxString& name, const wxString& localColumn, constraintType type, constraintAction onDelete, constraintAction onUpdate)
{
	m_name = name;
	m_localColumn = localColumn;
	m_type = type;
	m_onDelete = onDelete;
	m_onUpdate = onUpdate;
	InitSerializable();
}

void Constraint::InitSerializable()
{
	XS_SERIALIZE(m_name, wxT("name"));
	XS_SERIALIZE(m_localColumn, wxT("localColumn"));
	XS_SERIALIZE_INT(m_type, wxT("type"));
	XS_SERIALIZE(m_refTable, wxT("refTable"));
	XS_SERIALIZE(m_refCol, wxT("refCol"));
	XS_SERIALIZE_INT(m_onDelete,wxT("onDelete"));
	XS_SERIALIZE_INT(m_onUpdate,wxT("onUpdate"));
	
}


