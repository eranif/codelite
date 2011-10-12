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
class DBEColumn : public xsSerializable {
protected:
	wxString m_name;
	wxString m_parentName;
	
	IDbType* m_pType;
	void initSerializable();
	
public:
	// -------------------------------------------------
	// Konstruktor
	// -------------------------------------------------
	XS_DECLARE_CLONABLE_CLASS(DBEColumn);
	/*! \brief Default constuctor*/
	DBEColumn();
	DBEColumn(const DBEColumn& obj);
	DBEColumn(const wxString& name,
			const wxString& parentName,
			IDbType* type);
	/*! \brief Default destruktor */
	virtual ~DBEColumn();
	
	wxString FormatName() {
		wxString typeDesc;
		if(m_pType) {
			typeDesc << m_pType->GetTypeName();
			if(m_pType->GetSize()) {
				typeDesc << wxT(" (") << m_pType->GetSize() << wxT(")");
			}
		}
		
		typeDesc.Trim().Trim(false);
		if(typeDesc.IsEmpty())
			typeDesc << wxT("<UNKNOWN>");
		
		wxString formattedName = wxString::Format( wxT("%s : %s"), GetName().c_str(), typeDesc.c_str());
		return formattedName;
	}
	
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
