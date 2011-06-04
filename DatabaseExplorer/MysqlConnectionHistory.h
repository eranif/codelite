#ifndef MYSQLCONNECTIONHISTORY_H
#define MYSQLCONNECTIONHISTORY_H

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/wxxmlserializer/XmlSerializer.h>

class MysqlConnectionHistory : public xsSerializable {

public:
	XS_DECLARE_CLONABLE_CLASS(MysqlConnectionHistory);
	MysqlConnectionHistory();
	MysqlConnectionHistory(const MysqlConnectionHistory& obj);
	virtual ~MysqlConnectionHistory();

	static bool SaveToFile(xsSerializable* node);
	static MysqlConnectionHistory* LoadFromFile();
	
protected:
	void InitSerialize();


};



class MysqlHistItem : public xsSerializable {
public:
	XS_DECLARE_CLONABLE_CLASS(MysqlHistItem);
	MysqlHistItem();
	MysqlHistItem(const MysqlHistItem& obj);
	MysqlHistItem(const wxString& name, const wxString& server, const wxString& userName, const wxString& password);
	virtual ~MysqlHistItem();
	
	
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
	
protected:
	void InitSerialize();
	wxString m_name;
	wxString m_server;
	wxString m_userName;
	wxString m_password;
};


#endif // MYSQLCONNECTIONHISTORY_H
