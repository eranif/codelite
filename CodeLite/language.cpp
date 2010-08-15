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
#include "crawler_include.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>

#include "language.h"
#include "variable.h"
#include "function.h"
#include "ctags_manager.h"
#include "y.tab.h"
#include <wx/stopwatch.h>
#include <wx/ffile.h>
#include "map"

//#define __PERFORMANCE
#include "performance.h"

#include "code_completion_api.h"
#include "scope_optimizer.h"

static wxString PathFromNameAndScope(const wxString &typeName, const wxString &typeScope)
{
	wxString path;
	if (typeScope != wxT("<global>"))
		path << typeScope << wxT("::");

	path << typeName;
	return path;
}

static wxString NameFromPath(const wxString &path)
{
	wxString name = path.AfterLast(wxT(':'));
	return name;
}

static wxString ScopeFromPath(const wxString &path)
{
	wxString scope = path.BeforeLast(wxT(':'));
	if (scope.IsEmpty())
		return wxT("<global>");

	if (scope.EndsWith(wxT(":"))) {
		scope.RemoveLast();
	}

	if (scope.IsEmpty())
		return wxT("<global>");

	return scope;
}

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
wxString Language::OptimizeScope(const wxString& srcString)
{
	std::string out;
	const wxCharBuffer inp = srcString.mb_str(wxConvUTF8);
	::OptimizeScope(inp.data(), out);

	wxString scope = _U(out.c_str());
	return scope;
}

ParsedToken* Language::ParseTokens(const wxString &scopeName)
{
	wxString token, delim;
	bool     subscript;
	ParsedToken* header(NULL);
	ParsedToken* currentToken(header);

	while( NextToken(token, delim, subscript) ) {

		ParsedToken* pt = new ParsedToken;
		pt->SetSubscriptOperator( subscript    );
		pt->SetOperator         ( delim        );
		pt->SetPrev             ( currentToken );
		pt->SetCurrentScopeName ( scopeName    );

		ExpressionResult result = ParseExpression( token );
		if(result.m_name.empty() && result.m_isGlobalScope == false) {
			ParsedToken::DeleteTokens( header );
			return NULL;
		}

		if (result.m_isGlobalScope && pt->GetOperator() != wxT("::")) {
			ParsedToken::DeleteTokens( header );
			return NULL;
		}

		if (result.m_isaType) {
			pt->SetTypeScope(result.m_scope.empty() ? wxString(wxT("<global>")) : wxString::From8BitData(result.m_scope.c_str()));
			pt->SetTypeName( wxString::From8BitData(result.m_name.c_str()) );

		} else if (result.m_isGlobalScope) {
			pt->SetTypeScope(wxT("<global>"));
			pt->SetTypeName(wxT("<global>"));

		} else if (result.m_isThis) {
			//-----------------------------------------
			// special handle for 'this' keyword
			//-----------------------------------------

			pt->SetTypeScope( result.m_scope.empty() ? wxString(wxT("<global>")) : wxString::From8BitData(result.m_scope.c_str()));
			if (scopeName == wxT("<global>")) {
				wxLogMessage(wxString::Format(wxT("'this' can not be used in the global scope")));
				ParsedToken::DeleteTokens( header );
				return NULL;
			}

			if (pt->GetOperator() == wxT("::")) {
				wxLogMessage(wxString::Format(wxT("'this' can not be used with operator ::")));
				ParsedToken::DeleteTokens( header );
				return NULL;
			}

			if (result.m_isPtr && pt->GetOperator() == wxT(".")) {
				wxLogMessage(wxString::Format(wxT("Did you mean to use '->' instead of '.' ?")));
				ParsedToken::DeleteTokens( header );
				return NULL;
			}

			if (!result.m_isPtr && pt->GetOperator() == wxT("->")) {
				wxLogMessage(wxString::Format(wxT("Can not use '->' operator on a non pointer object")));
				ParsedToken::DeleteTokens( header );
				return NULL;
			}
			pt->SetTypeName( scopeName );
			pt->SetName(wxT("this"));
		}

		pt->SetIsTemplate( result.m_isTemplate );

		// If the current token is 'this' then the type is actually the
		// current scope
		pt->SetName( _U(result.m_name.c_str()) );

		wxArrayString argsList;
		ParseTemplateInitList(wxString::From8BitData(result.m_templateInitList.c_str()), argsList );
		pt->SetTemplateInitialization( argsList );

		if(currentToken == NULL) {
			header = pt;
			currentToken = pt;
		} else {
			currentToken->SetNext( pt );
			currentToken = pt;
		}
		token.Clear();
		delim.Clear();
		subscript = false;
	}
	return header;
}

bool Language::NextToken(wxString &token, wxString &delim, bool &subscriptOperator)
{
	int type(0);
	int depth(0);
	subscriptOperator = false;
	while ( (type = m_tokenScanner->yylex()) != 0 ) {
		switch (type) {
		case lexTHIS:
			token << wxT("this");
			break;
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
		case wxT('['):
			subscriptOperator = true;
			depth++;
			token << wxT(" ") << _U(m_tokenScanner->YYText());
			break;
		case wxT('<'):
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
		case IDENTIFIER: case wxT(','):  case lexDOUBLE:
		case lexINT:     case lexSTRUCT: case lexLONG:
		case lexENUM:    case lexCHAR:   case UNION:
		case lexCONST:   case lexFLOAT:  case lexSHORT:
		case UNSIGNED:   case SIGNED:    case lexVOID:
		case lexCLASS:   case TYPEDEFname:
			token << wxT(" ") << _U(m_tokenScanner->YYText());
			break;
		default:
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
                                 wxString &typeName, 				//output
                                 wxString &typeScope,				//output
                                 wxString &oper,					//output
                                 wxString &scopeTemplateInitList)	//output
{
	bool evaluationSucceeded = true;

	std::map<wxString, wxString> typeMap = GetTagsManager()->GetCtagsOptions().GetTypesMap();
	wxString statement( stmt );

	// Trim whitespace from right and left
	static wxString trimString(_T("{};\r\n\t\v "));

	statement.erase(0, statement.find_first_not_of(trimString));
	statement.erase(statement.find_last_not_of(trimString)+1);

	wxString lastFuncSig;
	wxString visibleScope, scopeName;

	visibleScope = OptimizeScope(text);

	std::vector<wxString> additionalScopes;

	// Allways use the global namespace as an addition scope
	additionalScopes.push_back( wxT("<global>") );
	scopeName = GetScopeName(text, &additionalScopes);

	TagEntryPtr tag = GetTagsManager()->FunctionFromFileLine(fn, lineno);
	if (tag) {
		lastFuncSig = tag->GetSignature();
	}

	SetLastFunctionSignature(lastFuncSig     );
	SetVisibleScope         (visibleScope    );
	SetAdditionalScopes     (additionalScopes, fn.GetFullPath());

	//get next token using the tokenscanner object
	m_tokenScanner->SetText(_C(statement));

	 // By default we keep the head of the list to the top
	// of the chain
	TokenContainer container;

	container.head = ParseTokens( scopeName );
	if( !container.head ) {
		return false;
	}

	container.current = container.head;

	while ( container.current ) {

		bool res = ProcessToken( &container );
		if ( !res && !container.Rewind()) {
			evaluationSucceeded = false;
			break;

		} else if(!res && container.Rewind()) {
			// ProcessToken() modified the list
			container.SetRewind(false);
			continue;
		}

		container.retries = 0;

		// HACK1: Let the user override the parser decisions
		ExcuteUserTypes(container.current, typeMap);

		// We call here to IsTypeAndScopeExists which will attempt to provide the best scope / type
		// in cases there is a change in the scope we also need to update the templateHelper class
		wxString newScope = typeScope;


		DoIsTypeAndScopeExist( container.current );
		DoExtractTemplateInitListFromInheritance( container.current );

		if(container.current->GetIsTemplate() && container.current->GetTemplateArgList().IsEmpty()) {
			// We got no template declaration...
			container.current->SetTemplateArgList(DoExtractTemplateDeclarationArgs(container.current));
		}

		int  retryCount(0);
		bool cont(false);
		bool cont2(false);

		do {
			CheckForTemplateAndTypedef( container.current );
			// We check subscript operator only once
			cont = (container.current->GetSubscriptOperator() && OnSubscriptOperator( container.current ));
			if(cont) {
				ExcuteUserTypes(container.current, typeMap);
			}
			container.current->SetSubscriptOperator(false);
			cont2 = ( container.current->GetOperator() == wxT("->") && OnArrowOperatorOverloading( container.current ) );
			if(cont2) {
				ExcuteUserTypes( container.current , typeMap);
			}
			retryCount++;
		} while ( (cont || cont2) && retryCount < 5);

		// Update the results we got so far
		typeName  = container.current->GetTypeName();
		typeScope = container.current->GetTypeScope();

		// Keep the last operator used, it is required by the caller
		oper = container.current->GetOperator();

		container.current = container.current->GetNext();
	}

	// release the tokens
	ParsedToken::DeleteTokens( container.head );
	return evaluationSucceeded;
}

bool Language::OnTemplates(ParsedToken *token)
{
	token->ResolveTemplateType( GetTagsManager() );
	return token->ResovleTemplate( GetTagsManager() );
}

void Language::DoSimpleTypedef(ParsedToken *token)
{
	// If the match is typedef, try to replace it with the actual
	// typename
	std::vector<TagEntryPtr> tags;
	std::vector<TagEntryPtr> filteredTags;
	wxString                 path;

	GetTagsManager()->FindByPath(token->GetPath(), tags);

	// try to remove all tags that are Macros from this list
	for (size_t i=0; i<tags.size(); i++) {
		if (!tags.at(i)->IsMacro()) {
			filteredTags.push_back( tags.at(i) );
		}
	}

	if (filteredTags.size() == 1) {
		//we have a single match, test to see if it a typedef
		TagEntryPtr   tag = filteredTags.at(0);
		wxString      tmpInitList;

		wxString realName = tag->NameFromTyperef(tmpInitList);
		if (realName.IsEmpty() == false) {
			token->SetTypeName ( realName        );
			token->SetTypeScope( tag->GetScope() );

			//incase the realName already includes the scope, remove it from the typename
			token->RemoveScopeFromType();
		}
	}
}

bool Language::OnTypedef(ParsedToken *token)
{
	// If the match is typedef, try to replace it with the actual
	// typename
	bool                     res (false);
	std::vector<TagEntryPtr> tags;
	std::vector<TagEntryPtr> filteredTags;
	wxString                 path;
	TagsManager *            tagsManager = GetTagsManager();

	wxString oldName  = token->GetTypeName();
	wxString oldScope = token->GetTypeScope();

	tagsManager->FindByPath(token->GetPath(), tags);

	// try to remove all tags that are Macros from this list
	for (size_t i=0; i<tags.size(); i++) {
		if (!tags.at(i)->IsMacro()) {
			filteredTags.push_back( tags.at(i) );
		}
	}

	if (filteredTags.size() == 1) {
		// We have a single match, test to see if it a typedef
		TagEntryPtr   tag = filteredTags.at(0);
		wxString      tmpInitList;

		wxString realName = tag->NameFromTyperef(tmpInitList);
		if (realName.IsEmpty() == false) {

			wxArrayString scopeTempalteInitList;
			ParseTemplateInitList(tmpInitList, scopeTempalteInitList);

			if(scopeTempalteInitList.IsEmpty() == false) {
				token->SetTemplateInitialization(scopeTempalteInitList);
				token->SetIsTemplate(true);
			}

			token->SetTypeName(realName);
			token->SetTypeScope(tag->GetScope());

			// incase the typeName includes the scope in it, remove it
			token->RemoveScopeFromType();

			// if the resolved type does not exist, try again against the
			// global namespace. IsTypeAndScopeContainer() will check
			// this and will update the typeScope to 'global' if needed
			DoIsTypeAndScopeExist(token);
			res = true;
		}
	}

	if (filteredTags.empty() ) {
		// this is yet another attempt to fix a match which we failed to resolve it completly
		// a good example for such case is using a typedef which was defined inside a function
		// body

		// try to locate any typedefs defined locally
		clTypedefList typedefsList;
		const wxCharBuffer buf = _C(GetVisibleScope());
		get_typedefs(buf.data(), typedefsList);

		if (typedefsList.empty() == false) {
			// take the first match
			clTypedefList::iterator iter = typedefsList.begin();
			for (; iter != typedefsList.end(); iter++) {
				clTypedef td = *iter;
				wxString matchName(td.m_name.c_str(), wxConvUTF8);
				if (matchName == token->GetTypeName()) {
					wxArrayString scopeTempalteInitList;
					wxString      tmpInitList;

					token->SetTypeName (wxString(td.m_realType.m_type.c_str(),         wxConvUTF8));
					token->SetTypeScope(wxString(td.m_realType.m_typeScope.c_str(),    wxConvUTF8));
					tmpInitList = wxString(td.m_realType.m_templateDecl.c_str(), wxConvUTF8);

					ParseTemplateInitList(tmpInitList, scopeTempalteInitList);
					token->SetTemplateInitialization(scopeTempalteInitList);
					res = true;
					break;
				}
			}
		}
	}
	return res && (oldName != token->GetTypeName() || oldScope != token->GetTypeScope());
}

void Language::ParseTemplateArgs(const wxString &argListStr, wxArrayString &argsList)
{
	CppScanner scanner;
	scanner.SetText(_C(argListStr));
	int type = scanner.yylex();
	wxString word = _U(scanner.YYText());

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
		type = scanner.yylex();
		if (type == 0) {
			break;
		}

		switch (type) {
		case lexCLASS:
		case IDENTIFIER: {
			wxString word = _U(scanner.YYText());
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

void Language::ParseTemplateInitList(const wxString &argListStr, wxArrayString &argsList)
{
	CppScanner scanner;
	scanner.SetText(_C(argListStr));
	int type = scanner.yylex();
	wxString word = _U(scanner.YYText());

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
		type = scanner.yylex();
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
				typeName << _U(scanner.YYText());
			}
			break;
		}
	}

	if (typeName.Trim().Trim(false).IsEmpty() == false) {
		argsList.Add(typeName.Trim().Trim(false));
	}
	typeName.Empty();
}

void Language::ParseComments(const wxFileName &fileName, std::vector<CommentPtr> *comments)
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

			// save the previous comment buffer
			if ( comment.IsEmpty() == false ) {
				comments->push_back( new Comment( comment, fileName.GetFullPath(), line - 1) );
				comment.Empty();
				line = -1;
			}

			// first time or no comment is buffer
			if ( comment.IsEmpty() ) {
				comment = m_scanner->GetComment();
				line = m_scanner->lineno();
				m_scanner->ClearComment();
				continue;
			}

			comments->push_back( new Comment( m_scanner->GetComment(), fileName.GetFullPath(), m_scanner->lineno()-1) );
			comment.Empty();
			line = -1;
			m_scanner->ClearComment();

		} else if ( type == CComment ) {
			comments->push_back( new Comment( m_scanner->GetComment(), fileName.GetFullPath(), m_scanner->lineno()) );
			m_scanner->ClearComment();
		}
	}

	if ( comment.IsEmpty() == false ) {
		comments->push_back( new Comment( comment, fileName.GetFullPath(), line - 1) );
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
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

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
	ExpressionResult result;
	if ( in.IsEmpty() ) {
		result.m_isGlobalScope = true;

	} else {
		const wxCharBuffer buf = _C(in);
		result = parse_expression(buf.data());
	}
	return result;
}

bool Language::ProcessToken(TokenContainer *tokeContainer)
{
	// try local scope
	VariableList li;
	FunctionList fooList;

	//first we try to match the current scope
	std::vector<TagEntryPtr> tags;

	TagsManager *mgr = GetTagsManager();
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

	wxString type;
	wxString typeScope;
	ParsedToken *token = tokeContainer->current;

	// Handle 'this'
	if(token->IsThis()) {
		token->SetTypeName(token->GetContextScope());
		token->SetTypeScope(wxT("<global>"));
		return true;
	}

	bool hasMatch = DoSearchByNameAndScope(token->GetName(), token->GetContextScope(), tags, type, typeScope);
	if ( !hasMatch ) {
		if ( token->GetPrev() == NULL) {
			// We are the first token in the chain
			// examine the local scope
			const wxCharBuffer buf  = _C(GetVisibleScope()         );
			const wxCharBuffer buf2 = _C(GetLastFunctionSignature() + wxT(";"));

			get_variables(buf.data(), li, ignoreTokens, false);
			get_variables(buf2.data(), li, ignoreTokens, true);

			// Search for a full match in the returned list
			for (VariableList::iterator iter = li.begin(); iter != li.end(); iter++) {
				Variable var = (*iter);
				wxString var_name = _U(var.m_name.c_str());
				if (var_name == token->GetName()) {

					DoFixTokensFromVariable(tokeContainer, wxString::From8BitData(var.m_completeType.c_str()));
					return false;

				}
			}

			// failed to find it in the local scope
			// try the additional scopes
			for (size_t i=0; i<GetAdditionalScopes().size(); i++) {
				tags.clear();
				if (DoSearchByNameAndScope(token->GetName(), GetAdditionalScopes().at(i), tags, type, typeScope)) {
					token->SetTypeName(type);
					token->SetTypeScope(typeScope);
					return DoCorrectUsingNamespaces(token, tags);
				}
			}
		}
		return false;

	} else if( tags.size() ) {

		if(token->GetPrev() == NULL) {

			// we are first in the chain, but still we exists in the database
			// this means that we are either a scope followed by operator (e.g. Foo::)
			// or global variable or member
			// for the last two cases, we need to handle this as if we did not find a match in the
			// database
			li.clear();

			if(tags.at(0)->GetKind() == wxT("member") || tags.at(0)->GetKind() == wxT("variable")) {
				const wxCharBuffer buf = _C(tags.at(0)->GetPattern());
				get_variables(buf.data(), li, ignoreTokens, true);

				// Search for a full match in the returned list
				for (VariableList::iterator iter = li.begin(); iter != li.end(); iter++) {
					Variable var = (*iter);
					wxString var_name = _U(var.m_name.c_str());
					if (var_name == tags.at(0)->GetName()) {
						DoFixTokensFromVariable(tokeContainer, _U(var.m_completeType.c_str()));
					}
				}
				return false;

			}
		}

		// we got a match
		token->SetTypeName ( type      );
		token->SetTypeScope( typeScope );


		return DoCorrectUsingNamespaces(token, tags);
	}
	return false;
}

bool Language::CorrectUsingNamespace(wxString &type, wxString &typeScope, const wxString &parentScope, std::vector<TagEntryPtr> &tags)
{
	wxString strippedScope(typeScope);
	wxArrayString tmplInitList;
	DoRemoveTempalteInitialization(strippedScope, tmplInitList);

	if (!GetTagsManager()->IsTypeAndScopeExists(type, strippedScope)) {
		if (GetAdditionalScopes().empty() == false) {
			//the type does not exist in the global scope,
			//try the additional scopes
			for (size_t i=0; i<GetAdditionalScopes().size(); i++) {
				tags.clear();

				// try the typeScope in any of the "using namespace XXX" declarations
				// passed here (i.e. moreScopes variable)
				wxString newScope(GetAdditionalScopes().at(i));
				if (typeScope != wxT("<global>")) {
					newScope << wxT("::") << typeScope;
				}

				if (DoSearchByNameAndScope(type, newScope, tags, type, typeScope)) {
					return true;
				}
			}
		}

		//if we are here, it means that the more scopes did not matched any, try the parent scope
		tags.clear();

		wxString tmpParentScope(parentScope);
		wxString cuttedScope(tmpParentScope);

		tmpParentScope.Replace(wxT("::"), wxT("@"));

		cuttedScope.Trim().Trim(false);
		while ( !cuttedScope.IsEmpty() ) {

			// try all the scopes of thse parent:
			// for example:
			// assuming the parent scope is A::B::C
			// try to match:
			// A::B::C
			// A::B
			// A
			tags.clear();
			if (DoSearchByNameAndScope(type, cuttedScope, tags, type, typeScope)) {
				return true;
			}

			// get the next scope to search
			cuttedScope = tmpParentScope.BeforeLast(wxT('@'));
			cuttedScope.Replace(wxT("@"), wxT("::"));
			cuttedScope.Trim().Trim(false);

			tmpParentScope = tmpParentScope.BeforeLast(wxT('@'));
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
	PERF_BLOCK("DoSearchByNameAndScope") {
		std::vector<TagEntryPtr> tmp_tags;
		GetTagsManager()->FindByNameAndScope(name, scopeName, tmp_tags);
		if ( tmp_tags.empty() ) {
			// try the global scope maybe?
			GetTagsManager()->FindByNameAndScope(name, wxT("<global>"), tmp_tags);
		}

		// filter macros from the result
		for (size_t i=0; i<tmp_tags.size(); i++) {
			TagEntryPtr t = tmp_tags.at(i);
			if (t->GetKind() != wxT("macro") && !t->IsConstructor()) {
				tags.push_back(t);
			}
		}

		if (tags.size() == 1) {
			TagEntryPtr tag(tags.at(0));
			//we have a single match!
			if ( tag->IsMethod() ) {

				clFunction foo;
				if (FunctionFromPattern(tag, foo)) {
					type      = _U(foo.m_returnValue.m_type.c_str());

					// Guess the return value scope:
					// if we got scope, use it
					if (foo.m_returnValue.m_typeScope.empty() == false)
						typeScope = _U(foo.m_returnValue.m_typeScope.c_str());

					else {

						// we got no scope to use.
						// try the wxT("<global>") scope
						typeScope = wxT("<global>");
						if (! GetTagsManager()->GetDatabase()->IsTypeAndScopeExistLimitOne(type, typeScope) ) {
							// try the current scope
							typeScope = scopeName;
						}
						// TODO: continue to scan the entire 'using namespaces' stack
					}
					return true;
				}

				return false;

			} else if (tag->GetKind() == wxT("member") || tag->GetKind() == wxT("variable")) {
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

			// if list contains more than one entry, check if all entries are of type 'function' or 'prototype'
			// (they can be mixed). If all entries are of one of these types, test their return value,
			// if all have the same return value, then we are ok
			clFunction foo;
			for (size_t i=0; i<tags.size(); i++) {
				TagEntryPtr tag(tags.at(i));
				if (!FunctionFromPattern(tag, foo)) {
					break;
				}

				type      = _U(foo.m_returnValue.m_type.c_str());
				typeScope = foo.m_returnValue.m_typeScope.empty() ? tag->GetScope() : _U(foo.m_returnValue.m_typeScope.c_str());
				if (type != wxT("void")) {
					return true;
				}
			}

			return false;
		}
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
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

	get_variables(patbuf.data(), li, ignoreTokens, false);
	VariableList::iterator iter = li.begin();
	for (; iter != li.end(); iter++) {
		Variable v = *iter;
		if (name == _U(v.m_name.c_str())) {
			var = (*iter);
			return true;
		}
	} // if(li.size() == 1)
	return false;
}

bool Language::FunctionFromPattern(TagEntryPtr tag, clFunction &foo)
{
	FunctionList fooList;
	wxString pattern(tag->GetPattern());
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
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

	// use the replacement table on the pattern before processing it
	DoReplaceTokens(pattern, GetTagsManager()->GetCtagsOptions().GetTokensWxMap());

	const wxCharBuffer patbuf = _C(pattern);
	get_functions(patbuf.data(), fooList, ignoreTokens);
	if (fooList.size() == 1) {
		foo = (*fooList.begin());
		DoFixFunctionUsingCtagsReturnValue(foo, tag);
		return true;

	} else if (fooList.size() == 0) {
		// Fail to parse the statement, assume we got a broken pattern
		// (this can happen because ctags keeps only the first line of a function which was declared
		// over multiple lines)
		// Manually construct the pattern from TagEntry
		wxString pat2;

		pat2 << tag->GetReturnValue() << wxT(" ") << tag->GetName() << tag->GetSignature() << wxT(";");

		// use the replacement table on the pattern before processing it
		DoReplaceTokens(pat2, GetTagsManager()->GetCtagsOptions().GetTokensWxMap());

		const wxCharBuffer patbuf1 = _C(pat2);
		get_functions(patbuf1.data(), fooList, ignoreTokens);
		if (fooList.size() == 1) {
			foo = (*fooList.begin());
			DoFixFunctionUsingCtagsReturnValue(foo, tag);
			return true;

		} else if (fooList.empty()) {
			//try a nasty hack:
			//the yacc cant find ctor declarations
			//so add a 'void ' infront of the function...
			wxString pat_tag(pattern);
			pat_tag = pat_tag.Trim(false).Trim();
			wxString pat3;
			bool dummyReturnValue(true);

			// failed to parse function.
			if (tag->GetReturnValue().IsEmpty() == false) {
				pat3 = pat_tag;
				pat3.Prepend(tag->GetReturnValue() + wxT(" "));
				dummyReturnValue = false;

			} else {
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
			}
			const wxCharBuffer patbuf2 = _C(pat3);
			get_functions(patbuf2.data(), fooList, ignoreTokens);
			if (fooList.size() == 1) {
				foo = (*fooList.begin());

				if (dummyReturnValue)
					foo.m_returnValue.Reset(); //clear the dummy return value
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

	pattern = pattern.Trim().Trim(false);
	const wxCharBuffer patbuf = _C(pattern);
	li.clear();

	TagsManager *mgr = GetTagsManager();
	std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

	// incase the 'in' string starts with '(' it is most likely that the input string is the
	// function signature in that case we pass 'true' as the fourth parameter to get_variables(..)
	get_variables(patbuf.data(), li, ignoreTokens, pattern.StartsWith(wxT("(")));

	VariableList::iterator iter = li.begin();
	for (; iter != li.end(); iter++) {
		var = (*iter);
		if (var.m_name.empty()) {
			continue;
		}

		wxString tagName = _U(var.m_name.c_str());

		//if we have name, collect only tags that matches name
		if (name.IsEmpty() == false) {

			// incase CaseSensitive is not required, make both string lower case
			wxString tmpName(name);
			wxString tmpTagName(tagName);
			if (flags & IgnoreCaseSensitive) {
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

		wxString scope;
		if (var.m_typeScope.empty() == false) {
			scope << wxString(var.m_typeScope.c_str(), wxConvUTF8) << wxT("::");
		}
		if (var.m_type.empty() == false) {
			scope << wxString(var.m_type.c_str(), wxConvUTF8);
		}
		tag->SetScope(scope);
		tag->SetAccess(wxT("public"));
		tag->SetPattern(_U(var.m_pattern.c_str()));
		tags.push_back(tag);
	}
}

bool Language::OnArrowOperatorOverloading(ParsedToken *token)
{
	bool ret(false);
	//collect all functions of typename
	std::vector< TagEntryPtr > tags;
	wxString                   typeScope ( token->GetTypeScope() );
	wxString                   typeName  ( token->GetTypeName()  );

	// this function will retrieve the ineherited tags as well
	GetTagsManager()->GetDereferenceOperator(token->GetPath(), tags);

	if (tags.size() == 1) {
		//loop over the tags and scan for operator -> overloading
		//we found our overloading operator
		//extract the 'real' type from the pattern
		clFunction f;
		if (FunctionFromPattern(tags.at(0), f)) {

			typeName  = _U(f.m_returnValue.m_type.c_str());
			typeScope = f.m_returnValue.m_typeScope.empty() ? token->GetPath() : _U(f.m_returnValue.m_typeScope.c_str());

			token->SetTypeName ( typeName );
			token->SetTypeScope( typeScope);

			// Call the magic method that fixes typename/typescope
			DoIsTypeAndScopeExist( token );
			ret = true;
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

void Language::DoRemoveTempalteInitialization(wxString &str, wxArrayString &tmplInitList)
{
	CppScanner sc;
	sc.SetText( _C(str) );

	int type(0);
	int depth(0);

	wxString token;
	wxString outputString;
	str.Clear();

	while ((type = sc.yylex()) != 0) {
		if (type == 0)
			return;

		token = _U(sc.YYText());
		switch (type) {
		case wxT('<'):
			if (depth ==0) outputString.Clear();
			outputString << token;
			depth++;
			break;

		case wxT('>'):
			outputString << token;
			depth--;
			break;

		default:
			if (depth > 0) outputString << token;
			else str << token;
			break;
		}
	}

	if (outputString.IsEmpty() == false) {
		ParseTemplateInitList(outputString, tmplInitList);
	}
}

void Language::DoFixFunctionUsingCtagsReturnValue(clFunction& foo, TagEntryPtr tag)
{
	if (foo.m_returnValue.m_type.empty()) {

		// Use the CTAGS return value
		wxString ctagsRetValue = tag->GetReturnValue();
		DoReplaceTokens(ctagsRetValue, GetTagsManager()->GetCtagsOptions().GetTokensWxMap());

		const wxCharBuffer cbuf = ctagsRetValue.mb_str(wxConvUTF8);
		std::map<std::string, std::string> ignoreTokens = GetTagsManager()->GetCtagsOptions().GetTokensMap();

		VariableList li;
		get_variables(cbuf.data(), li, ignoreTokens, false);
		if (li.size() == 1) {
			foo.m_returnValue = *li.begin();
		}
	}
}

void Language::DoReplaceTokens(wxString &inStr, const std::map<wxString, wxString>& ignoreTokens)
{
	if(inStr.IsEmpty())
		return;

	std::map<wxString, wxString>::const_iterator iter = ignoreTokens.begin();
	for(; iter != ignoreTokens.end(); iter++) {
		wxString findWhat    = iter->first;
		wxString replaceWith = iter->second;

		if(findWhat.StartsWith(wxT("re:"))) {
			findWhat.Remove(0, 3);
			wxRegEx re(findWhat);
			if(re.IsValid() && re.Matches(inStr)) {
				re.ReplaceAll(&inStr, replaceWith);
			}
		} else {
			// Simple replacement
			int where = inStr.Find(findWhat);
			if(where >= 0) {
				if(inStr.Length() > static_cast<size_t>(where)) {
					// Make sure that the next char is a non valid char otherwise this is not a complete word
					if(inStr.Mid(where, 1).find_first_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890")) != wxString::npos) {
						// the match is not a full word
						continue;
					} else {
						inStr.Replace(findWhat, replaceWith);
					}
				} else {
					inStr.Replace(findWhat, replaceWith);
				}
			}
		}
	}
}

void Language::CheckForTemplateAndTypedef(ParsedToken *token)
{
	bool typedefMatch;
	bool templateMatch;
	int  retry(0);

	do {
		typedefMatch = OnTypedef(token);

		// Attempt to fix the result
		DoIsTypeAndScopeExist( token );

		if (typedefMatch) {
			DoExtractTemplateInitListFromInheritance(token);

			// The typeName was a typedef, so make sure we update the template declaration list
			// with the actual type
			std::vector<TagEntryPtr> tags;
			GetTagsManager()->FindByPath(token->GetPath(), tags);
			if (tags.size() == 1 && !tags.at(0)->IsTypedef()) {

				// Not a typedef
				token->SetTemplateArgList(DoExtractTemplateDeclarationArgs(tags.at(0)));
				token->SetIsTemplate( token->GetTemplateArgList().IsEmpty() == false );

			} else if (tags.size() == 1) {

				// Typedef
				TagEntryPtr t = tags.at(0);
				wxString pattern ( t->GetPattern() );
				wxArrayString tmpInitList;
				DoRemoveTempalteInitialization(pattern, tmpInitList);

				// Incase any of the template initialization list is a
				// typedef, resolve it as well
				DoResolveTemplateInitializationList(tmpInitList);
				token->SetTemplateInitialization( tmpInitList );
			}
		}

		templateMatch = OnTemplates( token );
		if(templateMatch) {
			DoIsTypeAndScopeExist( token );
			token->SetIsTemplate(false);
			DoExtractTemplateInitListFromInheritance( token );
		}

		retry++;

	} while ( (typedefMatch || templateMatch) && retry < 15 ) ;
}

void Language::DoResolveTemplateInitializationList(wxArrayString &tmpInitList)
{
	for (size_t i=0; i<tmpInitList.GetCount(); i++) {
		wxString fixedTemplateArg;
		wxString name  = NameFromPath (tmpInitList.Item(i));

		wxString tmpScope = ScopeFromPath(tmpInitList.Item(i));
		wxString scope = tmpScope == wxT("<global>") ? m_templateHelper.GetPath() : tmpScope;

		ParsedToken tok;
		tok.SetTypeName(name);
		tok.SetTypeScope(scope);

		DoSimpleTypedef( &tok );

		name = tok.GetTypeName();
		scope = tok.GetTypeScope();

		if (GetTagsManager()->GetDatabase()->IsTypeAndScopeExistLimitOne(name, scope) == false) {
			// no match, assume template: NAME only
			tmpInitList.Item(i) = name;
		} else
			tmpInitList.Item(i) = PathFromNameAndScope(name, scope);
	}
}

wxArrayString Language::DoExtractTemplateDeclarationArgs(ParsedToken *token)
{
	// Find a tag in the database that matches this find and
	// extract the template declaration for it
	std::vector<TagEntryPtr> tags;
	GetTagsManager()->FindByPath(token->GetPath(), tags);
	if (tags.size() != 1)
		return wxArrayString();

	return DoExtractTemplateDeclarationArgs(tags.at(0));
}

wxArrayString Language::DoExtractTemplateDeclarationArgsFromScope()
{
	wxString tmpParentScope(m_templateHelper.GetTypeScope());
	wxString cuttedScope(tmpParentScope);

	tmpParentScope.Replace(wxT("::"), wxT("@"));
	std::vector<TagEntryPtr> tags;

	cuttedScope.Trim().Trim(false);
	while ( !cuttedScope.IsEmpty() ) {

		// try all the scopes of thse parent:
		// for example:
		// assuming the parent scope is A::B::C
		// try to match:
		// A::B::C
		// A::B
		// A
		tags.clear();
		GetTagsManager()->FindByPath(cuttedScope, tags);
		if (tags.size() == 1) {
			if (tags.at(0)->GetPattern().Contains(wxT("template"))) {
				return DoExtractTemplateDeclarationArgs(tags.at(0));
			}
		}

		// get the next scope to search
		cuttedScope = tmpParentScope.BeforeLast(wxT('@'));
		cuttedScope.Replace(wxT("@"), wxT("::"));
		cuttedScope.Trim().Trim(false);

		tmpParentScope = tmpParentScope.BeforeLast(wxT('@'));
	}

	return wxArrayString();
}

wxArrayString Language::DoExtractTemplateDeclarationArgs(TagEntryPtr tag)
{
	wxString pattern = tag->GetPattern();
	wxString templateString;

	//extract the template declartion list
	CppScanner declScanner;
	declScanner.ReturnWhite(1);
	declScanner.SetText( _C(pattern) );
	bool foundTemplate(false);
	int type (0);
	while ( true ) {
		type = declScanner.yylex();
		if ( type == 0 ) //eof
			break;

		wxString word = _U(declScanner.YYText());
		switch (type) {
		case IDENTIFIER:
			if (word == wxT("template")) {
				foundTemplate = true;

			} else if (foundTemplate) {
				templateString << word;

			}
			break;

		default:
			if ( foundTemplate ) {
				templateString << word;
			}
			break;
		}
	}

	if (foundTemplate) {
		wxArrayString ar;
		ParseTemplateArgs(templateString, ar);
		return ar;
	}
	return wxArrayString();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//      Scope Class
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void TemplateHelper::SetTemplateInstantiation(const wxArrayString& templInstantiation)
{
	// incase we are using template argument as template instantiation,
	// we should perform the replacement or else we will lose
	// the actual tempalte instantiation list
	// an example for such cases:
	// template <class _Tp> class vector {
	//    typedef Something<_Tp> reference;
	//  reference get();
	// };
	// Now, by attempting to resolve this:
	// vector<wxString> v;
	// v.get()->
	// we should replace Something<_Tp> into Something<wxString> *before* we continue with
	// the resolving

	wxArrayString newInstantiationList = templInstantiation;
	// search for 'name' in the declaration list
	for (size_t i=0; i<newInstantiationList.GetCount(); i++) {
		int where = this->templateDeclaration.Index(newInstantiationList.Item(i));
		if (where != wxNOT_FOUND) {
			wxString name = Substitute(newInstantiationList.Item(i));
			if (!name.IsEmpty())
				newInstantiationList[i] = name;
		}
	}

	templateInstantiationVector.push_back(newInstantiationList);
}

wxString TemplateHelper::Substitute(const wxString& name)
{
//	for(size_t i=0; i<templateInstantiationVector.size(); i++) {
	int count = static_cast<int>(templateInstantiationVector.size());
	for (int i=count-1; i>=0; i--) {
		int where = templateDeclaration.Index(name);
		if (where != wxNOT_FOUND) {
			// it exists, return the name in the templateInstantiation list
			if (templateInstantiationVector.at(i).GetCount() > (size_t)where && templateInstantiationVector.at(i).Item(where) != name)
				return templateInstantiationVector.at(i).Item(where);
		}
	}
	return wxT("");
}

void TemplateHelper::Clear()
{
	typeName.Clear();
	typeScope.Clear();
	templateInstantiationVector.clear();
	templateDeclaration.Clear();
}

wxString TemplateHelper::GetPath() const
{
	wxString path;
	if (typeScope != wxT("<global>"))
		path << typeScope << wxT("::");

	path << typeName;
	return path;
}

void Language::SetAdditionalScopes(const std::vector<wxString>& additionalScopes, const wxString &filename)
{

	if( !(GetTagsManager()->GetCtagsOptions().GetFlags() &  CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING) ) {
		this->m_additionalScopes = additionalScopes;

	} else {
		this->m_additionalScopes.clear();
		// do a deep scan of the entire include tree
		wxArrayString includePaths = GetTagsManager()->GetProjectPaths();
		{
			wxCriticalSectionLocker locker( GetTagsManager()->m_crawlerLocker );

			fcFileOpener::Instance()->ClearResults();
			fcFileOpener::Instance()->ClearSearchPath();
			for(size_t i=0; i<includePaths.GetCount(); i++) {
				fcFileOpener::Instance()->AddSearchPath( includePaths.Item(i).mb_str(wxConvUTF8).data() );
			}

			// Invoke the crawler
			const wxCharBuffer cfile = filename.mb_str(wxConvUTF8);
			crawlerScan( cfile.data() );

			std::set<std::string>::iterator iter = fcFileOpener::Instance()->GetNamespaces().begin();
			for(; iter != fcFileOpener::Instance()->GetNamespaces().end(); iter++) {
				this->m_additionalScopes.push_back( wxString(iter->c_str(), wxConvUTF8) );
			}
		}
	}
}

const std::vector<wxString>& Language::GetAdditionalScopes() const
{
	return m_additionalScopes;
}

bool Language::OnSubscriptOperator(ParsedToken *token)
{
	bool ret(false);
	//collect all functions of typename
	std::vector< TagEntryPtr > tags;
	wxString scope;
	wxString typeName (token->GetTypeName() );
	wxString typeScope(token->GetTypeScope());

	if (typeScope == wxT("<global>"))
		scope << token->GetTypeName();
	else
		scope << token->GetTypeScope() << wxT("::") << token->GetTypeName();

	// this function will retrieve the ineherited tags as well
	GetTagsManager()->GetSubscriptOperator(scope, tags);
	if (tags.size() == 1) {
		//we found our overloading operator
		//extract the 'real' type from the pattern
		clFunction f;
		if (FunctionFromPattern(tags.at(0), f)) {
			token->SetTypeName( _U(f.m_returnValue.m_type.c_str()) );

			// first assume that the return value has the same scope like the parent (unless the return value has a scope)
			token->SetTypeScope( f.m_returnValue.m_typeScope.empty() ? scope : _U(f.m_returnValue.m_typeScope.c_str()) );

			// Call the magic method that fixes typename/typescope
			DoIsTypeAndScopeExist( token );
			ret = true;
		}
	}
	return ret;
}

void Language::ExcuteUserTypes(ParsedToken *token, const std::map<wxString, wxString> &typeMap)
{
	// HACK1: Let the user override the parser decisions
	wxString path = token->GetPath();
	std::map<wxString, wxString>::const_iterator where = typeMap.find(path);
	if (where != typeMap.end()) {
		wxArrayString argList;
		token->SetTypeName(where->second.BeforeFirst(wxT('<')));
		wxString argsString = where->second.AfterFirst(wxT('<'));
		argsString.Prepend(wxT("<"));

		DoRemoveTempalteInitialization(argsString, argList);
		if(argList.IsEmpty() == false) {
			token->SetTemplateInitialization(argList);
			token->SetIsTemplate(true);
		}
	}
}

bool Language::DoIsTypeAndScopeExist(ParsedToken* token)
{
	wxString type (token->GetTypeName());
	wxString scope(token->GetTypeScope());
	bool res = GetTagsManager()->IsTypeAndScopeExists(type, scope);

	token->SetTypeName( type );
	token->SetTypeScope( scope );
	return res;
}

bool Language::DoCorrectUsingNamespaces(ParsedToken* token, std::vector<TagEntryPtr> tags)
{
	wxString type (token->GetTypeName());
	wxString scope(token->GetTypeScope());
	bool res = CorrectUsingNamespace(type, scope, token->GetContextScope(), tags);

	token->SetTypeName( type );
	token->SetTypeScope( scope );

	return res;
}

void Language::DoExtractTemplateInitListFromInheritance(TagEntryPtr tag, ParsedToken *token)
{
	wxArrayString initList;
	wxString      parent;
	wxString      scope;

	if(token->GetIsTemplate()) {
		// if this token is already tagged as 'template' dont
		// change this
		return;
	}

	// Loop over the parents of 'tag' and search for any template parent
	// In case we find one, extract the template initialization list from
	// the parent inheritance line and copy it to the current token.

	// If we do find a match, search for the parent itself in the database
	// and extract its template declaration list
	if(tag->IsClass() || tag->IsStruct()) {
		// returns the inheris string with template initialization list
		wxArrayString inherits           = tag->GetInheritsAsArrayWithTemplates();
		wxArrayString inheritsNoTemplate = tag->GetInheritsAsArrayNoTemplates();
		size_t i=0;
		for(; i<inherits.size(); i++) {
			DoRemoveTempalteInitialization(inherits.Item(i), initList);
			if(initList.IsEmpty() == false) {
				break;
			}
		}

		if(initList.IsEmpty() == false) {
			token->SetIsTemplate(true);
			token->SetTemplateInitialization( initList );

			if(i < inheritsNoTemplate.GetCount()) {
				parent = inheritsNoTemplate.Item(i);
				scope  = tag->GetScope();

				// Find this parent
				GetTagsManager()->IsTypeAndScopeExists(parent, scope);
				if(scope.IsEmpty() == false && scope != wxT("<global>")) {
					parent.Prepend(scope + wxT("::"));
				}

				std::vector<TagEntryPtr> tags;
				GetTagsManager()->FindByPath(parent, tags);
				if(tags.size() == 1) {
					wxArrayString newArgList = DoExtractTemplateDeclarationArgs(tags.at(0));
					if(newArgList.IsEmpty() == false) {
						token->SetTemplateArgList( newArgList );
					}
				}
			}
		}
	}
}

void Language::DoExtractTemplateInitListFromInheritance(ParsedToken *token)
{
	std::vector<TagEntryPtr> tags;
	GetTagsManager()->FindByPath( token->GetPath(), tags );
	if(tags.size() == 1) {
		DoExtractTemplateInitListFromInheritance(tags.at(0), token);
	}
}

void Language::DoFixTokensFromVariable(TokenContainer* tokeContainer, const wxString& variableDecl)
{
	// the current tokan is indeed defined on the local stack.
	// what we do now is creating new chain of tokens based on the
	// variable declaration, removing the token that represents the local variable
	// and link the two chains together
	//
	// In addition, we copy the subscript operator flag
	// from the variable into the token declaration
	ParsedToken *token = tokeContainer->current;
	wxString scopeName = token->GetCurrentScopeName();
	wxString op        = token->GetOperator();
	bool     subscript = token->GetSubscriptOperator();

	wxString newTextToParse;
	newTextToParse << variableDecl << op;
	m_tokenScanner->SetText( newTextToParse.To8BitData() );
	ParsedToken *newToken = ParseTokens( scopeName );
	if(newToken) {
		// copy the subscript operator from the local variable token to the
		// last token in the new parsed list
		ParsedToken *lastToken = newToken;
		while(lastToken && lastToken->GetNext()) {
			lastToken = lastToken->GetNext();
		}
		lastToken->SetSubscriptOperator(subscript);
		// If the local variable token has more tokens down the chain,
		// disconnect it from them while connecting the rest of the
		// tokens to the newly parsed list
		if(token->GetNext()) {
			lastToken->SetNext( token->GetNext() );
			token->GetNext()->SetPrev( lastToken );
			token->SetNext(NULL);
		}
		// free the token
		ParsedToken::DeleteTokens( token );
		tokeContainer->head    = newToken;
		tokeContainer->current = newToken;
		tokeContainer->SetRewind(true);
	}
}
