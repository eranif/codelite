//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : comment.cpp              
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
 #include "precompiled_header.h" 

#include "comment.h"
#include <iostream>

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

Comment::Comment(wxSQLite3ResultSet & rs)
{
	m_comment = rs.GetString(0);
	m_file = rs.GetString(1);
	m_line = rs.GetInt(2);
}

Comment::Comment(const Comment& rhs)
{
	*this = rhs;
}

Comment::Comment(const wxString & comment, const wxString & file, const int line)
: m_comment( comment )
, m_file( file )
, m_line( line )
{
	m_comment.erase(m_comment.find_last_not_of(_T("\n\r\t"))+1);
}

Comment & Comment::operator=(const Comment &rhs)
{
	if( this == &rhs )
		return *this;

	m_comment = rhs.m_comment;
	m_file = rhs.m_file;
	m_line = rhs.m_line;
	return *this;
}

int Comment::Store(wxSQLite3Statement& insertPreparedStmnt, TagsDatabase* db)
{
	try
	{
		insertPreparedStmnt.Bind(1, GetComment());
		insertPreparedStmnt.Bind(2, GetFile());
		insertPreparedStmnt.Bind(3, GetLine());
		insertPreparedStmnt.ExecuteUpdate();
		insertPreparedStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		if(exc.ErrorCodeAsString(exc.GetErrorCode()) == _T("SQLITE_CONSTRAINT"))
			return TagExist;
		std::cerr << "Error: " << exc.GetMessage() << std::endl;
		return TagError;
	}
	return TagOk;
}

int Comment::Update(wxSQLite3Statement& updatePreparedStmnt)
{
	try
	{
		updatePreparedStmnt.Bind(1, GetComment());
		updatePreparedStmnt.Bind(2, GetFile());
		updatePreparedStmnt.Bind(3, GetLine());
		updatePreparedStmnt.ExecuteUpdate();
		updatePreparedStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		std::cerr << "Error: " << exc.GetMessage() << std::endl;
		return TagError;
	}
	return TagOk;
}

int Comment::Delete(wxSQLite3Statement& deletePreparedStmnt)
{
	try
	{
		deletePreparedStmnt.Bind(1, GetFile());
		deletePreparedStmnt.Bind(2, GetLine());
		deletePreparedStmnt.ExecuteUpdate();
		deletePreparedStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		std::cerr << "Error: " << exc.GetMessage() << std::endl;
		return TagError;
	}
	return TagOk;
}

wxString Comment::GetDeleteOneStatement()
{
	return _T("DELETE FROM COMMENTS WHERE File=? AND Line=?");
}

wxString Comment::GetUpdateOneStatement()
{
	return _T("UPDATE COMMENTS SET Comment=? WHERE File=? AND Line=?");
}

wxString Comment::GetInsertOneStatement()
{
	return _T("INSERT INTO COMMENTS VALUES (?, ?, ?)");	
}
