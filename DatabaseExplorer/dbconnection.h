#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <wx/wx.h>
#include <wx/dblayer/include/DatabaseResultSet.h>
#include <wx/wxxmlserializer/XmlSerializer.h>
#include "IDbAdapter.h"
/*! \brief Class representing one db conneciton */
class DbConnection : public xsSerializable {

public:
	XS_DECLARE_CLONABLE_CLASS(DbConnection);
	/*! \brief Default constructors */
	DbConnection();
	DbConnection(IDbAdapter* dbAdapter, const wxString& serverName);
	DbConnection(const DbConnection& obj);
	/*! \brief Default destructor */
	virtual ~DbConnection();
	/*! \brief Return server name */
	wxString GetServerName() { return m_serverName; }
	/*! \brief Load databases from connection */
	void Load();
	/*! \brief Return IDbAdapter */
	IDbAdapter* GetDbAdapter() { return m_pDbAdapter; }
	/*! \brief Refresh all children. (databases) */
	void RefreshChildren();
	
protected:
	IDbAdapter* m_pDbAdapter;
	wxString m_serverName;
};

#endif // DBCONNECTION_H
