//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : tokenizer.h              
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
 // StringTokenizer.h: interface for the StringTokenizer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CODELITE_TOKENIZER_H
#define CODELITE_TOKENIZER_H

#include <wx/string.h>
#include <vector>
#include <wx/arrstr.h>
#include "codelite_exports.h"

/**
 * StringTokenizer helps you to break a string up into a number of tokens.
 *
 * Usage example:
 *
 * \code
 * StringTokenizer tok(_T("first second third"), _T(" "));
 * for(int i=0; i<tok.GetCount(); i++)
 * {
 *		wxString token = tok[i];
 * }
 * \endcode
 *
 * StringTokenizer also allows you to break string with number of delimiters:
 *
 * \code
 * wxArrayString delimArr;
 * delimArr.Add("_T(" "));
 * delimArr.Add("_T("-"));
 * delimArr.Add("_T(":"));
 * StringTokenizer tok(_T("first-second:third"), delimArr);
 * \endcode 
 * 
 * The above example will result with: first, second and third.
 *
 * \ingroup CodeLite
 * \version 1.0
 * first version
 *
 * \date 09-02-2006
 * \author Eran
 */
class WXDLLIMPEXP_CL StringTokenizer
{
	std::vector<wxString> m_tokensArr;

public:
	/**
	 * Construct a tokenizer with given input string and delimiter
	 * \param str source string
	 * \param delimiter delimiter to use
	 * \param bAllowEmptyTokens if set to true, empty tokens will be returned as well. Default is no empty tokens
	 */
	StringTokenizer(const wxString& str, const wxString& delimiter = _T(" "), bool bAllowEmptyTokens = false);

	/**
	 * Construct a tokenizer with given input string and array of delimiters
	 * \param str source string
	 * \param delimiterArr array of delimiters
	 * \param bAllowEmptyTokens if set to true, empty tokens will be returned as well. Default is no empty tokens
	 */
	StringTokenizer(const wxString& str,const wxArrayString& delimiterArr, bool bAllowEmptyTokens = false);
	/**
	 * Copy constructor
	 * \param src source tokenizer
	 */
	StringTokenizer(const StringTokenizer&) = default;

	/**
	 * Default constructor
	 */
	StringTokenizer() = default;

	/**
	 * Destructor
	 */
	virtual ~StringTokenizer() = default;

	//-----------------------------------------------------
	// Operations
	//-----------------------------------------------------

	/**
	 * Get the number of tokens
	 * \return number of tokens
	 */
	int Count() const;
	/**
	 * Random access operator, starting from zero
	 * \param nIndex token index
	 * \return token at nIndex (copy of it)
	 */
	wxString operator[](int nIndex);

	/**
	 * Copy one tokenizer to another
	 * \param src source tokenizer to copy from
	 * \return this
	 */
	StringTokenizer& operator=(const StringTokenizer&) = default;
};

#endif // CODELITE_TOKENIZER_H
