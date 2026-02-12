//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : view.h
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

#ifndef VIEW_H
#define VIEW_H

#include "IDbAdapter.h"
#include "wx/wxxmlserializer/XmlSerializer.h"

#include <wx/dblayer/include/DatabaseResultSet.h>

// ------------------------------------------------
// Trida View
// -------------------------------------------------
/*! \brief Class representing database view. */
class View : public xsSerializable {

public:
	XS_DECLARE_CLONABLE_CLASS(View);
	/*! \brief Default constructors */
	View();
	View(const View& obj);
	View(IDbAdapter* dbAdapter,const wxString& name, const wxString& parentName, const wxString& select);
	/*! \brief Default destructors */
	virtual ~View() = default;

	/*! \brief Set view name */
	void SetName(const wxString & name) {
		this->m_name = name;
	}
	/*! \brief Set parent name (Database) */
	void SetParentName(const wxString & parentName) {
		this->m_parentName = parentName;
	}
	/*! \brief Set SELECT string */
	void SetSelect(const wxString & select) {
		this->m_select = select;
	}
	/*! \brief Get view name */
	const wxString & GetName() const {
		return m_name;
	}
	/*! \brief Get paretn name (database) */
	const wxString & GetParentName() const {
		return m_parentName;
	}
	/*! \brief Get SELECT string */
	const wxString & GetSelect() const {
		return m_select;
	}
	/*! \brief Get IDbAdapter */
	IDbAdapter * GetDbAdapter() {
		return m_pDbAdapter;
	}

protected:
	wxString m_name;
	wxString m_parentName;
	wxString m_select;
	void initSerializable();
	IDbAdapter* m_pDbAdapter;
};

#endif // VIEW_H
