//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : language.h
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
 #ifndef CODELITE_LANGUAGE_H
#define CODELITE_LANGUAGE_H

#include "tokenizer.h"
#include "cpp_scanner.h"
#include "y.tab.h"
#include "singleton.h"
#include "entry.h"
#include <wx/filename.h>
#include "db_record.h"
#include "expression_result.h"
#include "variable.h"
#include "function.h"

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else
#    define WXDLLIMPEXP_CL
#endif

enum SearchFlags
{
	PartialMatch = 1,
	ExactMatch = 2,
	IgnoreCaseSensitive = 4
};

class TagsManager;
/**
 * Language, a helper class that parses (currently) C/C++ code.
 * It conatains all the parsing of small fragments of code, in places where
 * ctags chosose to ignore (for example, ctags provides us with a member name,
 * but it does not provide us with its qualifier).
 *
 * Whenever possible, it is advised to use TagsManager API instead of Language API.
 *
 * \ingroup CodeLite
 * \date 09-02-2006
 * \author Eran
 */
class WXDLLIMPEXP_CL Language
{
	friend class Singleton<Language>;

public:
	/**
	 * Return the visible scope of syntax starting at depth zero.
	 *
	 * For example, given the following srcString:
	 * \code
	 * int main()
	 * {
	 *		{
	 *			int i =0;
	 *		}
	 * }
	 * \endcode
	 *
	 * the output will be:
	 * \code
	 * int main()
	 * {
	 *   ;
	 * }
	 * \code
	 *
	 * \note The scopes with depth greater then 0 will be removed.
	 * This function is useful for searching for local members.
	 * \param srcString Input string
	 * \return visible scope
	 */
	wxString GetScope(const wxString& srcString);

	/**
	 * Set the language specific auto completion delimeteres, for example: for C++ you should populate
	 * the array with { . , -> , :: }
	 * \param delimArr delimeter array
	 */
	void SetAutoCompDeliemters(const std::vector<wxString>& delimArr);

	/**
	 * \brief set the tags manager to be used by this language instance
	 * \param tm
	 */
	void SetTagsManager(TagsManager *tm);

	/**
	 * \brief return the available tags manager
	 * \return
	 */
	TagsManager *GetTagsManager();

	/**
	 * Parse comments from source file
	 * \param name file name
	 * \param comments [output] returned vector of comments
	 */
	void ParseComments(const wxFileName &fileName, std::vector<DbRecordPtr>* comments);

	//==========================================================
	// New API based on the yacc grammar files
	//==========================================================

	/**
	 * Evaluate a C++ expression. for example, the following expression: '((Notebook*)book)->'
	 * will be processed into typeName=Notebook, and typeScope=<global> (assuming Notebook is not
	 * placed within any namespace)
	 * \param stmt c++ expression
	 * \param text text where this expression was found
	 * \param fn filename context
	 * \param lineno current line number
	 * \param typeName [output]
	 * \param typeScope [output]
	 * \param oper [output] return the operator used (::, ., ->)
	 * \param scopeTemplateInitList [output] return the scope tempalte intialization (e.g. "std::auto_ptr<wxString> str;" -> <wxString>
	 * \return true on success, false otherwise. The output fields are only to be checked with the return
	 * valus is 'true'
	 */
	bool ProcessExpression(const wxString& stmt, const wxString& text, const wxFileName &fn, int lineno, wxString &typeName, wxString &typeScope, wxString &oper, wxString &scopeTemplateInitList);

	/**
	 * return scope name from given input string
	 * \param in input string
	 * \return scope name or empty string
	 */
	wxString GetScopeName(const wxString &in, std::vector<wxString> *additionlNS);

	/**
	 * parse an expression and return the result. this functions uses
	 * the sqlite database as its symbol table
	 * \param in input string expression
	 * \return ExpressionResult, if it fails to parse it, check result.m_name.empty() for success
	 */
	ExpressionResult ParseExpression(const wxString &in);

	/**
	 * get the name's type, using local scope when possible. if parsing of local scope fails to find
	 * a match, try the symbol database
	 * \param name name to search for
	 * \param text the scope
	 * \param scopeName the scope name
	 * \param firstToken set to true if the 'name' is the first token in the chain (myClass.some_func(). --> when testing for some_func(), firstToken = false
	 * and when testing myClass. --> first token is set to true
	 * \param type [output] name's type, incase 'name' is matched to a function, type will contain the type of the reutrn value of the function
	 * \param typeScope [output] type's scope
	 * \return true on success false otherwise
	 */
	bool TypeFromName(	const wxString &name,
						const wxString &text,
						const wxString &extraScope,
						const wxString &scopeName,
						const std::vector<wxString> &moreScopes,
						bool firstToken,
						wxString &type,
						wxString &typeScope);

	/**
	 * Collect local variables from given scope text (in) and an optional symbol name
	 * \param in scope to search for
	 * \param tags output, since we dont have full information about each token, all local variables returned are of type
	 *			   'variable' with public access
	 * \param name optional name to look for (name can be partial).
	 */
	void GetLocalVariables(const wxString &in, std::vector<TagEntryPtr> &tags, const wxString &name = wxEmptyString, size_t flag = PartialMatch);


	bool VariableFromPattern(const wxString &pattern, const wxString &name, Variable &var);
	bool FunctionFromPattern(const wxString &pattern, clFunction &foo);

	bool ResolveTempalte(wxString &typeName, wxString &typeScope, const wxString &parentPath, const wxString &parenttempalteInitList);
private:
	bool DoSearchByNameAndScope(const wxString &name,
								const wxString &scopeName,
								std::vector<TagEntryPtr> &tags,
								wxString &type,
								wxString &typeScope);
	bool CorrectUsingNamespace(wxString &type, wxString &typeScope, const std::vector<wxString> &moreScopes, const wxString &parentScope, std::vector<TagEntryPtr>& tags);
	/**
	 * Private constructor
	 */
	Language();

	/**
	 * Private destructor
	 */
	virtual ~Language();

	/**
	 * Return the next token and the delimiter found, the source string is taken from the
	 * m_tokenScanner member of this class
	 * \param token next token
	 * \param delim delimiter found
	 * \return true if token was found false otherwise
	 */
	bool NextToken(wxString &token, wxString &delim);

	/**
	 * \brief Attempt to fix template results
	 * \param typeName the type name that was detected by the parser
	 * \param typeScope the type scope
	 */
	bool OnTemplates(wxString &typeName, wxString &typeScope, Variable &parent);

	/**
	 * \brief attempt to expand 'typedef' to their actual value
	 */
	bool OnTypedef(wxString &typeName, wxString &typeScope, wxString &templateInitList, const wxString &optionalScope, wxString &scopeTempalteInitList);

	/**
	 * \brief expand reference operator (->) overloading
	 * \param typeName [input/output] the type name to check for the reference operator overloading
	 * \param typeScope [input/output] its scope
	 * \return true on success, false otherwise
	 */
	bool OnArrowOperatorOverloading(wxString &typeName, wxString &typeScope);

	void ParseTemplateArgs(CppScanner *scanner, wxArrayString &argsList);
	void ParseTemplateInitList(CppScanner *scanner, wxArrayString &argsList);
	void DoRemoveTempalteInitialization(wxString &str, wxString &tmplInitList);
private:
	std::map<char, char>    m_braces;
	std::vector<wxString>	m_delimArr;
	wxString m_expression;
	CppScannerPtr m_scanner;
	CppScannerPtr m_tokenScanner;
	Variable m_parentVar;
	TagsManager *m_tm;
};

typedef Singleton<Language> LanguageST;

#endif // CODELITE_LANGUAGE_H
