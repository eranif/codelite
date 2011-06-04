#ifndef COLUMN_H
#define COLUMN_H
#include <wx/wx.h>
#include <wx/dblayer/include/DatabaseResultSet.h>
#include <wx/wxxmlserializer/XmlSerializer.h>
#include "IDbAdapter.h"
#include "IDbType.h"
// ------------------------------------------------
// Trida databazoveho sloupecku
// -------------------------------------------------
/*! \brief Class representing one table column */
class Column : public xsSerializable {
protected:
	wxString m_name;
	wxString m_parentName;
	
	IDbType* m_pType;
	void initSerializable();
	
public:
	// -------------------------------------------------
	// Konstruktor
	// -------------------------------------------------
	XS_DECLARE_CLONABLE_CLASS(Column);
	/*! \brief Default constuctor*/
	Column();
	Column(const Column& obj);
	Column(const wxString& name,
			const wxString& parentName,
			IDbType* type);
	/*! \brief Default destruktor */
	virtual ~Column();
	
	
	/*! \brief Retrun column name */
	wxString GetName() { return this->m_name; }
	/*! \brief Set column name */
	void SetName(const wxString& name) { this->m_name = name; }
	/*! \brief Return parent name (table) */
	wxString GetParentName() { return this->m_parentName; }
	
	/*! \brief Return IDbType */
	IDbType* GetPType(){ return this->m_pType; }
	/*! \brief Set IDbType */
	void SetPType(IDbType* pType ) { this->m_pType = pType; }
	
	/*! \brief Function for column editing. */
	void Edit(wxString& name,
			wxString& parentName,
			IDbType* type);
	
};

#endif // COLUMN_H
