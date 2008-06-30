//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : variable_entry.cpp              
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
 #include "variable_entry.h"
#include "wx/log.h"

VariableEntry::VariableEntry(const wxString &name, const wxString &path)
: m_name(name)
, m_value(path)
{
}

VariableEntry::VariableEntry(wxSQLite3ResultSet &rs)
{
	m_name = rs.GetString(0);
	m_value = rs.GetString(1);
}

VariableEntry::VariableEntry(const VariableEntry &rhs)
{
	*this = rhs;
}

VariableEntry::VariableEntry()
{
}

VariableEntry::~VariableEntry()
{
}

VariableEntry& VariableEntry::operator =(const VariableEntry& rhs)
{
	if(this == &rhs)
		return *this;

	m_name = rhs.m_name;
	m_value = rhs.m_value;
	return *this;
}

bool VariableEntry::operator ==(const VariableEntry& rhs)
{
	return m_name == rhs.m_name && m_value == rhs.m_value;
}


//---------------------------------------------------------
// Database opertions
//---------------------------------------------------------

int VariableEntry::Store(wxSQLite3Statement& insertPreparedStmnt, TagsDatabase *db)
{
	try
	{
		// see TagsDatabase::GetInsertOneStatement() for the order of binding
		insertPreparedStmnt.Bind(1, GetName());
		insertPreparedStmnt.Bind(2, GetValue());
		insertPreparedStmnt.ExecuteUpdate();
		insertPreparedStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		if(exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		wxUnusedVar(exc);
		return TagError;
	}
	return TagOk;
}

int VariableEntry::Update(wxSQLite3Statement& updatePreparedStmnt)
{
	try
	{
		updatePreparedStmnt.Bind(1, GetValue());
		updatePreparedStmnt.Bind(2, GetName());
		updatePreparedStmnt.ExecuteUpdate();
		updatePreparedStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		wxUnusedVar(exc);
		return TagError;
	}
	return TagOk;
}

int VariableEntry::Delete(wxSQLite3Statement& deletePreparedStmnt)
{
	try
	{
		deletePreparedStmnt.Bind(1, GetName());	
		deletePreparedStmnt.ExecuteUpdate();
		deletePreparedStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		wxUnusedVar(exc);
		return TagError;
	}
	return TagOk;
}

wxString VariableEntry::GetDeleteOneStatement()
{
	return wxT("DELETE FROM VARIABLES WHERE NAME=?");	
}

wxString VariableEntry::GetUpdateOneStatement()
{
	return wxT("UPDATE VARIABLES SET VALUE=? WHERE NAME=?");
}

wxString VariableEntry::GetInsertOneStatement()
{
	return wxT("INSERT INTO VARIABLES VALUES (?, ?)");	
}

