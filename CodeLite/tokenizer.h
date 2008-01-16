// StringTokenizer.h: interface for the StringTokenizer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CODELITE_TOKENIZER_H
#define CODELITE_TOKENIZER_H

#include <wx/string.h>
#include <vector>
#include <wx/arrstr.h>

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif

/**
 * StringTokenizer helps you to break a string up into a number of tokens. 
 * It replaces the standard C function strtok() and also extends it in a number of ways.
 *
 * Usage example:
 *
 * \code
 * StringTokenizer tok(_T("first second third"), _T(" "));
 * while( tok.HasMore() )
 * {
 *		wxString token = tok.Next();
 * }
 * \endcode
 * 
 * An alternate way to iterate over the tokenizer:
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
 * wxArrayString dlimArr;
 * delimArr.Add("_T(" "));
 * delimArr.Add("_T("-"));
 * delimArr.Add("_T(":"));
 * StringTokenizer tok(_T("first-second:third"), delimArr);
 * \endcode 
 * 
 * The above exmple will result with: first, second and third.
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
	int m_nCurr;

public:
	/**
	 * Construct a tokenizer with given input string and delimiter
	 * \param str source string
	 * \param delimiter delimiter to use
	 * \param bAllowEmptyTokens if set to true, empty tokens will be returned as well. Default is no empty tokens
	 */
	StringTokenizer(const wxString& str,const wxString& delimiter = _T(" ") , const bool &bAllowEmptyTokens = false);

	/**
	 * Construct a tokenizer with given input string and array of delimiters
	 * \param str source string
	 * \param delimiterArr array of delimiters
	 * \param bAllowEmptyTokens if set to true, empty tokens will be returned as well. Default is no empty tokens
	 */
	StringTokenizer(const wxString& str,const wxArrayString& delimiterArr, const bool &bAllowEmptyTokens = false);
	/**
	 * Copy constructor
	 * \param src source tokenizer
	 */
	StringTokenizer(const StringTokenizer& src);

	/**
	 * Default constructor
	 */
	StringTokenizer();		//Default

	/**
	 * Destructor
	 */
	virtual ~StringTokenizer();

	//-----------------------------------------------------
	// Operations
	//-----------------------------------------------------

	/**
	 * Return the last token 
	 * \return last token
	 */
	wxString Last();
	/**
	 * Get the number of tokens
	 * \return number of tokens
	 */
	const int Count() const;
	/**
	 * Return the current token without progressing the internal pointer
	 * \return current token
	 */
	wxString Current();

	/**
	 * Return the first token and progress the internal pointer
	 * \return First token
	 */
	wxString First();
	/**
	 * Do we have more tokens?
	 * \return true if the next call to Next() will return a token
	 */
	bool HasMore();
	/** 
	 * Return the previous string, this function moves the internal pointer backward.
	 * So, subsequent calls to this function, will eventually return us to the start of the tokenizer
	 * \return previous token of empty string if we are already at start
	 */
	wxString Previous();
	/**
	 * Return next token, usually combined with HasMore() function. This function progress the 
	 * internal pointer to next token
	 * \return next token
	 */
	wxString Next();
	/**
	 * Random acess operator, statring from zero
	 * \param nIndex token index
	 * \return token at nIndex (copy of it)
	 */
	wxString operator[](const int nIndex);

	/**
	 * Copy one tokenizer to another
	 * \param src source tokenizer to copy from
	 * \return this
	 */
	StringTokenizer& operator=(const StringTokenizer& src);

private:
	// Functions
	/**
	 * Initialize the tokenizer
	 */
	void Initialize();
};

#endif // CODELITE_TOKENIZER_H
