#include "PostgresConnectionHistory.h"
#include "databaseexplorer.h"

XS_IMPLEMENT_CLONABLE_CLASS(PostgresConnectionHistory,xsSerializable);

PostgresConnectionHistory::PostgresConnectionHistory()
{
	InitSerialize();
}
PostgresConnectionHistory::PostgresConnectionHistory(const PostgresConnectionHistory& obj):xsSerializable(obj)
{
	InitSerialize();
}
PostgresConnectionHistory::~PostgresConnectionHistory()
{
}
void PostgresConnectionHistory::InitSerialize()
{
	//XS_SERIALIZE(m_lstChildItems,wxT("childrens"));
}

bool PostgresConnectionHistory::SaveToFile(xsSerializable* node)
{
	wxXmlSerializer serializer;
	serializer << node;
	serializer.SerializeToXml( DatabaseExplorer::GetManager()->GetInstallDirectory() + wxFileName::GetPathSeparator() + wxT("PostgresHistory.xml") );
	serializer.RemoveAll();
	return true;
}
PostgresConnectionHistory* PostgresConnectionHistory::LoadFromFile()
{
	wxXmlSerializer serializer;
	PostgresConnectionHistory *item = NULL;
	if (wxFile::Exists( DatabaseExplorer::GetManager()->GetInstallDirectory() + wxFileName::GetPathSeparator() + wxT("PostgresHistory.xml")) ){		
		if (serializer.DeserializeFromXml( DatabaseExplorer::GetManager()->GetInstallDirectory() + wxFileName::GetPathSeparator() + wxT("PostgresHistory.xml")) ){
			xsSerializable* ser = serializer.GetRootItem();
			item = wxDynamicCast(ser->GetFirstChild(), PostgresConnectionHistory);			
			if (item) item = (PostgresConnectionHistory*) item->Clone();			
			}		
		} 	
	//PostgresConnectionHistory *item =(PostgresConnectionHistory*)  serializer.GetRootItem();// wxDynamicCast(serializer.GetRootItem(), MysqlConnectionHistory);
	return item;
}
// ---------------------------------------------------------------------------------
// PostgresHistItem
// ---------------------------------------------------------------------------------
XS_IMPLEMENT_CLONABLE_CLASS(PostgresHistItem,xsSerializable);
PostgresHistItem::PostgresHistItem()
{
	InitSerialize();
}

PostgresHistItem::PostgresHistItem(const PostgresHistItem& obj):xsSerializable(obj)
{
	m_name = obj.m_name;
	m_server = obj.m_server;
	m_userName = obj.m_userName;
	m_password = obj.m_password;
	m_database = obj.m_database;
	InitSerialize();
}
PostgresHistItem::PostgresHistItem(const wxString& name, const wxString& server, const wxString& userName, const wxString& password, const wxString& database)
{
	m_name = name;
	m_server = server;
	m_userName = userName;
	m_password = password;
	m_database = database;
	InitSerialize();
}
PostgresHistItem::~PostgresHistItem()
{
}
void PostgresHistItem::InitSerialize()
{
	XS_SERIALIZE(m_name, wxT("m_name"));
	XS_SERIALIZE(m_server, wxT("m_server"));
	XS_SERIALIZE(m_userName, wxT("m_userName"));
	XS_SERIALIZE(m_password, wxT("m_password"));
	XS_SERIALIZE(m_database, wxT("m_database"));
}







