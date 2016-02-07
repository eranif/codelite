//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : dbconnection.h
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

#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <wx/wx.h>
#include <wx/dblayer/include/DatabaseResultSet.h>
#include <wx/wxxmlserializer/XmlSerializer.h>
#include "IDbAdapter.h"
/*! \brief Class representing one db connection */
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
