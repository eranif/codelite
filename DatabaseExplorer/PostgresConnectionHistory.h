#ifndef POSTGRESCONNECTIONHISTORY_H
#define POSTGRESCONNECTIONHISTORY_H

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/wxxmlserializer/XmlSerializer.h>

class PostgresConnectionHistory : public xsSerializable {

public:
	XS_DECLARE_CLONABLE_CLASS(PostgresConnectionHistory);
	PostgresConnectionHistory();
	PostgresConnectionHistory(const PostgresConnectionHistory& obj);
	virtual ~PostgresConnectionHistory();

	static bool SaveToFile(xsSerializable* node);
	static PostgresConnectionHistory* LoadFromFile();

protected:
	void InitSerialize();


};



class PostgresHistItem : public xsSerializable {
public:
	XS_DECLARE_CLONABLE_CLASS(PostgresHistItem);
	PostgresHistItem();
	PostgresHistItem(const PostgresHistItem& obj);
	PostgresHistItem(const wxString& name, const wxString& server, const wxString& userName, const wxString& password, const wxString& database);
	virtual ~PostgresHistItem();


	const wxString& GetName() const {
		return m_name;
	}
	const wxString& GetPassword() const {
		return m_password;
	}
	const wxString& GetServer() const {
		return m_server;
	}
	const wxString& GetUserName() const {
		return m_userName;
	}
	const wxString& GetDatabase() const {
		return m_database;
	}
protected:
	void InitSerialize();
	wxString m_name;
	wxString m_server;
	wxString m_userName;
	wxString m_password;
	wxString m_database;
};


#endif // POSTGRESCONNECTIONHISTORY_H
