#ifndef DBEXPLORERSETTINGS_H
#define DBEXPLORERSETTINGS_H

#include "serialized_object.h"

class DbConnectionInfo : public SerializedObject
{
protected:
	wxString m_connectionName;
	wxString m_server;
	wxString m_password;
	wxString m_username;
	wxString m_defaultDatabase;
	int      m_connectionType;

public:
	enum {
		DbConnTypeMySQL,
		DbConnTypePgSQL
	};

public:
	DbConnectionInfo();
	virtual ~DbConnectionInfo();

	virtual void DeSerialize(Archive& arch);
	virtual void Serialize(Archive& arch);

	void SetConnectionName(const wxString& connectionName) {
		this->m_connectionName = connectionName;
	}
	void SetPassword(const wxString& password) {
		this->m_password = password;
	}
	void SetServer(const wxString& server) {
		this->m_server = server;
	}
	void SetUsername(const wxString& username) {
		this->m_username = username;
	}
	const wxString& GetConnectionName() const {
		return m_connectionName;
	}
	const wxString& GetPassword() const {
		return m_password;
	}
	const wxString& GetServer() const {
		return m_server;
	}
	const wxString& GetUsername() const {
		return m_username;
	}
	void SetDefaultDatabase(const wxString& defaultDatabase) {
		this->m_defaultDatabase = defaultDatabase;
	}
	const wxString& GetDefaultDatabase() const {
		return m_defaultDatabase;
	}
	void SetConnectionType(int connectionType) {
		this->m_connectionType = connectionType;
	}
	int GetConnectionType() const {
		return m_connectionType;
	}
	
	bool IsValid() const {
		return  !m_connectionName.IsEmpty() &&
				!m_server.IsEmpty() &&
				!m_username.IsEmpty() &&
				!m_password.IsEmpty();
	}
};

typedef std::vector<DbConnectionInfo> DbConnectionInfoVec;

class DbExplorerSettings : public SerializedObject
{
protected:
	wxArrayString       m_recentFiles;
	DbConnectionInfoVec m_connections;

public:
	DbExplorerSettings();
	virtual ~DbExplorerSettings();

	void SetRecentFiles(const wxArrayString& recentFiles);
	const wxArrayString& GetRecentFiles() const {
		return m_recentFiles;
	}

	DbConnectionInfoVec GetPgSQLConnections();
	void SetPgSQLConnections(const DbConnectionInfoVec& conns);

	DbConnectionInfoVec GetMySQLConnections();
	void SetMySQLConnections(const DbConnectionInfoVec& conns);

	virtual void DeSerialize(Archive& arch);
	virtual void Serialize(Archive& arch);
};

#endif // DBEXPLORERSETTINGS_H
