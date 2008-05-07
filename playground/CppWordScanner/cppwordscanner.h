#ifndef __cppwordscanner__
#define __cppwordscanner__

#include <wx/arrstr.h>
#include "cpptoken.h"

class CppWordScanner {
	wxString m_text;
	wxSortedArrayString m_arr;
	
protected:
	void doFind(const wxString &filter, CppTokenList &l);
	
public:
	CppWordScanner(const wxString &text);
	~CppWordScanner();
	
	void findAll(CppTokenList &l);
	void match(const wxString &word, CppTokenList &l);

};
#endif // __cppwordscanner__
