//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : column.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
	// Constructor
	// -------------------------------------------------
	XS_DECLARE_CLONABLE_CLASS(Column);
	/*! \brief Default constructor*/
	Column();
	Column(const Column& obj);
	Column(const wxString& name,
			const wxString& parentName,
			IDbType* type);
	/*! \brief Default destructor */
	virtual ~Column();
	
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
	IDbType* GetType(){ return this->m_pType; }
	/*! \brief Set IDbType */
	void SetType(IDbType* pType ) { this->m_pType = pType; }
	
	/*! \brief Function for column editing. */
	void Edit(wxString& name,
			wxString& parentName,
			IDbType* type);
	
};

#endif // COLUMN_H
