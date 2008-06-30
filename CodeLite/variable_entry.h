//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : variable_entry.h              
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
 #ifndef VARIABLE_ENTRY_H
#define VARIABLE_ENTRY_H

#include "wx/string.h"
#include "db_record.h"
#include "smart_ptr.h"

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else
#    define WXDLLIMPEXP_CL
#endif
class TagsDatabase;

class WXDLLIMPEXP_CL VariableEntry : public DbRecord
{
	wxString m_name;
	wxString m_value;
public:
	/**
	 * Default constructor.
	 */
	VariableEntry();	

	/**
	 * Default constructor.
	 */
	VariableEntry(const wxString &name, const wxString &path);	

	/**
	 * Copy constructor.
	 */
	VariableEntry(const VariableEntry& rhs);
	
	/**
	 * Construct a tag entry from db record.
	 * \param rs Result set
	 */
	VariableEntry(wxSQLite3ResultSet& rs);

	/**
	 * \param rhs Source to copy from (right hand side)
	 * \return this
	 */
	VariableEntry& operator=(const VariableEntry& rhs);

	/**
	 * Compare two VariableEntry objects.
	 * \param rhs Right hand side
	 * \return true if identical, false otherwise
	 */
	bool operator==(const VariableEntry& rhs);

	/**
	 *	Destructor
	 */
	virtual ~VariableEntry();


	const wxString& GetName() const { return m_name;}
	void SetName(const wxString& name) { m_name = name; }
	
	const wxString &GetValue() const{return m_value;}
	void SetValue(const wxString &value){m_value = value;}

	//------------------------------------------
	// Database operations
	//------------------------------------------
	/**
	 * Save this record into db.
	 * \param insertPreparedStmnt Prepared statement for insert operation
	 * \return TagOk, TagExist, TagError
	 */
	virtual int Store(wxSQLite3Statement& insertPreparedStmnt, TagsDatabase *db);

	/**
	 * Update this record into db.
	 * \param insertPreparedStmnt Prepared statement for insert operation
	 * \return TagOk, TagError
	 */
	virtual int Update(wxSQLite3Statement& updatePreparedStmnt);

	/**
	 * Delete this record from db.
	 * \param deletePreparedStmnt Prepared statement for delete operation
	 * \return TagOk, TagError
	 */
	virtual int Delete(wxSQLite3Statement& deletePreparedStmnt);
	
	/**
	 * \return delete preapred statement
	 */
	virtual wxString GetDeleteOneStatement();

	/**
	 * \return update preapred statement
	 */
	virtual wxString GetUpdateOneStatement();

	/**
	 * \return insert preapred statement
	 */
	virtual wxString GetInsertOneStatement();
};
typedef SmartPtr<VariableEntry> VariableEntryPtr;
#endif // VARIABLE_ENTRY_H


