#ifndef TABLE_H
#define TABLE_H

#include <wx/wx.h>
#include <wx/dblayer/include/DatabaseResultSet.h>
#include <wx/wxxmlserializer/XmlSerializer.h>
#include "column.h"
#include "constraint.h"
#include "IDbAdapter.h"
// -------------------------------------------------
// Trida databazove tabulky
// -------------------------------------------------
/*! \brief Class representing one database table */
class DBETable : public xsSerializable {
protected:
	wxString m_name;
	wxString m_parentName;
	IDbAdapter* m_pDbAdapter;
	int m_rowCount;	
	bool m_isSaved;	
	bool m_isView;
	
	
	void initSerializable();
	
	
public:
	XS_DECLARE_CLONABLE_CLASS(DBETable);	
	/*! \brief Default consturctors */
	DBETable();
	DBETable(const DBETable& obj);
	DBETable(IDbAdapter* dbAdapter,const wxString& tableName, const wxString& parentName, bool isView);
	/*! \brief Default destructor*/
	virtual ~DBETable();
	/*! \brief Return table name */
	wxString GetName() { return this->m_name; }
	/*! \brief Return parent name, usually dbName */
	wxString GetParentName() { return this->m_parentName; }
	/*! \brief Reload children db structure. */
	void RefreshChildren();
	/*! \brief Set table name */
	void SetName(const wxString& name) ;
	
	/*! \brief DEPRECATED. Return row count */
	int GetRowCount() { return this->m_rowCount; }
	/*! \brief DEPRECATED. Check if table modifications are saved */
	bool IsSaved() { return this->m_isSaved; }
	/*! \brief Add column */
	void AddColumn(DBEColumn* col) { this->AddChild(col); }
	/*! \brief Add constraint */
	void AddConstraint(Constraint* cont) { this->AddChild(cont); } 
	/*! \brief Return first talbe column */	
	DBEColumn* GetFristColumn() { return (DBEColumn*) GetFirstChild( CLASSINFO(DBEColumn)); }
	/*! \brief Return connected dbAdapter. */
	IDbAdapter* GetDbAdapter() { return m_pDbAdapter; }
	/*! \brief Is true, if table object was construted on view base. This feature is usisng in code generating. */
	bool IsView() { return m_isView;  }
	/*! \brief Set view parameter. More info in IsView() */
	void SetView(bool view) { m_isView = view; }
};

#endif // TABLE_H
