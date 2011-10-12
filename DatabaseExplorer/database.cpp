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
			DBETable* pTab = wxDynamicCast(tabNode->GetData(), DBETable);
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
			DBETable* pTab = wxDynamicCast(tabNode->GetData(), DBETable);
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
		DBETable* pTab = wxDynamicCast(tabNode->GetData(),DBETable);
		if (pTab) pTab->RefreshChildren();
		tabNode = tabNode->GetNext();
	}
}
