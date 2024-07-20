//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : cscopeentrydata.h              
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
 #ifndef __cscopeentrydata__
#define __cscopeentrydata__

#include <wx/string.h>

enum {
	KindFileNode = 0,
	KindSingleEntry
};

class CscopeEntryData
{
	wxString m_file;
	int m_line;
	wxString m_pattern;
	wxString m_scope;
	int m_kind;

public:
	CscopeEntryData();
	~CscopeEntryData();


//Setters
	void SetFile(const wxString& file) {
		this->m_file = file.c_str();
	}
	void SetKind(const int& kind) {
		this->m_kind = kind;
	}
	void SetLine(const int& line) {
		this->m_line = line;
	}
	void SetPattern(const wxString& pattern) {
		this->m_pattern = pattern.c_str();
	}
	void SetScope(const wxString& scope) {
		this->m_scope = scope.c_str();
	}
//Getters
	const wxString& GetFile() const {
		return m_file;
	}
	const int& GetKind() const {
		return m_kind;
	}
	const int& GetLine() const {
		return m_line;
	}
	const wxString& GetPattern() const {
		return m_pattern;
	}
	const wxString& GetScope() const {
		return m_scope;
	}

};
#endif // __cscopeentrydata__
