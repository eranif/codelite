#ifndef __cppwordscanner__
#define __cppwordscanner__

#include <wx/arrstr.h>
#include "cpptoken.h"

//class TokenDb;

class CppWordScanner {
	wxString m_text;
	wxSortedArrayString m_arr;
//	TokenDb *m_db;
	wxString m_filename;
protected:
	void doFind(const wxString &filter, CppTokensMap &l);
	
public:
	CppWordScanner(const wxString &file_name);
	~CppWordScanner();
	
	void FindAll(CppTokensMap &l);
	void Match(const wxString &word, CppTokensMap &l);
//	void SetDatabase(TokenDb *db);
};
#endif // __cppwordscanner__
