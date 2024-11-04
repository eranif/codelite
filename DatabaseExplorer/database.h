//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : database.h
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

#ifndef DATABASE_H
#define DATABASE_H
#include <wx/wx.h>
#include <wx/dblayer/include/DatabaseResultSet.h>
#include <wx/wxxmlserializer/XmlSerializer.h>


// -------------------------------------------------
// Trida database
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
