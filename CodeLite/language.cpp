//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : language.cpp
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
#include "precompiled_header.h"

#include "language.h"
#include "variable.h"
#include "function.h"
#include "ctags_manager.h"
#include "y.tab.h"
#include <wx/stopwatch.h>
#include <wx/ffile.h>
#include "map"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif

//===============================================================
//defined in generated files from the yacc grammar:
//cpp_scope_garmmar.y
//cpp_variables_grammar.y
//expr_garmmar.y

extern std::string get_scope_name(const std::string &in, std::vector<std::string> &additionlNS, const std::map<std::string, std::string> &ignoreTokens);
extern ExpressionResult &parse_expression(const std::string &in);
extern void get_variables(const std::string &in, VariableList &li, const std::map<std::string, std::string> &ignoreTokens);
extern void get_functions(const std::string &in, FunctionList &li, const std::map<std::string, std::string> &ignoreTokens);

//===============================================================

Language::Language()
		: m_expression(wxEmptyString)
		, m_scanner(new CppScanner())
		, m_tokenScanner(new CppScanner())
		, m_tm(NULL)
{
	// Initialise the braces map
	m_braces['<'] = '>';
	m_braces['('] = ')';
	m_braces['['] = ']';
	m_braces['{'] = '}';

	// C++ / C auto complete delimiters for tokens
	std::vector<wxString> delimArr;
	delimArr.push_back(_T("::"));
	delimArr.push_back(_T("->"));
	delimArr.push_back(_T("."));
	SetAutoCompDeliemters(delimArr);
}

/// Destructor
Language::~Language()
{
}

/// Return the visible scope until pchStopWord is encountered
wxString Language::GetScope(const wxString& srcString)
{
	wxArrayString scope_stack;
	wxString currScope;
	int type;

	// Initialize the scanner with the string to search
	const wxCharBuffer scannerText =  _C(srcString);
	m_scanner->SetText( scannerText.data());
	bool changedLine = false;
	bool prepLine = false;
	int curline = 0;
	while (true) {
		type = m_scanner->yylex();


		// Eof ?
		if (type == 0) {
			if (!currScope.empty())
				scope_stack.push_back(currScope);
			break;
		}

		// eat up all tokens until next line
		if ( prepLine && m_scanner->lineno() == curline) {
			wxString word = _U(m_scanner->YYText());
			currScope += _T(" ");
			currScope += word;
			continue;
		}

		prepLine = false;

		// Get the current line number, it will help us detect preprocessor lines
		changedLine = (m_scanner->lineno() > curline);
		if (changedLine) {
			currScope << wxT("\n");
		}

		curline = m_scanner->lineno();

		// For debug purposes
		wxString word = _U(m_scanner->YYText());
		switch (type) {
		case (int)'(':
						currScope += _T("\n");
			scope_stack.push_back(currScope);
			currScope = wxT("(\n");
			break;
		case (int)'{':
						currScope += _T("\n");
			scope_stack.push_back(currScope);
			currScope = _T("{\n");
			break;
		case (int)')':
						// Discard the current scope since it is completed
						if ( !scope_stack.empty() ) {
					currScope = scope_stack.back();
					scope_stack.pop_back();
					currScope += _T("()");
				} else
					currScope = wxEmptyString;
			break;
		case (int)'}':
						// Discard the current scope since it is completed
						if ( !scope_stack.empty() ) {
					currScope = scope_stack.back();
					scope_stack.pop_back();
					currScope += _T("\n{}\n");
				} else {
					currScope = wxEmptyString;
				}
			break;
		case (int)'#':
						if (changedLine) {
					// We are at the start of a new line
					// consume everything until new line is found or end of text
					currScope += _T(" ");
					currScope += _U(m_scanner->YYText());
					prepLine = true;
					break;
				}
		default:
			currScope += _T(" ");
			currScope += _U(m_scanner->YYText());
			break;
		}
	}

	m_scanner->Reset();

	if (scope_stack.empty())
		return srcString;

	currScope = wxEmptyString;
	size_t i = 0;
	for (; i < scope_stack.size(); i++)
		currScope += scope_stack[i];

	// if the current scope is not empty, terminate it with ';' and return
	if ( currScope.IsEmpty() == false ) {
		currScope += _T(";");
		return currScope;
	}

	return srcString;
}

bool Language::NextToken(wxString &token, wxString &delim)
{
	int type(0);
	int depth(0);
	while ( (type = m_tokenScanner->yylex()) != 0 ) {
		switch (type) {
		case CLCL:
		case wxT('.'):
					case lexARROW:
							if (depth == 0) {
						delim = _U(m_tokenScanner->YYText());
						return true;
					} else {
						token << wxT(" ") << _U(m_tokenScanner->YYText());
					}
			break;
		case wxT('<'):
					case wxT('['):
						case wxT('('):
							case wxT('{'):
									depth++;
			token << wxT(" ") << _U(m_tokenScanner->YYText());
			break;
		case wxT('>'):
					case wxT(']'):
						case wxT(')'):
							case wxT('}'):
									depth--;
			token << wxT(" ") << _U(m_tokenScanner->YYText());
			break;
		default:
			token << wxT(" ") << _U(m_tokenScanner->YYText());
			break;
		}
	}
	return false;
}

void Language::SetAutoCompDeliemters(const std::vector<wxString> &delimArr)
{
	m_delimArr = delimArr;
}

bool Language::ProcessExpression(const wxString& stmt,
                                 const wxString& text,
                                 const wxFileName &fn, int lineno,
                                 wxString &typeName, 	//output
                                 wxString &typeScope,	//output
                                 wxString &oper)		//output
{
	ExpressionResult result;
	wxString statement( stmt );
	bool evaluationSucceeded = true;

	// Trim whitespace from right and left
	static wxString trimString(_T("{};\r\n\t\v "));

	statement.erase(0, statement.find_first_not_of(trimString));
	statement.erase(statement.find_last_not_of(trimString)+1);
	wxString dbgStmnt = statement;

	// First token is handled sepratly
	wxString word;
	wxString op;
	wxString lastFuncSig;
	wxString accumulatedScope;

	std::vector<TagEntry> tags;
	wxString visibleScope = GetScope(text);
	std::vector<wxString> additionalScopes;
	wxString scopeName = GetScopeName(text, &additionalScopes);

	TagEntryPtr tag = GetTagsManager()->FunctionFromFileLine(fn, lineno);
	if (tag) {
		lastFuncSig = tag->GetSignature();
	}

	wxString parentTypeName, parentTypeScope;
	wxString grandParentTypeName, grandParentTypeScope;

	//get next token using the tokenscanner object
	m_tokenScanner->SetText(_C(statement));
	Variable parent;
	while (NextToken(word, op)) {
		oper = op;
		m_parentVar.Reset();
		wxString templateInitList;
		result = ParseExpression(word);
		word.clear();

		//parsing failed?
		if (result.m_name.empty()) {
			evaluationSucceeded = false;
			break;
		}

		//no tokens before this, what we need to do now, is find the TagEntry
		//that corrseponds to the result
		if (result.m_isaType) {
			//-------------------------------------------
			// Handle type (usually when casting is found
			//--------------------------------------------

			typeScope = result.m_scope.empty() ? wxT("<global>") : _U(result.m_scope.c_str());
			typeName = _U(result.m_name.c_str());
		} else if (result.m_isThis) {
			//-----------------------------------------
			// special handle for 'this' keyword
			//-----------------------------------------

			typeScope = result.m_scope.empty() ? wxT("<global>") : _U(result.m_scope.c_str());
			if (scopeName == wxT("<global>")) {
				evaluationSucceeded = false;
				break;
			}
			if (op == wxT("::")) {
				evaluationSucceeded = false;
				break;
			} // if(oper == wxT("::"))

			if (result.m_isPtr && op == wxT(".")) {
				evaluationSucceeded = false;
				break;
			}
			if (!result.m_isPtr && op == wxT("->")) {
				evaluationSucceeded = false;
				break;
			}
			typeName = scopeName;
		} else {
			//-------------------------------------------
			// found an identifier
			//--------------------------------------------
			wxString scopeToSearch(scopeName);
			if (parentTypeScope.IsEmpty() == false && parentTypeScope != wxT("<global>")) {
				scopeToSearch = parentTypeScope + wxT("::") + parentTypeName;
			} else if ((parentTypeScope.IsEmpty()|| parentTypeScope == wxT("<global>")) && !parentTypeName.IsEmpty()) {
				scopeToSearch = parentTypeName;
			}

			//--------------------------------------------------------------------------------------------
			//keep the scope that we searched so far. The accumumlated scope
			//are used for types, for scenarios like:
			//void Box::GetWidth()
			//{
			//	Rectangle::
			//
			//trying to process the above code, will yield searching Rectangle inside Box scope, since we are
			//inside Box's GetWidth() function.
			//the correct behavior shuold be searching for Rectangle in the global scope.
			//to correct this, we do special handling for Qualifier followed by coloon:colon operator (::)
			if (accumulatedScope.IsEmpty() == false) {
				if (accumulatedScope == wxT("<global>")) {
					accumulatedScope = scopeToSearch;
				} else {
					accumulatedScope << wxT("::");
					accumulatedScope << scopeToSearch;
				}
			} else {
				accumulatedScope << wxT("<global>");
			}

			if (op == wxT("::")) {
				//if the operator was something like 'Qualifier::', it is safe to assume
				//that the secope to be searched is the full expression
				scopeToSearch = accumulatedScope;
			}

			//get the derivation list of the typename
			bool res(false);
			res = TypeFromName(	_U(result.m_name.c_str()),
			                    visibleScope,
			                    lastFuncSig,
			                    scopeToSearch,
			                    additionalScopes,
			                    parentTypeName.IsEmpty(),
			                    typeName,	//output
			                    typeScope);	//output
			if (!res) {
				evaluationSucceeded = false;
				break;
			}

			//do typedef subsitute
			wxString tmp_name(typeName);
			while (OnTypedef(typeName, typeScope, templateInitList, scopeName)) {
				if (tmp_name == typeName) {
					//same type? break
					break;
				}
				tmp_name = typeName;
			}

			//do template subsitute
			if (OnTemplates(typeName, typeScope, parent)) {
				//do typedef subsitute
				wxString tmp_name(typeName);
				while (OnTypedef(typeName, typeScope, templateInitList, scopeName)) {
					if (tmp_name == typeName) {
						//same type? break
						break;
					}
					tmp_name = typeName;
				}
			}

			//try match any overloading operator to the typeName
			wxString tmpTypeName(typeName);
			if ( op == wxT("->") && OnArrowOperatorOverloading(typeName, typeScope) ) {

				//there is an operator overloading for ->
				//do the whole typedef/template subsitute again
				wxString tmp_name(typeName);
				while (OnTypedef(typeName, typeScope, templateInitList, scopeName)) {
					if (tmp_name == typeName) {
						//same type? break
						break;
					}
					tmp_name = typeName;
				}

				//When template is found, replace the typeName with the temporary type name
				//usually it will replace 'T' with the parent type, such as
				//'Singleton'
				if (templateInitList.IsEmpty() == false) {
					m_parentVar.m_isTemplate = true;
					m_parentVar.m_templateDecl = _C(templateInitList);
					m_parentVar.m_type = _C(tmpTypeName);
					m_parentVar.m_typeScope = _C(typeScope);
				}

				//do template subsitute
				if (OnTemplates(typeName, typeScope, m_parentVar)) {
					//do typedef subsitute
					wxString tmp_name(typeName);
					while (OnTypedef(typeName, typeScope, templateInitList, scopeName)) {
						if (tmp_name == typeName) {
							//same type? break
							break;
						}
						tmp_name = typeName;
					}
				}
			}
		}

		parent = m_parentVar;

		//Keep the information about this token for next iteration
		if (!parent.m_isTemplate && result.m_isTemplate) {

			parent.m_isTemplate = true;
			parent.m_templateDecl = result.m_templateInitList;
			parent.m_type = _C(typeName);
			parent.m_typeScope = _C(typeScope);

		} else if (templateInitList.IsEmpty() == false) {

			parent.m_isTemplate = true;
			parent.m_templateDecl = _C(templateInitList);
			parent.m_type = _C(typeName);
			parent.m_typeScope = _C(typeScope);
		}

		parentTypeName = typeName;
		parentTypeScope = typeScope;
	}
	return evaluationSucceeded;
}

bool Language::OnTemplates(wxString &typeName, wxString &typeScope, Variable &parent)
{
	bool res (false);
	//make sure that the type really exist
	TagsManager *tagsManager = GetTagsManager();
	if (!tagsManager->IsTypeAndScopeExists(typeName, typeScope)) {
		if (parent.m_isTemplate) {
			//we need to test the parent declaration line
			std::vector< TagEntryPtr > tagsVec;
			wxString parent_scope = _U(parent.m_typeScope.c_str());

			parent_scope.IsEmpty() ? parent_scope = wxT("<global>") : parent_scope;
			wxString parent_type  = _U(parent.m_type.c_str());
			tagsManager->TagsByScopeAndName(parent_scope, parent_type, tagsVec, ExactMatch);
			if (tagsVec.size() == 1) {
				//we got a match in the database
				TagEntryPtr tt = tagsVec.at(0);
				wxString pattern = tt->GetPattern();

				//extract the template declartion list
				CppScanner declScanner;
				declScanner.SetText( _C(pattern) );
				bool foundTemplate(false);
				int type (0);
				while ( !foundTemplate ) {
					type = declScanner.yylex();
					if ( type == 0 ) //eof
						break;

					switch (type) {
					case IDENTIFIER: {
						wxString word = _U(declScanner.YYText());
						if (word == wxT("template")) {
							foundTemplate = true;
							break;
						}
					}
					default:
						//do nothing
						break;
					}
				}

				wxArrayString templateDecl; // the names of the classes/typenames 'T'
				wxArrayString templateImpl; // the actual initialisation list

				CppScanner implScanner;
				implScanner.SetText(parent.m_templateDecl.c_str());
				ParseTemplateInitList(&implScanner, templateImpl);

				if ( foundTemplate ) {
					//we found our template declaration
					ParseTemplateArgs(&declScanner, templateDecl);
				}

				//we now got two lists, one contains the template name 'T' etc.
				//and the second is the actual types (initialisation)
				//we loop over the template list, and search for our real type
				for (size_t i=0; i< templateDecl.GetCount(); i++) {
					if (templateDecl.Item(i) == typeName) {
						if (templateImpl.GetCount() > i) {
							std::vector<TagEntryPtr> tags_vec;
							tagsManager->FindByPath(templateImpl.Item(i), tags_vec);
							//replace template arguments with actual values
							if (tags_vec.size() == 1) {
								TagEntryPtr t = tags_vec.at(0);
								typeName = t->GetName();
								typeScope = t->GetScope();
								res = true;
							}
							break;
						}
					}
				}
			}
		}
	}
	return res;
}

bool Language::OnTypedef(wxString &typeName, wxString &typeScope, wxString &templateInitList, const wxString &optionalScope)
{
	//if the match is typedef, try to replace it with the actual
	//typename
	bool res (false);
	TagsManager *tagsManager = GetTagsManager();
	std::vector<TagEntryPtr> tags, filtered_tags;
	wxString path;
	if (typeScope == wxT("<global>")) {
		path << typeName;
	} else {
		path << typeScope << wxT("::") << typeName;
	}

	tagsManager->FindByPath(path, tags);
	if (tags.empty()) {
		//try again using the additional scope
		if (optionalScope.IsEmpty() == false) {
			path.Clear();
			if (optionalScope == wxT("<global>")) {
				path << typeName;
			} else {
				path << optionalScope << wxT("::") << typeName;
			}
			tagsManager->FindByPath(path, tags);
		}
	}
	
	
	// try to remove all tags that are Macros from this list
	for(size_t i=0; i<tags.size(); i++){
		TagEntryPtr t = tags.at(i);
		if(t->GetKind() != wxT("macro")) {
			filtered_tags.push_back(t);
		}
	}
	
	if (filtered_tags.size() == 1) {
		//we have a single match, test to see if it a typedef
		TagEntryPtr tag = filtered_tags.at(0);
		wxString realName = tag->NameFromTyperef(templateInitList);
		if (realName.IsEmpty() == false) {
			typeName  = realName;
			typeScope = tag->GetScope();

			//incase the realName already includes the scope, remove it from the typename
			if (!typeScope.IsEmpty() && typeName.StartsWith(typeScope, &typeName)) {
				//remove any :: prefix
				typeName.StartsWith(wxT("::"), &typeName);
			}
			res = true;
		}
	}
	return res;
}

void Language::ParseTemplateArgs(CppScanner *scanner, wxArrayString &argsList)
{
	int type = scanner->yylex();
	wxString word = _U(scanner->YYText());

	//Eof?
	if (type == 0) {
		return;
	}
	if (type != (int)'<') {
		return;
	}

	bool nextIsArg(false);
	bool cont(true);
	while ( cont ) {
		type = scanner->yylex();
		if (type == 0) {
			break;
		}

		switch (type) {
		case lexCLASS:
		case IDENTIFIER: {
			wxString word = _U(scanner->YYText());
			if (word == wxT("class") || word == wxT("typename")) {
				nextIsArg = true;

			} else if (nextIsArg) {
				argsList.Add(word);
				nextIsArg = false;
			}
			break;
		}
		case (int)'>':
						cont = false;
			break;
		default:
			break;
		}
	}
}

void Language::ParseTemplateInitList(CppScanner *scanner, wxArrayString &argsList)
{
	int type = scanner->yylex();
	wxString word = _U(scanner->YYText());

	//Eof?
	if (type == 0) {
		return;
	}
	if (type != (int)'<') {
		return;
	}

	int depth(1);
	wxString typeName;
	while ( depth > 0 ) {
		type = scanner->yylex();
		if (type == 0) {
			break;
		}

		switch (type) {
		case (int)',': {
			if (depth == 1) {
				argsList.Add(typeName.Trim().Trim(false));
				typeName.Empty();
			}
			break;
		}
		case (int)'>':
						depth--;
			break;
		case (int)'<':
						depth++;
			break;
		case (int)'*':
					case (int)'&':
							//ignore pointers & references
							break;
		default:
			if (depth == 1) {
				typeName << _U(scanner->YYText());
			}
			break;
		}
	}

	if (typeName.Trim().Trim(false).IsEmpty() == false) {
		argsList.Add(typeName.Trim().Trim(false));
	}
	typeName.Empty();
}

void Language::ParseComments(const wxFileName &fileName, std::vector<DbRecordPtr> *comments)
{
	wxString content;
	try {
		wxFFile f(fileName.GetFullPath().GetData());
		if ( !f.IsOpened() )
			return;

		// read the content of the file and parse it
		f.ReadAll( &content );
		f.Close();
	} catch ( ... ) {
		return;
	}

	m_scanner->Reset();
	m_scanner->SetText( _C(content) );
	m_scanner->KeepComment( 1 );

	int type( 0 );

	wxString comment(_T(""));
	int line(-1);

	while ( true ) {
		type = m_scanner->yylex();
		if ( type == 0 ) //eof
			break;


		// we keep only comments
		if ( type == CPPComment ) {
			// incase the previous comment was one line above this one,
			// concatenate them to a single comment
			if ( m_scanner->lineno() - 1 == line ) {
				comment << m_scanner->GetComment();
				line = m_scanner->lineno();
				m_scanner->ClearComment();
				continue;
			}

			// first time or no comment is buffer
			if ( comment.IsEmpty() ) {
				comment = m_scanner->GetComment();
				line = m_scanner->lineno();
				m_scanner->ClearComment();
				continue;
			}

			if ( comment.IsEmpty() == false ) {
				comments->push_back( static_cast<DbRecord*>( new Comment( comment, fileName.GetFullPath(), line - 1)) );
				comment.Empty();
				line = -1;
			}

			comments->push_back( static_cast<DbRecord*>( new Comment( m_scanner->GetComment(), fileName.GetFullPath(), m_scanner->lineno()-1)) );
			m_scanner->ClearComment();
		} else if ( type == CComment ) {
			comments->push_back( static_cast<DbRecord*>( new Comment( m_scanner->GetComment(), fileName.GetFullPath(), m_scanner->lineno()) ) );
			m_scanner->ClearComment();
		}
	}

	if ( comment.IsEmpty() == false ) {
		comments->push_back( static_cast<DbRecord*>( new Comment( comment, fileName.GetFullPath(), line - 1) ) );
	}

	// reset the scanner
	m_scanner->KeepComment( 0 );
	m_scanner->Reset();
}

wxString Language::GetScopeName(const wxString &in, std::vector<wxString> *additionlNS)
{
	std::string lastFunc, lastFuncSig;
	std::vector<std::string> moreNS;
	FunctionList fooList;

	const wxCharBuffer buf = _C(in);

	TagsManager *mgr = GetTagsManager();
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetPreprocessorAsMap();
	
	std::string scope_name = get_scope_name(buf.data(), moreNS, ignoreTokens);
	wxString scope = _U(scope_name.c_str());
	if (scope.IsEmpty()) {
		scope = wxT("<global>");
	}
	if (additionlNS) {
		for (size_t i=0; i<moreNS.size(); i++) {
			additionlNS->push_back(_U(moreNS.at(i).c_str()));
		}
	}
	return scope;
}

ExpressionResult Language::ParseExpression(const wxString &in)
{
	const wxCharBuffer buf = _C(in);
	ExpressionResult result = parse_expression(buf.data());
	return result;
}

bool Language::TypeFromName(const wxString &name,
                            const wxString &text,
                            const wxString &extraScope,
                            const wxString &scopeName,
                            const std::vector<wxString> &moreScopes,
                            bool firstToken,
                            wxString &type,
                            wxString &typeScope)
{
	//try local scope
	VariableList li;
	FunctionList fooList;

	//first we try to match the current scope
	std::vector<TagEntryPtr> tags;

	TagsManager *mgr = GetTagsManager();
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetPreprocessorAsMap();
	
	if (!DoSearchByNameAndScope(name, scopeName, tags, type, typeScope)) {
		if (firstToken) {
			//can we test visible scope?
			const wxCharBuffer buf = _C(text);
			const wxCharBuffer buf2 = _C(extraScope);
			get_variables(buf.data(), li, ignoreTokens);
			get_variables(buf2.data(), li, ignoreTokens);

			//search for a full match in the returned list
			for (VariableList::iterator iter = li.begin(); iter != li.end(); iter++) {
				Variable var = (*iter);
				wxString var_name = _U(var.m_name.c_str());
				if (var_name == name) {
					type = _U(var.m_type.c_str());
					typeScope = var.m_typeScope.empty() ? wxT("<global>") : _U(var.m_typeScope.c_str());

					m_parentVar = var;

					//support for 'using namespace'
					if (typeScope == wxT("<global>")) {
						return CorrectUsingNamespace(type, typeScope, moreScopes, scopeName, tags);
					}
					return true;
				}
			}

			//failed to find it in the local scope
			//try the additional scopes
			for (size_t i=0; i<moreScopes.size(); i++) {
				tags.clear();
				if (DoSearchByNameAndScope(name, moreScopes.at(i), tags, type, typeScope)) {
					if (typeScope == wxT("<global>")) {
						return CorrectUsingNamespace(type, typeScope, moreScopes, scopeName, tags);
					}
					return true;
				}
			}
		}
		return false;
	} else {
		if (tags.size() > 0) {
			const wxCharBuffer buf = _C(tags.at(0)->GetPattern());
			get_variables(buf.data(), li, ignoreTokens);
			//search for a full match in the returned list
			for (VariableList::iterator iter = li.begin(); iter != li.end(); iter++) {
				Variable var = (*iter);
				wxString var_name = _U(var.m_name.c_str());
				if (var_name == name) {
					m_parentVar = var;
				}
			}
		} else {
			m_parentVar.m_name = _C(name);
			m_parentVar.m_type = _C(type);
			m_parentVar.m_typeScope = _C(typeScope);
		}
		return CorrectUsingNamespace(type, typeScope, moreScopes, scopeName, tags);
	}
}

bool Language::CorrectUsingNamespace(wxString &type, wxString &typeScope, const std::vector<wxString> &moreScopes, const wxString &parentScope, std::vector<TagEntryPtr> &tags)
{
	if (!GetTagsManager()->IsTypeAndScopeExists(type, typeScope)) {
		if (moreScopes.empty() == false) {
			//the type does not exist in the global scope,
			//try the additional scopes
			for (size_t i=0; i<moreScopes.size(); i++) {
				tags.clear();
				if (DoSearchByNameAndScope(type, moreScopes.at(i), tags, type, typeScope)) {
					return true;
				}
			}
		}

		//if we are here, it means that the more scopes did not matched any, try the parent scope
		tags.clear();
		if (DoSearchByNameAndScope(type, parentScope, tags, type, typeScope)) {
			return true;
		}

		//still no match?
		return true;
	}
	return true;
}

bool Language::DoSearchByNameAndScope(const wxString &name,
                                      const wxString &scopeName,
                                      std::vector<TagEntryPtr> &tags,
                                      wxString &type,
                                      wxString &typeScope)
{
	std::vector<TagEntryPtr> tmp_tags;
	GetTagsManager()->FindByNameAndScope(name, scopeName, tmp_tags);
	if( tmp_tags.empty() ){
		// try the global scope maybe?
		GetTagsManager()->FindByNameAndScope(name, wxT("<global>"), tmp_tags);
	}
	
	// filter macros from the result
	for(size_t i=0; i<tmp_tags.size(); i++){
		TagEntryPtr t = tmp_tags.at(i);
		if(t->GetKind() != wxT("macro")){
			tags.push_back(t);
		}
	}
	
	if (tags.size() == 1) {
		TagEntryPtr tag(tags.at(0));
		//we have a single match!
		if (tag->GetKind() == wxT("function") || tag->GetKind() == wxT("prototype")) {
			clFunction foo;
			if (FunctionFromPattern(tag->GetPattern(), foo)) {
				type = _U(foo.m_returnValue.m_type.c_str());
				typeScope = foo.m_returnValue.m_typeScope.empty() ? wxT("<global>") : _U(foo.m_returnValue.m_typeScope.c_str());
				return true;
			} // if(FunctionFromPattern(tag->GetPattern(), foo))
			return false;
		} // if(tag->GetKind() == wxT("function") || tag->GetKind() == wxT("prototype"))
		else if (tag->GetKind() == wxT("member") || tag->GetKind() == wxT("variable")) {
			Variable var;
			if (VariableFromPattern(tag->GetPattern(), tag->GetName(), var)) {
				type = _U(var.m_type.c_str());
				typeScope = var.m_typeScope.empty() ? wxT("<global>") : _U(var.m_typeScope.c_str());
				return true;
			}
			return false;
		} else {
			type = tag->GetName();
			typeScope = tag->GetScopeName();
		}
		return true;
	} else if (tags.size() > 1) {
		//if list contains more than one entry, check if all entries are of type 'function' or 'prototype'
		//(they can be mixed). If all entries are of one of these types, test their return value,
		//if all have the same return value, then we are ok
		clFunction foo;
		wxString tmpType, tmpTypeScope;
		bool allthesame(true);
		for (size_t i=0; i<tags.size(); i++) {
			TagEntryPtr tag(tags.at(i));
			wxString dbg_str = tag->GetPattern();
			if (!FunctionFromPattern(tag->GetPattern(), foo)) {
				allthesame = false;
				break;
			}

			tmpType = _U(foo.m_returnValue.m_type.c_str());
			tmpTypeScope = foo.m_returnValue.m_typeScope.empty() ? wxT("<global>") : _U(foo.m_returnValue.m_typeScope.c_str());
			if (i > 0 && (tmpType != type || tmpTypeScope != typeScope)) {
				allthesame = false;
				break;
			}
			type = tmpType;
			typeScope = tmpTypeScope;
		}

		if (allthesame && !tags.empty()) {
			return true;
		}
		wxString msg;
		msg << wxT("Too many matches for ") << name << wxT(", using scope: ") << scopeName;
//		wxLogMessage(msg);
		return false;
	}
	return false;
}

bool Language::VariableFromPattern(const wxString &in, const wxString &name, Variable &var)
{
	VariableList li;
	wxString pattern(in);
	//we need to extract the return value from the pattern
	pattern = pattern.BeforeLast(wxT('$'));
	pattern = pattern.AfterFirst(wxT('^'));

	const wxCharBuffer patbuf = _C(pattern);
	li.clear();

	TagsManager *mgr = GetTagsManager();
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetPreprocessorAsMap();
	
	get_variables(patbuf.data(), li, ignoreTokens);
	VariableList::iterator iter = li.begin();
	for(; iter != li.end(); iter++){
		Variable v = *iter;
		if(name == _U(v.m_name.c_str())) {
			var = (*iter);
			return true;
		}
	} // if(li.size() == 1)
	return false;
}

bool Language::FunctionFromPattern(const wxString &in, clFunction &foo)
{
	FunctionList fooList;
	wxString pattern(in);
	//we need to extract the return value from the pattern
	pattern = pattern.BeforeLast(wxT('$'));
	pattern = pattern.AfterFirst(wxT('^'));

	pattern = pattern.Trim();
	pattern = pattern.Trim(false);
	if (pattern.EndsWith(wxT(";"))) {
		pattern = pattern.RemoveLast();
	}

	//remove any comments from the pattern
	wxString tmp_pattern(pattern);
	pattern.Empty();
	GetTagsManager()->StripComments(tmp_pattern, pattern);

	//a limitiation of the function parser...
	pattern << wxT(';');

	TagsManager *mgr = GetTagsManager();
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetPreprocessorAsMap();
	
	const wxCharBuffer patbuf = _C(pattern);
	get_functions(patbuf.data(), fooList, ignoreTokens);
	if (fooList.size() == 1) {
		foo = (*fooList.begin());
		return true;
	} else if (fooList.size() == 0) {
		//fail to parse the statement, assume we got a broken pattern
		//(this can happen because ctags keeps only the first line of a function which was declared
		//over multiple lines)
		//do some work on the input buffer
		wxString pat2(pattern);

		if (pat2.EndsWith(wxT(";"))) {
			pat2 = pat2.RemoveLast();
		}
		if (pat2.EndsWith(wxT(","))) {
			pat2 = pat2.RemoveLast();
		}
		pat2 << wxT(");");
		const wxCharBuffer patbuf1 = _C(pat2);
		get_functions(patbuf1.data(), fooList, ignoreTokens);
		if (fooList.size() == 1) {
			foo = (*fooList.begin());
			return true;
		} else if (fooList.empty()) {
			//try a nasty hack:
			//the yacc cant find ctor declarations
			//so add a 'void ' infront of the function...
			wxString pat_tag(pattern);
			pat_tag = pat_tag.Trim(false).Trim();

			wxString pat3;

			// consider virtual methods as well
			bool virt(false);
			virt = pat_tag.StartsWith(wxT("virtual"), &pat3);
			if ( virt ) {
				pat3.Prepend(wxT("void "));
				pat3.Prepend(wxT("virtual "));
			} else {
				pat3 = pat_tag;
				pat3.Prepend(wxT("void "));
			}

			const wxCharBuffer patbuf2 = _C(pat3);
			get_functions(patbuf2.data(), fooList, ignoreTokens);
			if (fooList.size() == 1) {
				foo = (*fooList.begin());
				foo.m_returnValue.Reset();//clear the dummy return value
				return true;
			}
		}
	}
	return false;
}

void Language::GetLocalVariables(const wxString &in, std::vector<TagEntryPtr> &tags, const wxString &name, size_t flags)
{
	VariableList li;
	Variable var;
	wxString pattern(in);

	const wxCharBuffer patbuf = _C(pattern);
	li.clear();

	TagsManager *mgr = GetTagsManager();
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetPreprocessorAsMap();
	
	get_variables(patbuf.data(), li, ignoreTokens);
	VariableList::iterator iter = li.begin();
	for (; iter != li.end(); iter++) {
		var = (*iter);
		wxString tagName = _U(var.m_name.c_str());

		//if we have name, collect only tags that matches name
		if (name.IsEmpty() == false){
		
			// incase CaseSensitive is not required, make both string lower case
			wxString tmpName(name);
			wxString tmpTagName(tagName);
			if(flags & IgnoreCaseSensitive) {
				tmpName.MakeLower();
				tmpTagName.MakeLower();
			}
			
			if (flags & PartialMatch && !tmpTagName.StartsWith(tmpName))
				continue;
				
			if (flags & ExactMatch && tmpTagName != tmpName)
				continue;
		} // else no name is specified, collect all tags
		
		TagEntryPtr tag(new TagEntry());
		tag->SetName(tagName);
		tag->SetKind(wxT("variable"));
		tag->SetParent(wxT("<local>"));
		tag->SetAccess(wxT("public"));
		tag->SetPattern(_U(var.m_pattern.c_str()));
		tags.push_back(tag);
	}
}

bool Language::OnArrowOperatorOverloading(wxString &typeName, wxString &typeScope)
{
	bool ret(false);

	//collect all functions of typename
	std::vector< TagEntryPtr > tags;
	wxString scope;
	if (typeScope == wxT("<global>"))
		scope << typeName;
	else
		scope << typeScope << wxT("::") << typeName;

	//this function will retrieve the ineherited tags as well
	GetTagsManager()->TagsByScope(scope, tags);
	if (tags.empty() == false) {
		//loop over the tags and scan for operator -> overloading
		for (std::vector< TagEntryPtr >::size_type i=0; i< tags.size(); i++) {
			wxString pattern = tags.at(i)->GetPattern();
			if (pattern.Contains(wxT("operator")) && pattern.Contains(wxT("->"))) {
				//we found our overloading operator
				//extract the 'real' type from the pattern
				clFunction f;
				if (FunctionFromPattern(pattern, f)) {
					typeName = _U(f.m_returnValue.m_type.c_str());
					typeScope = f.m_returnValue.m_typeScope.empty() ? wxT("<global>") : _U(f.m_returnValue.m_typeScope.c_str());
					ret = true;
					break;
				} else {
					//failed to extract the return value from the patterm
					//fallback to the current behavior
					break;
				}
			}
		}
	}
	return ret;
}

void Language::SetTagsManager(TagsManager *tm)
{
	m_tm = tm;
}

TagsManager* Language::GetTagsManager()
{
	if ( !m_tm ) {
		//for backward compatibility allows access to the tags manager using
		//the singleton call
		return TagsManagerST::Get();
	} else {
		return m_tm;
	}
}
