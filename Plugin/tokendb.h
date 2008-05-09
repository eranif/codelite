#ifndef __tokendb__
#define __tokendb__

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
	void Fetch(const wxString& name, CppTokenList &l);
	
	/**
	 * \brief delete all entries by file name
	 * \param file
	 */
	void DeleteByFile(const wxString  &file);
	
public:
	TokenDb();
	virtual ~TokenDb();

};
#endif // __tokendb__
