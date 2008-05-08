#ifndef __tokendb__
#define __tokendb__

#include "cpptoken.h"
#include <wx/string.h>

class TokenDb {
	
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
	
public:
	TokenDb();
	virtual ~TokenDb();

};
#endif // __tokendb__
