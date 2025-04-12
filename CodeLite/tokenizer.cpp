//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : tokenizer.cpp              
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
 // StringTokenizer.cpp: implementation of the StringTokenizer class.
//
//////////////////////////////////////////////////////////////////////
#include "precompiled_header.h"
#include "tokenizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StringTokenizer::StringTokenizer(const wxString& str,
								 const wxString& strDelimiter,
								 const bool &bAllowEmptyTokens /* false */)
{
	Initialize();

	int nEnd = (int)str.find(strDelimiter, 0);
	int nStart = 0;
	wxString token;
	while( nEnd != -1 )
	{
		if( nEnd != nStart)
			token = str.substr(nStart, nEnd-nStart);
		else
			token.Empty();

		if(!token.empty())
			m_tokensArr.push_back(token);
		else if( bAllowEmptyTokens )
			m_tokensArr.push_back(token);

		// next token
		nStart = nEnd + (int)strDelimiter.length();
		nEnd = (int)str.find(strDelimiter, nStart );
	}
	if( nStart != (int)str.length() )
	{
		//We have another token which is not delimited 
		wxString token = str.substr(nStart);
		m_tokensArr.push_back(token);
	}
}

StringTokenizer::StringTokenizer(const wxString& str, const wxArrayString& delimiterArr, const bool &allowEmptyTokens)
{
	Initialize();
	wxString tmpStr( str );

	// Replace all delimiters to the first one
	if(delimiterArr.GetCount() >= 2)
	{
		size_t i=1;
		for(; i<delimiterArr.GetCount(); i++)
			tmpStr.Replace(delimiterArr[i], delimiterArr[0]);
	}
	*this = StringTokenizer(tmpStr, delimiterArr[0], allowEmptyTokens);
}

// Default
StringTokenizer::StringTokenizer()
{
	Initialize();
}

// Copy constructor
StringTokenizer::StringTokenizer(const StringTokenizer &src)
{
	*this = src;
}

StringTokenizer& StringTokenizer::operator =(const StringTokenizer &src)
{
	if( &src == this)
		return *this;
	Initialize();

	// Copy the tokens
	m_tokensArr.clear();
	for( int i=0; i<(int)src.m_tokensArr.size() ; i++)
		m_tokensArr.push_back(src.m_tokensArr[i]);
	return *this;
}

StringTokenizer::~StringTokenizer()
{
	m_tokensArr.clear();
}

const int StringTokenizer::Count() const
{
	return (int)m_tokensArr.size();
}

void StringTokenizer::Initialize()
{
	m_tokensArr.clear();
}

wxString StringTokenizer::operator[](const int nIndex)
{
	if(m_tokensArr.size() == 0)
		return wxEmptyString;
	if( nIndex>=(int)m_tokensArr.size() || nIndex<0)
		return wxEmptyString;
	return m_tokensArr[nIndex];
}


