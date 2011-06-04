#ifndef DATABASE_H
#define DATABASE_H
#include <wx/wx.h>
#include <wx/dblayer/include/DatabaseResultSet.h>
#include <wx/wxxmlserializer/XmlSerializer.h>


// -------------------------------------------------
// Trida databaze
// -------------------------------------------------
class IDbAdapter;
/*! \brief Class representing one database */
class Database : public xsSerializable {
	
	
protected:
	wxString m_name;
	IDbAdapter* m_pDbAdapter;
	
public:
	XS_DECLARE_CLONABLE_CLASS(Database);
	/*! \brief Default consturctors */
	Database();
	Database(const Database& obj);
	Database(IDbAdapter* dbAdapter,const wxString& dbName);
	/*! \brief Default destructor*/
	virtual ~Database();
	/*! \brief Refresh all children, if includeViews is true, all view will be load as tables.*/
	void RefreshChildren(bool includeViews);
	/*! \brief Refresh all children details (table columns atc.)  */
	void RefreshChildrenDetails();
	/*! \brief Return IDbAdapter */
	IDbAdapter* GetDbAdapter() { return this->m_pDbAdapter; }
	/*! \brief Return database name*/
	wxString GetName() { return this->m_name; }


};

#endif // DATABASE_H
