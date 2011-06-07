#include "db_explorer_settings.h"

DbExplorerSettings::DbExplorerSettings()
{
}

DbExplorerSettings::~DbExplorerSettings()
{
}

void DbExplorerSettings::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_recentFiles"), m_recentFiles);
	
	// Read the connections
	m_connections.clear();
	size_t num_of_connections(0);
	arch.Read(wxT("num_of_connections"), num_of_connections);
	for(size_t i=0; i<num_of_connections; i++) {
		DbConnectionInfo connInfo;
		wxString connId = wxString::Format(wxT("connection_%u"), i);
		arch.Read(connId, &connInfo);
		m_connections.push_back(connInfo);
	}
}

void DbExplorerSettings::Serialize(Archive& arch)
{
	arch.Write(wxT("m_recentFiles"), m_recentFiles);
	
	// Write the connections
	arch.Write(wxT("num_of_connections"), m_connections.size());
	for(size_t i=0; i<m_connections.size(); i++) {
		wxString connId = wxString::Format(wxT("connection_%u"), i);
		arch.Write(connId, &m_connections.at(i));
	}
}

void DbExplorerSettings::SetRecentFiles(const wxArrayString& recentFiles)
{
	m_recentFiles.Clear();
	for(size_t i=0; i<recentFiles.Count(); i++) {
		if(m_recentFiles.Index(recentFiles.Item(i)) == wxNOT_FOUND) {
			m_recentFiles.Add(recentFiles.Item(i));
		}
	}
}

//---------------------------------------------------
// Database Connection info
//---------------------------------------------------

DbConnectionInfo::DbConnectionInfo()
	: m_connectionType(DbConnTypeMySQL)
{
}

DbConnectionInfo::~DbConnectionInfo()
{
}

void DbConnectionInfo::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_connectionName") , m_connectionName );
	arch.Read(wxT("m_connectionType") , m_connectionType );
	arch.Read(wxT("m_defaultDatabase"), m_defaultDatabase);
	arch.Read(wxT("m_password")       , m_password       );
	arch.Read(wxT("m_server")         , m_server         );
	arch.Read(wxT("m_username")       , m_username       );
}

void DbConnectionInfo::Serialize(Archive& arch)
{
	arch.Write(wxT("m_connectionName") , m_connectionName );
	arch.Write(wxT("m_connectionType") , m_connectionType );
	arch.Write(wxT("m_defaultDatabase"), m_defaultDatabase);
	arch.Write(wxT("m_password")       , m_password       );
	arch.Write(wxT("m_server")         , m_server         );
	arch.Write(wxT("m_username")       , m_username       );
}

DbConnectionInfoVec DbExplorerSettings::GetMySQLConnections()
{
	DbConnectionInfoVec conns;
	for(size_t i=0; i<m_connections.size(); i++) {
		if(m_connections.at(i).GetConnectionType() == DbConnectionInfo::DbConnTypeMySQL) {
			conns.push_back(m_connections.at(i));
		}
	}
	return conns;
}

DbConnectionInfoVec DbExplorerSettings::GetPgSQLConnections()
{
	DbConnectionInfoVec conns;
	for(size_t i=0; i<m_connections.size(); i++) {
		if(m_connections.at(i).GetConnectionType() == DbConnectionInfo::DbConnTypePgSQL) {
			conns.push_back(m_connections.at(i));
		}
	}
	return conns;
}

void DbExplorerSettings::SetMySQLConnections(const DbConnectionInfoVec& conns)
{
	DbConnectionInfoVec pgconns = GetPgSQLConnections();
	m_connections.clear();
	
	m_connections.insert(m_connections.end(), pgconns.begin(), pgconns.end());
	m_connections.insert(m_connections.end(), conns.begin(), conns.end());
}

void DbExplorerSettings::SetPgSQLConnections(const DbConnectionInfoVec& conns)
{
	DbConnectionInfoVec myconns = GetMySQLConnections();
	m_connections.clear();
	
	m_connections.insert(m_connections.end(), myconns.begin(), myconns.end());
	m_connections.insert(m_connections.end(), conns.begin(), conns.end());
}

