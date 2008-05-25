//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : cppwordscanner.h              
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
