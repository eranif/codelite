//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : stringsearcher.h              
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
 #ifndef __stringsearcher__
#define __stringsearcher__

#include <wx/string.h>

// Possible search data options:
enum {
	wxSD_MATCHCASE         = 1,
	wxSD_MATCHWHOLEWORD    = 2,
	wxSD_REGULAREXPRESSION = 4,
	wxSD_SEARCH_BACKWARD   = 8
};

class StringFindReplacer {

protected:
	static wxString GetString(const wxString& input, int from, bool search_up);
	static bool DoRESearch(const wxString &input, int startOffset, const wxString &find_what, size_t flags, int &pos, int &matchLen);
	static bool DoSimpleSearch(const wxString &input, int startOffset, const wxString &find_what, size_t flags, int &pos, int &matchLen);
	
public:
	static bool Search(const wxString &input, int startOffset, const wxString &find_what, size_t flags, int &pos, int &matchLen);
};
#endif // __stringsearcher__
