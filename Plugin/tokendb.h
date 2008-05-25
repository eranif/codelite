//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : tokendb.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #ifndef __tokendb__
#define __tokendb__

#if 0
#include "cpptoken.h"
#include <wx/string.h>

class wxSQLite3Database;

class TokenDb {
	wxSQLite3Database *m_db;
	
public:
	/**
	 * \brief open database file. this function also creates the schema if needed
	 * \param file_name file to open
	 */
	void Open(const wxString &file_name);
	
	/**
	 * \brief start a database transaction
	 */
	void BeginTransaction();
	
	/**
	 * \brief commit a previosly opened transaction
	 */
	void Commit();
	
	/**
	 * @brief rollback last transaction
	 */
	void Rollback();
	
	/**
	 * \brief store CppToken into the database
	 * \param token
	 */
	void Store(const CppToken& token);
	
	/**
	 * \brief load cpp tokens from the database by name
	 * \param name name to search
	 * \param l [output] the result
	 */
	void Fetch(const wxString& name, CppTokensMap &l);
	
	/**
	 * \brief delete all entries by file name
	 * \param file
	 */
	void DeleteByFile(const wxString  &file);
	
	/**
	 * @brief drop the existing tables (if any) and recreate the sechema
	 */
	void RecreateSchema();
	
public:
	TokenDb();
	virtual ~TokenDb();

};
#endif 

#endif // __tokendb__
