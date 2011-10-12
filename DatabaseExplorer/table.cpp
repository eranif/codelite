#include "table.h"

XS_IMPLEMENT_CLONABLE_CLASS(DBETable,xsSerializable);

DBETable::DBETable()
{
	m_isView = false;
	initSerializable();
}

DBETable::DBETable(const DBETable& obj): xsSerializable(obj)
{
	this->m_name = obj.m_name;
	this->m_parentName = obj.m_parentName;
	this->m_rowCount = obj.m_rowCount;
	this->m_isView = obj.m_isView;
	//this->m_lstChildItems = obj.m_lstChildItems;
	initSerializable();

}


DBETable::DBETable(IDbAdapter* dbAdapter, const wxString& tableName, const wxString& parentName, bool isView)
{
	this->m_name = tableName;
	this->m_parentName = parentName;
	this->m_rowCount = 0;
	this->m_pDbAdapter = dbAdapter;
	this->m_isView = isView;
	//dbAdapter->GetColumns(this);
	
	initSerializable();
}
DBETable::~DBETable()
{

}
void DBETable::SetName(const wxString& name)
{
	 this->m_isSaved = false; 
	 this->m_name = name; 
}

void DBETable::initSerializable()
{
	XS_SERIALIZE(this->m_name,wxT("tableName")); 
	XS_SERIALIZE(this->m_parentName,wxT("parentName")); 
	XS_SERIALIZE_INT(this->m_rowCount, wxT("rowCount"));
	XS_SERIALIZE(m_lstChildItems,wxT("columns"));
	XS_SERIALIZE(m_isView, wxT("isView"));
	
}
void DBETable::RefreshChildren()
{
	GetChildrenList().DeleteContents(true);
	GetChildrenList().Clear();
	if (m_pDbAdapter){
		m_pDbAdapter->GetColumns(this);
	}
}
