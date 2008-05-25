//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : gdbinfolexer.cpp              
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
 #include "gdbinfolexer.h"

GdbInfoLexer::GdbInfoLexer(const wxString &input)
: m_input(input)
{
	m_input = m_input.Trim();
	m_input = m_input.Trim(false);
}

GdbInfoLexer::~GdbInfoLexer()
{
}

int GdbInfoLexer::Lex(wxString &word)
{
	word.Empty();
	if(m_input.IsEmpty()){
		return GdbEof;
	}

	while(m_input.Length() > 0){
		//Skip whitespaces
		wxChar ch = m_input.GetChar(0);
		m_input = m_input.Remove(0, 1);

		switch(ch){
			case wxT('{'):
				word = word.Trim();
				if(word.IsEmpty() == false){
					//put back this char into the input string
					m_input.Prepend(wxT("{"));
					return GdbWord;
				}else{
					word = ch;
					return GdbOpenBrace;
				}
			case wxT('}'):
				word = word.Trim();
				if(word.IsEmpty() == false){
					//put back this char into the input string
					m_input.Prepend(wxT("}"));
					return GdbWord;
				}else{
					word = ch;
					return GdbCloseBrace;
				}
			case wxT('='):
				word = word.Trim();
				if(word.IsEmpty() == false){
					//put back this char into the input string
					m_input.Prepend(wxT("="));
					return GdbWord;
				}else{
					word = ch;
					return GdbEqualSign;
				}
			case wxT(','):
				word = word.Trim();
				if(word.IsEmpty() == false){
					//put back this char into the input string
					m_input.Prepend(wxT(","));
					return GdbWord;
				}else{
					word = ch;
					return GdbComma;
				}
			case wxT('\n'):
			case wxT('\t'):
			case wxT('\v'):
			case wxT('\r'):
				//whitespaces
				break;
			case wxT(' '):
				word = word.Trim();
				if(word.IsEmpty() == false){
					return GdbWord;
				}
				break;
			default:
				word.Append(ch);
				break;
		}
	}

	word = word.Trim();
	if(word.IsEmpty() == false){
		return GdbWord;
	}
	return GdbEof;
}
