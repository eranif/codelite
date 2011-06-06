#include "MysqlConnectionHistory.h"
#include "databaseexplorer.h"

XS_IMPLEMENT_CLONABLE_CLASS(MysqlConnectionHistory,xsSerializable);

MysqlConnectionHistory::MysqlConnectionHistory()
{
	InitSerialize();
}
MysqlConnectionHistory::MysqlConnectionHistory(const MysqlConnectionHistory& obj):xsSerializable(obj)
{
	InitSerialize();
}
MysqlConnectionHistory::~MysqlConnectionHistory()
{
}
void MysqlConnectionHistory::InitSerialize()
{
	//XS_SERIALIZE(m_lstChildItems,wxT("childrens"));
}

bool MysqlConnectionHistory::SaveToFile(xsSerializable* node)
{
	wxXmlSerializer serializer;
	serializer << node;
	serializer.SerializeToXml( DatabaseExplorer::GetManager()->GetInstallDirectory() + wxFileName::GetPathSeparator() + wxT("MySqlHistory.xml") );
	serializer.RemoveAll();
	return true;
}
MysqlConnectionHistory* MysqlConnectionHistory::LoadFromFile()
{
	wxXmlSerializer serializer;
	MysqlConnectionHistory *item = NULL;
	wxString filename;
	filename << DatabaseExplorer::GetManager()->GetInstallDirectory() << wxFileName::GetPathSeparator() << wxT("MySqlHistory.xml");
	if ( wxFile::Exists(filename) ){		
		if (serializer.DeserializeFromXml( filename )){
			xsSerializable* ser = serializer.GetRootItem();
			item = wxDynamicCast(ser->GetFirstChild(), MysqlConnectionHistory);			
			if (item) item = (MysqlConnectionHistory*) item->Clone();			
			}		
		} 	
	//MysqlConnectionHistory *item =(MysqlConnectionHistory*)  serializer.GetRootItem();// wxDynamicCast(serializer.GetRootItem(), MysqlConnectionHistory);
	return item;
}
// ---------------------------------------------------------------------------------
// MysqlHistItem
// ---------------------------------------------------------------------------------
XS_IMPLEMENT_CLONABLE_CLASS(MysqlHistItem,xsSerializable);
MysqlHistItem::MysqlHistItem()
{
	InitSerialize();
}

MysqlHistItem::MysqlHistItem(const MysqlHistItem& obj):xsSerializable(obj)
{
	m_name = obj.m_name;
	m_server = obj.m_server;
	m_userName = obj.m_userName;
	m_password = obj.m_password;
	InitSerialize();
}
MysqlHistItem::MysqlHistItem(const wxString& name, const wxString& server, const wxString& userName, const wxString& password)
{
	m_name = name;
	m_server = server;
	m_userName = userName;
	m_password = password;
	InitSerialize();
}
MysqlHistItem::~MysqlHistItem()
{
}
void MysqlHistItem::InitSerialize()
{
	XS_SERIALIZE(m_name, wxT("m_name"));
	XS_SERIALIZE(m_server, wxT("m_server"));
	XS_SERIALIZE(m_userName, wxT("m_userName"));
	XS_SERIALIZE(m_password, wxT("m_password"));
}







