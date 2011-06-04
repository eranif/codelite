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
	if (m_pDbAdapter) delete m_pDbAdapter;
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

