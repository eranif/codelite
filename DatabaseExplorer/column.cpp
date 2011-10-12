#include "column.h"

XS_IMPLEMENT_CLONABLE_CLASS(DBEColumn,xsSerializable);

DBEColumn::DBEColumn()
{
	m_pType = NULL;
	initSerializable();
}

DBEColumn::~DBEColumn()
{
	delete m_pType;
}

DBEColumn::DBEColumn(const DBEColumn& obj):xsSerializable(obj)
{
	m_pType = (IDbType*) obj.m_pType->Clone();
	m_name = obj.m_name;
	m_parentName = obj.m_parentName;
	initSerializable();
}

DBEColumn::DBEColumn(const wxString& name,
                     const wxString& parentName,
                     IDbType* type)
{
	this->m_name = name;
	this->m_parentName = parentName;
	this->m_pType = type;
	initSerializable();
}

void DBEColumn::Edit(wxString& name,
                     wxString& parentName,
                     IDbType* type)
{
	this->m_pType = NULL;
	this->m_name = name;
	this->m_parentName = parentName;
	if (this->m_pType) delete this->m_pType;
	this->m_pType = type;

}

void DBEColumn::initSerializable()
{
	XS_SERIALIZE(m_name,wxT("m_name"));
	XS_SERIALIZE(m_parentName,wxT("m_parentName"));
	XS_SERIALIZE_DYNAMIC_OBJECT(m_pType, wxT("type"));
}
