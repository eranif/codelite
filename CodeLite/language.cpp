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
#include "crawler_include.h"
#include "file_logger.h"
#include "precompiled_header.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>

#include "CxxLexerAPI.h"
#include "CxxPreProcessor.h"
#include "CxxScannerTokens.h"
#include "CxxTemplateFunction.h"
#include "CxxUsingNamespaceCollector.h"
#include "CxxVariableScanner.h"
#include "ctags_manager.h"
#include "function.h"
#include "language.h"
#include "map"
#include "pptable.h"
#include "variable.h"
#include "y.tab.h"
#include <algorithm>
#include <wx/ffile.h>
#include <wx/stopwatch.h>

//#define __PERFORMANCE
#include "performance.h"

#include "code_completion_api.h"
#include "scope_optimizer.h"

static wxString PathFromNameAndScope(const wxString& typeName, const wxString& typeScope)
{
    wxString path;
    if(typeScope != wxT("<global>")) path << typeScope << wxT("::");

    path << typeName;
    return path;
}

static wxString NameFromPath(const wxString& path)
{
    wxString name = path.AfterLast(wxT(':'));
    return name;
}

static wxString ScopeFromPath(const wxString& path)
{
    wxString scope = path.BeforeLast(wxT(':'));
    if(scope.IsEmpty()) return wxT("<global>");

    if(scope.EndsWith(wxT(":"))) { scope.RemoveLast(); }

    if(scope.IsEmpty()) return wxT("<global>");

    return scope;
}

Language::Language()
    : m_expression(wxEmptyString)
    , m_scanner(new CppScanner())
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
    delimArr.push_back(wxT("@"));
    SetAutoCompDeliemters(delimArr);
}

/// Destructor
Language::~Language() {}

#define SCP_STATE_NORMAL 0
#define SCP_STATE_IN_IF 1
#define SCP_STATE_IN_WHILE 2
#define SCP_STATE_IN_FOR 3
#define SCP_STATE_IN_CATCH 4
#define SCP_STATE_IN_FOR_NO_SEMICOLON 5

/// Return the visible scope until pchStopWord is encountered
wxString Language::OptimizeScope(const wxString& srcString, int lastFuncLine, wxString& localsScope)
{
    CxxTokenizer tokenizer;
    std::stack<wxString> scopes;
    tokenizer.Reset(srcString);

    CxxLexerToken token;
    wxString currentScope;
    int parenthesisDepth = 0;
    int state = SCP_STATE_NORMAL;
    while(tokenizer.NextToken(token)) {
        if(tokenizer.IsInPreProcessorSection()) continue;
        switch(state) {
        case SCP_STATE_NORMAL:
            switch(token.GetType()) {
            case '{':
                currentScope << "{";
                scopes.push(currentScope);
                currentScope.clear();
                break;
            case '}':
                if(scopes.empty()) return ""; // Invalid braces count
                currentScope = scopes.top();
                scopes.pop();
                currentScope << "} ";
                break;
            case T_IF:
                state = SCP_STATE_IN_IF;
                currentScope << " if ";
                break;
            case T_WHILE:
                state = SCP_STATE_IN_WHILE;
                currentScope << " while ";
                break;
            case T_FOR:
                state = SCP_STATE_IN_FOR_NO_SEMICOLON;
                currentScope << ";";
                break;
            case T_CATCH:
                state = SCP_STATE_IN_CATCH;
                currentScope << ";";
                break;
            case '(':
                parenthesisDepth++;
                currentScope << "(";
                // Handle lambda
                // If we are enterting lamda function defenition, collect the locals
                // this is exactly what we in 'catch' hence the state change to SCP_STATE_IN_CATCH
                if(tokenizer.GetLastToken().GetType() == ']') { state = SCP_STATE_IN_CATCH; }
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                break;
            default:
                if(parenthesisDepth == 0) { currentScope << " " << token.GetWXString(); }
                break;
            }
            break;
        case SCP_STATE_IN_WHILE:
        case SCP_STATE_IN_IF:
            switch(token.GetType()) {
            case '(':
                parenthesisDepth++;
                currentScope << "(";
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                if(parenthesisDepth == 0) { state = SCP_STATE_NORMAL; }
                break;
            }
            break;
        case SCP_STATE_IN_FOR_NO_SEMICOLON:
            switch(token.GetType()) {
            case '(':
                parenthesisDepth++;
                currentScope << "(";
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                if(parenthesisDepth == 0) { state = SCP_STATE_NORMAL; }
                break;
            case ';':
                currentScope << ";";
                state = SCP_STATE_IN_FOR;
                break;
            default:
                currentScope << " " << token.GetWXString();
                break;
            }
            break;
        case SCP_STATE_IN_FOR:
            switch(token.GetType()) {
            case '(':
                parenthesisDepth++;
                currentScope << "(";
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                if(parenthesisDepth == 0) { state = SCP_STATE_NORMAL; }
                break;
            default:
                break;
            }
            break;
        case SCP_STATE_IN_CATCH:
            switch(token.GetType()) {
            case '(':
                currentScope << "(";
                parenthesisDepth++;
                break;
            case ')':
                parenthesisDepth--;
                currentScope << ")";
                if(parenthesisDepth == 0) { state = SCP_STATE_NORMAL; }
                break;
            default:
                currentScope << " " << token.GetWXString();
                break;
            }
            break;
        default:
            break;
        }
    }

    wxString s;
    while(!scopes.empty()) {
        s.Prepend(scopes.top());
        scopes.pop();
    }
    s << currentScope;
    localsScope = s;
    return s;
}

ParsedToken* Language::ParseTokens(const wxString& scopeName)
{
    wxString token;
    wxString delim;
    bool subscript;
    ParsedToken* header(NULL);
    ParsedToken* currentToken(header);
    wxString funcArgList;

    while(NextToken(token, delim, subscript, funcArgList)) {
        ParsedToken* pt = new ParsedToken;
        pt->SetSubscriptOperator(subscript);
        pt->SetOperator(delim);
        pt->SetPrev(currentToken);
        pt->SetCurrentScopeName(scopeName);
        pt->SetArgumentList(funcArgList);

        ExpressionResult result = ParseExpression(token);
        if(result.m_name.empty() && result.m_isGlobalScope == false) {
            ParsedToken::DeleteTokens(header);
            return NULL;
        }

        if(result.m_isGlobalScope && pt->GetOperator() != wxT("::")) {
            ParsedToken::DeleteTokens(header);
            return NULL;
        }

        if(result.m_isaType) {
            pt->SetTypeScope(result.m_scope.empty() ? wxString(wxT("<global>"))
                                                    : wxString::From8BitData(result.m_scope.c_str()));
            pt->SetTypeName(wxString::From8BitData(result.m_name.c_str()));

        } else if(result.m_isGlobalScope) {
            pt->SetTypeScope(wxT("<global>"));
            pt->SetTypeName(wxT("<global>"));

        } else if(result.m_isThis) {
            //-----------------------------------------
            // special handle for 'this' keyword
            //-----------------------------------------

            pt->SetTypeScope(result.m_scope.empty() ? wxString(wxT("<global>"))
                                                    : wxString::From8BitData(result.m_scope.c_str()));
            if(scopeName == wxT("<global>")) {
                ParsedToken::DeleteTokens(header);
                return NULL;
            }

            if(pt->GetOperator() == wxT("::")) {
                ParsedToken::DeleteTokens(header);
                return NULL;
            }

            if(result.m_isPtr && pt->GetOperator() == wxT(".")) {
                ParsedToken::DeleteTokens(header);
                return NULL;
            }

            if(!result.m_isPtr && pt->GetOperator() == wxT("->")) {
                ParsedToken::DeleteTokens(header);
                return NULL;
            }
            pt->SetTypeName(scopeName);
            pt->SetName(wxT("this"));
        }

        pt->SetIsTemplate(result.m_isTemplate);

        // If the current token is 'this' then the type is actually the
        // current scope
        pt->SetName(_U(result.m_name.c_str()));

        wxArrayString argsList;
        ParseTemplateInitList(wxString::From8BitData(result.m_templateInitList.c_str()), argsList);
        pt->SetTemplateInitialization(argsList);

        if(currentToken == NULL) {
            header = pt;
            currentToken = pt;
        } else {
            currentToken->SetNext(pt);
            currentToken = pt;
        }
        token.Clear();
        delim.Clear();
        subscript = false;
    }

    if(header && header->GetNext() && header->GetName().IsEmpty() && header->GetOperator() == "::") {
        // a chain with more than one token and the first token is simple "::"
        // Delete the first token from the list
        ParsedToken* newHeader = header->GetNext();
        newHeader->SetPrev(NULL);
        wxDELETE(header);
        header = newHeader;
    }
    return header;
}

bool Language::NextToken(wxString& token, wxString& delim, bool& subscriptOperator, wxString& funcArgList)
{
    int depth(0);
    int parenthesisDepth(0);
    bool collectingFuncArgList = true;

    subscriptOperator = false;
    funcArgList.Clear();

    CxxLexerToken tok;
    while(m_tokenScanner.NextToken(tok)) {
        if(parenthesisDepth) {
            switch(tok.GetType()) {
            case '(':
                ++parenthesisDepth;
                if(collectingFuncArgList) { funcArgList << "("; }
                break;
            case ')':
                --parenthesisDepth;
                if(collectingFuncArgList) { funcArgList << ")"; }
                break;
            default:
                if(collectingFuncArgList) { funcArgList << " " << tok.GetWXString(); }
                break;
            }
        } else {
            switch(tok.GetType()) {
            case T_DECLTYPE: {
                if(m_tokenScanner.GetLastToken().GetType() == ',') { token.RemoveLast(); }
                wxString dummy;
                if(m_tokenScanner.ReadUntilClosingBracket(')', dummy)) {
                    m_tokenScanner.NextToken(tok); // Consume the closing parent
                }
                break;
            }
            case T_STATIC_CAST:
            case T_DYNAMIC_CAST:
            case T_REINTERPRET_CAST:
            case T_CONST_CAST: {
                // We expect now: "<"
                wxString txt;
                if(m_tokenScanner.PeekToken(txt) != '<') return false;
                wxString typestr;
                if(!m_tokenScanner.ReadUntilClosingBracket('>', typestr)) return false;
                token.swap(typestr);

                // Consume the closing angle bracket
                m_tokenScanner.NextToken(tok);

                // Peek at the next token
                if(m_tokenScanner.PeekToken(txt) != '(') return false;
                if(!m_tokenScanner.ReadUntilClosingBracket(')', typestr)) return false;
                // Consume the closing parenthessis
                m_tokenScanner.NextToken(tok);
                token.Replace("*", "");
                token.Replace("&", "");
                token.Trim().Trim(false);
                token.Remove(0, 1).RemoveLast();
                token.Trim().Trim(false);
                break;
            }
            case T_THIS:
                token << "this";
                break;
            case T_DOUBLE_COLONS:
            case '.':
            case T_ARROW:
                if(depth == 0) {
                    delim = tok.GetWXString();
                    return true;
                } else {
                    token << " " << tok.GetWXString();
                }
                break;
            case '[':
                subscriptOperator = true;
                depth++;
                token << " " << tok.GetWXString();
                break;
            case '(':
                if(!token.IsEmpty()) {
                    // If the token is empty, we ignore this parenthessis.
                    // The reason is that it is probably from an expression like casting:
                    // (wxClipboard*)
                    parenthesisDepth++;
                    if(collectingFuncArgList) { token << tok.GetWXString(); }
                }
                break;
            case ')':
                // Closing brace on this leve, means that their partner (the open brace)
                // was ignored (see above) so we should ignore this one as well
                break;
            case '<':
            case '{':
                depth++;
                token << " " << tok.GetWXString();
                break;
            case '>':
            case ']':
            case '}':
                depth--;
                token << " " << tok.GetWXString();
                break;
            case T_IDENTIFIER:
            case ',':
            case T_DOUBLE:
            case T_INT:
            case T_STRUCT:
            case T_LONG:
            case T_ENUM:
            case T_CHAR:
            case T_UNION:
            case T_FLOAT:
            case T_SHORT:
            case T_UNSIGNED:
            case T_SIGNED:
            case T_VOID:
            case T_CLASS:
            case T_TYPEDEF:
                token << " " << tok.GetWXString();
                break;
            default:
                break;
            }
        }
    }
    if(token.IsEmpty() == false && depth == 0) {
        if(delim.IsEmpty()) {
            delim = ".";
            return true;
        }
    }
    return false;
}

void Language::SetAutoCompDeliemters(const std::vector<wxString>& delimArr) { m_delimArr = delimArr; }

bool Language::ProcessExpression(const wxString& stmt, const wxString& text, const wxFileName& fn, int lineno,
                                 wxString& typeName,              // output
                                 wxString& typeScope,             // output
                                 wxString& oper,                  // output
                                 wxString& scopeTemplateInitList) // output
{
    bool evaluationSucceeded = true;
    m_templateArgs.clear();

    wxString statement(stmt);

    // Trim whitespace from right and left
    static wxString trimString(_T("{};\r\n\t\v "));

    statement.erase(0, statement.find_first_not_of(trimString));
    statement.erase(statement.find_last_not_of(trimString) + 1);

    wxString lastFuncSig;
    wxString visibleScope, scopeName, localsBody;

    TagEntryPtr tag = GetTagsManager()->FunctionFromFileLine(fn, lineno);
    if(tag) {
        lastFuncSig = tag->GetSignature();
        lastFuncSig.Trim().Trim(false);
        if(!lastFuncSig.IsEmpty() && lastFuncSig[0] == '(') { lastFuncSig.Remove(0, 1); }
        if(!lastFuncSig.IsEmpty() && lastFuncSig[lastFuncSig.size() - 1] == ')') { lastFuncSig.RemoveLast(); }
    }
    wxString textAfterTokensReplacements;
    textAfterTokensReplacements = ApplyCtagsReplacementTokens(text);

    // Parse the local variables once
    const wxStringTable_t& ignoreTokens = GetTagsManager()->GetCtagsOptions().GetTokensWxMap();
    m_locals.clear();
    {
        CxxVariableScanner scanner(textAfterTokensReplacements, eCxxStandard::kCxx11, ignoreTokens, false);
        CxxVariable::Map_t localsMap = scanner.GetVariablesMap();
        m_locals.insert(localsMap.begin(), localsMap.end());
        scanner.OptimizeBuffer(textAfterTokensReplacements, visibleScope);
    }

    if(!lastFuncSig.IsEmpty()) {
        CxxVariableScanner scanner(lastFuncSig, eCxxStandard::kCxx11, ignoreTokens, true);
        CxxVariable::Map_t localsMap = scanner.GetVariablesMap();
        m_locals.insert(localsMap.begin(), localsMap.end());
    }

    std::vector<wxString> additionalScopes;

    scopeName = GetScopeName(visibleScope, &additionalScopes);

    // Always use the global namespace as an addition scope
    // but make sure we add it last
    additionalScopes.push_back(wxT("<global>"));

    SetLastFunctionSignature(lastFuncSig);
    SetVisibleScope(localsBody);
    SetAdditionalScopes(additionalScopes, fn.GetFullPath());

    // get next token using the tokenscanner object
    m_tokenScanner.Reset(statement);

    // By default we keep the head of the list to the top
    // of the chain
    TokenContainer container;

    container.head = ParseTokens(scopeName);
    if(!container.head) { return false; }

    container.current = container.head;

    while(container.current) {

        bool res = ProcessToken(&container);

        if(!res && !container.Rewind()) {
            evaluationSucceeded = false;
            break;

        } else if(!res && container.Rewind()) {
            // ProcessToken() modified the list
            container.SetRewind(false);
            continue;
        }

        container.retries = 0;

        // HACK1: Let the user override the parser decisions
        RunUserTypes(container.current);
        // We call here to IsTypeAndScopeExists which will attempt to provide the best scope / type
        DoIsTypeAndScopeExist(container.current);
        DoExtractTemplateInitListFromInheritance(container.current);

        if(container.current->GetIsTemplate() && container.current->GetTemplateArgList().IsEmpty()) {
            // We got no template declaration...
            container.current->SetTemplateArgList(DoExtractTemplateDeclarationArgs(container.current), m_templateArgs);
        }

        int retryCount(0);
        bool cont(false);
        bool cont2(false);

        do {
            CheckForTemplateAndTypedef(container.current);
            // We check subscript operator only once
            cont = (container.current->GetSubscriptOperator() && OnSubscriptOperator(container.current));
            if(cont) { RunUserTypes(container.current); }
            container.current->SetSubscriptOperator(false);
            cont2 = (container.current->GetOperator() == wxT("->") && OnArrowOperatorOverloading(container.current));
            if(cont2) { RunUserTypes(container.current); }
            retryCount++;
        } while((cont || cont2) && retryCount < 5);

        // Update the results we got so far
        typeName = container.current->GetTypeName();
        typeScope = container.current->GetTypeScope();

        // Keep the last operator used, it is required by the caller
        oper = container.current->GetOperator();

        container.current = container.current->GetNext();
    }

    // release the tokens
    ParsedToken::DeleteTokens(container.head);
    return evaluationSucceeded;
}

bool Language::OnTemplates(ParsedToken* token)
{
    token->ResolveTemplateType(GetTagsManager());
    return token->ResovleTemplate(GetTagsManager());
}

void Language::DoSimpleTypedef(ParsedToken* token)
{
    // If the match is typedef, try to replace it with the actual
    // typename
    std::vector<TagEntryPtr> tags;
    std::vector<TagEntryPtr> filteredTags;
    wxString path;

    GetTagsManager()->FindByPath(token->GetPath(), tags);

    // try to remove all tags that are Macros from this list
    for(size_t i = 0; i < tags.size(); i++) {
        if(!tags.at(i)->IsMacro()) { filteredTags.push_back(tags.at(i)); }
    }

    if(filteredTags.size() == 1) {
        // we have a single match, test to see if it a typedef
        TagEntryPtr tag = filteredTags.at(0);
        wxString tmpInitList;

        wxString realName = tag->NameFromTyperef(tmpInitList);
        if(realName.IsEmpty() == false) {
            token->SetTypeName(realName);
            token->SetTypeScope(tag->GetScope());

            // incase the realName already includes the scope, remove it from the typename
            token->RemoveScopeFromType();
        }
    }
}

bool Language::OnTypedef(ParsedToken* token)
{
    // If the match is typedef, try to replace it with the actual
    // typename
    bool res(false);
    std::vector<TagEntryPtr> tags;
    std::vector<TagEntryPtr> filteredTags;
    wxString path;
    TagsManager* tagsManager = GetTagsManager();

    wxString oldName = token->GetTypeName();
    wxString oldScope = token->GetTypeScope();

    tagsManager->FindByPath(token->GetPath(), tags);

    // try to remove all tags that are Macros from this list
    for(size_t i = 0; i < tags.size(); i++) {
        if(!tags.at(i)->IsMacro()) { filteredTags.push_back(tags.at(i)); }
    }

    if(filteredTags.size() == 1) {
        // We have a single match, test to see if it a typedef
        TagEntryPtr tag = filteredTags.at(0);
        wxString tmpInitList;

        wxString realName = tag->NameFromTyperef(tmpInitList);
        if(realName.IsEmpty() == false) {

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

    if(filteredTags.empty()) {
        // this is yet another attempt to fix a match which we failed to resolve it completly
        // a good example for such case is using a typedef which was defined inside a function
        // body

        // try to locate any typedefs defined locally
        clTypedefList typedefsList;
        const wxCharBuffer buf = _C(GetVisibleScope());
        get_typedefs(buf.data(), typedefsList);

        if(typedefsList.empty() == false) {
            // take the first match
            clTypedefList::iterator iter = typedefsList.begin();
            for(; iter != typedefsList.end(); iter++) {
                clTypedef td = *iter;
                wxString matchName(td.m_name.c_str(), wxConvUTF8);
                if(matchName == token->GetTypeName()) {
                    wxArrayString scopeTempalteInitList;
                    wxString tmpInitList;

                    token->SetTypeName(wxString(td.m_realType.m_type.c_str(), wxConvUTF8));
                    token->SetTypeScope(wxString(td.m_realType.m_typeScope.c_str(), wxConvUTF8));
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

void Language::ParseTemplateArgs(const wxString& argListStr, wxArrayString& argsList)
{
    CppScanner scanner;
    scanner.SetText(_C(argListStr));
    int type = scanner.yylex();
    wxString word = _U(scanner.YYText());

    // Eof?
    if(type == 0) { return; }
    if(type != (int)'<') { return; }

    bool nextIsArg(false);
    bool cont(true);
    while(cont) {
        type = scanner.yylex();
        if(type == 0) { break; }

        switch(type) {
        case lexCLASS:
        case IDENTIFIER: {
            wxString word = _U(scanner.YYText());
            if(word == wxT("class") || word == wxT("typename")) {
                nextIsArg = true;

            } else if(nextIsArg) {
                argsList.Add(word);
                nextIsArg = false;
            }
            break;
        }
        case(int)'>':
            cont = false;
            break;
        default:
            break;
        }
    }
}

void Language::ParseTemplateInitList(const wxString& argListStr, wxArrayString& argsList)
{
    CppScanner scanner;
    scanner.SetText(_C(argListStr));
    int type = scanner.yylex();
    wxString word = _U(scanner.YYText());

    // Eof?
    if(type == 0) { return; }
    if(type != (int)'<') { return; }

    int depth(1);
    wxString typeName;
    while(depth > 0) {
        type = scanner.yylex();
        if(type == 0) { break; }

        switch(type) {
        case(int)',': {
            if(depth == 1) {
                argsList.Add(typeName.Trim().Trim(false));
                typeName.Empty();
            }
            break;
        }
        case(int)'>':
            depth--;
            break;
        case(int)'<':
            depth++;
            break;
        case(int)'*':
        case(int)'&':
            // ignore pointers & references
            break;
        default:
            if(depth == 1) { typeName << _U(scanner.YYText()); }
            break;
        }
    }

    if(typeName.Trim().Trim(false).IsEmpty() == false) { argsList.Add(typeName.Trim().Trim(false)); }
    typeName.Empty();
}

void Language::ParseComments(const wxFileName& fileName, std::vector<CommentPtr>* comments)
{
    wxString content;
    try {
        wxFFile f(fileName.GetFullPath().GetData());
        if(!f.IsOpened()) return;

        // read the content of the file and parse it
        f.ReadAll(&content);
        f.Close();
    } catch(...) {
        return;
    }

    m_scanner->Reset();
    m_scanner->SetText(_C(content));
    m_scanner->KeepComment(1);

    int type(0);

    wxString comment(_T(""));
    int line(-1);

    while(true) {
        type = m_scanner->yylex();
        if(type == 0) // eof
            break;

        // we keep only comments
        if(type == CPPComment) {
            // incase the previous comment was one line above this one,
            // concatenate them to a single comment
            if(m_scanner->lineno() - 1 == line) {
                comment << m_scanner->GetComment();
                line = m_scanner->lineno();
                m_scanner->ClearComment();
                continue;
            }

            // save the previous comment buffer
            if(comment.IsEmpty() == false) {
                comments->push_back(new Comment(comment, fileName.GetFullPath(), line - 1));
                comment.Empty();
                line = -1;
            }

            // first time or no comment is buffer
            if(comment.IsEmpty()) {
                comment = m_scanner->GetComment();
                line = m_scanner->lineno();
                m_scanner->ClearComment();
                continue;
            }

            comments->push_back(new Comment(m_scanner->GetComment(), fileName.GetFullPath(), m_scanner->lineno() - 1));
            comment.Empty();
            line = -1;
            m_scanner->ClearComment();

        } else if(type == CComment) {
            comments->push_back(new Comment(m_scanner->GetComment(), fileName.GetFullPath(), m_scanner->lineno()));
            m_scanner->ClearComment();
        }
    }

    if(comment.IsEmpty() == false) { comments->push_back(new Comment(comment, fileName.GetFullPath(), line - 1)); }

    // reset the scanner
    m_scanner->KeepComment(0);
    m_scanner->Reset();
}

wxString Language::GetScopeName(const wxString& in, std::vector<wxString>* additionlNS)
{
    std::vector<std::string> moreNS;

    const wxCharBuffer buf = _C(in);

    TagsManager* mgr = GetTagsManager();
    std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

    std::string scope_name = get_scope_name(buf.data(), moreNS, ignoreTokens);
    wxString scope = _U(scope_name.c_str());
    if(scope.IsEmpty()) { scope = wxT("<global>"); }

    if(additionlNS) {
        for(size_t i = 0; i < moreNS.size(); i++) {
            additionlNS->push_back(_U(moreNS.at(i).c_str()));
        }

        // In case we are found some 'using namesapce XXX;' statement
        // we should scan the following scopes:
        // XXX
        // and also:
        // XXX::CurrentScope (assuming that CurrentScope != <global>)
        if(scope != wxT("<global>")) {
            std::vector<wxString> tmpScopes;
            for(size_t i = 0; i < additionlNS->size(); i++) {
                tmpScopes.push_back(additionlNS->at(i));
                tmpScopes.push_back(additionlNS->at(i) + wxT("::") + scope);
            }
            additionlNS->clear();
            additionlNS->insert(additionlNS->begin(), tmpScopes.begin(), tmpScopes.end());
        }

        wxArrayString moreScopes = GetTagsManager()->BreakToOuterScopes(scope);
        for(size_t i = 0; i < moreScopes.GetCount(); i++) {
            if(moreScopes.Item(i) != scope &&
               std::find(additionlNS->begin(), additionlNS->end(), moreScopes.Item(i)) == additionlNS->end()) {
                additionlNS->push_back(moreScopes.Item(i));
            }
        }
    }

    return scope;
}

ExpressionResult Language::ParseExpression(const wxString& in)
{
    ExpressionResult result;
    if(in.IsEmpty()) {
        result.m_isGlobalScope = true;

    } else {
        const wxCharBuffer buf = _C(in);
        result = parse_expression(buf.data());
    }
    return result;
}

bool Language::ProcessToken(TokenContainer* tokeContainer)
{
    // first we try to match the current scope
    std::vector<TagEntryPtr> tags;

    wxString type;
    wxString typeScope;
    ParsedToken* token = tokeContainer->current;

    // Handle 'this'
    if(token->IsThis()) {
        token->SetTypeName(token->GetContextScope());
        token->SetTypeScope(wxT("<global>"));
        return true;
    }

    // Since locals should take precedence over globals/memebers
    // we test the local scope first
    if(token->GetPrev() == NULL) {
        // We are the first token in the chain
        // Check the locals table
        CxxVariable::Ptr_t pVar = FindLocalVariable(token->GetName());

        // Search for a full match in the returned list
        if(pVar) {
            if(pVar->IsAuto()) {
                tokeContainer->current->SetIsAutoVariable(true);
                tokeContainer->current->SetAutoExpression(pVar->GetDefaultValue());
                DoFixTokensFromVariable(tokeContainer, tokeContainer->current->GetAutoExpression());

            } else {
                DoFixTokensFromVariable(tokeContainer, pVar->GetTypeAsCxxString());
            }
            return false;
        }
    }

    if(RunUserTypes(token, token->GetContextScope() + "::" + token->GetName())) { return true; }

    // Try the lookup tables
    bool hasMatch = DoSearchByNameAndScope(token->GetName(), token->GetContextScope(), tags, type, typeScope);
    if(!hasMatch && token->GetPrev() == NULL) {
        // failed to find it in the local scope and in the lookup table
        // try the additional scopes
        for(size_t i = 0; i < GetAdditionalScopes().size(); i++) {
            tags.clear();
            hasMatch = DoSearchByNameAndScope(token->GetName(), GetAdditionalScopes().at(i), tags, type, typeScope);
            if(hasMatch) { break; }
        }

        if(!hasMatch) {
            // Try macros
            PPToken tok = GetTagsManager()->GetDatabase()->GetMacro(token->GetName());
            if(tok.flags & PPToken::IsValid) {
                // we got a match in the macros DB
                if(tok.flags & PPToken::IsFunctionLike) {
                    // Handle function like macros
                    wxString initList = token->GetArgumentList();
                    if(initList.StartsWith(wxT("("))) { initList.Remove(0, 1); }

                    if(initList.EndsWith(wxT(")"))) { initList.RemoveLast(); }

                    wxArrayString initListArr = wxStringTokenize(initList, wxT(","), wxTOKEN_STRTOK);
                    tok.expandOnce(initListArr);
                }

                DoFixTokensFromVariable(tokeContainer, tok.replacement);
                return false;
            }
        }
    }

    if(hasMatch && !tags.empty()) {
        if(token->GetPrev() == NULL) {

            // we are first in the chain, but still we exists in the database
            // this means that we are either a scope followed by operator (e.g. Foo::)
            // or global variable or member
            // for the last two cases, we need to handle this as if we did not find a match in the
            // database
            TagEntryPtr tag = tags.at(0);
            if(tag->GetKind() == wxT("member") || tag->GetKind() == wxT("variable")) {
                CxxVariable::Ptr_t pVar = FindVariableInScope(tags.at(0)->GetPattern(), tags.at(0)->GetName());
                if(pVar) {
                    if(pVar->GetName() == tags.at(0)->GetName()) {
                        DoFixTokensFromVariable(tokeContainer, pVar->GetTypeAsCxxString());
                    }
                }
                return false;
            }

        } else {

            // if we are a "member" or " variable"
            // try to locate the template initialization list
            bool isTyperef = !tags.at(0)->GetTyperef().IsEmpty();

            TagEntryPtr tag = tags.at(0);
            if(!isTyperef && (tags.at(0)->GetKind() == wxT("member") || tags.at(0)->GetKind() == wxT("variable"))) {
                CxxVariable::Ptr_t pVar = FindVariableInScope(tag->GetPattern(), tags.at(0)->GetName());
                // Search for a full match in the returned list
                if(pVar) {
                    ExpressionResult expRes = ParseExpression(pVar->GetTypeAsCxxString());
                    if(expRes.m_isTemplate) {
                        token->SetIsTemplate(expRes.m_isTemplate);

                        wxArrayString argsList;
                        ParseTemplateInitList(wxString::From8BitData(expRes.m_templateInitList.c_str()), argsList);
                        token->SetTemplateInitialization(argsList);
                    }
                }

            } else if(tag->IsTemplateFunction()) {
                // Parse the definition list
                CxxTemplateFunction ctf(tag);
                ctf.ParseDefinitionList();

                if(!ctf.GetList().IsEmpty()) {
                    token->SetIsTemplate(true);
                    token->SetTemplateArgList(ctf.GetList(), m_templateArgs);
                }
            }
            // fall through...
        }

        // we got a match
        token->SetTypeName(type);
        token->SetTypeScope(typeScope);

        return DoCorrectUsingNamespaces(token, tags);
    }
    return false;
}

bool Language::CorrectUsingNamespace(wxString& type, wxString& typeScope, const wxString& parentScope,
                                     std::vector<TagEntryPtr>& tags)
{
    wxString strippedScope(typeScope);
    wxArrayString tmplInitList;
    DoRemoveTempalteInitialization(strippedScope, tmplInitList);

    if(typeScope == wxT("<global>") && GetAdditionalScopes().empty() == false) {
        // Incase the typeScope is "global" and we got additional-scopes
        // Use the additional scopes *before* the "global" scope
        for(size_t i = 0; i < GetAdditionalScopes().size(); i++) {
            tags.clear();
            wxString newScope(GetAdditionalScopes().at(i));
            if(typeScope != wxT("<global>")) { newScope << wxT("::") << typeScope; }

            if(DoSearchByNameAndScope(type, newScope, tags, type, typeScope)) { return true; }
        }
    }

    // try the passed scope (might be <global> now)
    if(GetTagsManager()->IsTypeAndScopeExists(type, strippedScope)) { return true; }

    // if we are here, it means that the more scopes did not matched any, try the parent scope
    tags.clear();

    // try all the scopes of the parent:
    // for example:
    // assuming the parent scope is A::B::C
    // try to match:
    // A::B::C
    // A::B
    // A
    wxArrayString scopesToScan = GetTagsManager()->BreakToOuterScopes(parentScope);
    scopesToScan.Add(wxT("<global>"));
    for(size_t i = 0; i < scopesToScan.GetCount(); i++) {
        tags.clear();
        if(DoSearchByNameAndScope(type, scopesToScan.Item(i), tags, type, typeScope, false)) { return true; }
    }
    return true;
}

bool Language::DoSearchByNameAndScope(const wxString& name, const wxString& scopeName, std::vector<TagEntryPtr>& tags,
                                      wxString& type, wxString& typeScope, bool testGlobalScope)
{
    PERF_BLOCK("DoSearchByNameAndScope")
    {
        std::vector<TagEntryPtr> tmp_tags;
        GetTagsManager()->FindByNameAndScope(name, scopeName, tmp_tags);
        if(tmp_tags.empty() && testGlobalScope) {
            // try the global scope maybe?
            GetTagsManager()->FindByNameAndScope(name, wxT("<global>"), tmp_tags);
        }

        // filter macros from the result
        for(size_t i = 0; i < tmp_tags.size(); i++) {
            TagEntryPtr t = tmp_tags.at(i);
            if(t->GetKind() != wxT("macro") && !t->IsConstructor()) { tags.push_back(t); }
        }

        if(tags.size() == 1) {
            TagEntryPtr tag(tags.at(0));
            // we have a single match!
            if(tag->IsMethod()) {

                clFunction foo;
                if(FunctionFromPattern(tag, foo)) {
                    type = _U(foo.m_returnValue.m_type.c_str());

                    // Guess the return value scope:
                    // if we got scope, use it
                    if(foo.m_returnValue.m_typeScope.empty() == false)
                        typeScope = _U(foo.m_returnValue.m_typeScope.c_str());

                    else {

                        // we got no scope to use.
                        // try the wxT("<global>") scope
                        typeScope = wxT("<global>");
                        if(!GetTagsManager()->GetDatabase()->IsTypeAndScopeExistLimitOne(type, typeScope)) {
                            // try the current scope
                            typeScope = scopeName;
                        }
                        // TODO: continue to scan the entire 'using namespaces' stack
                    }
                    return true;
                }

                return false;

            } else if(tag->GetKind() == wxT("member") || tag->GetKind() == wxT("variable")) {

                if(tag->GetKind() == wxT("member") && !tag->GetTyperef().IsEmpty()) {
                    // Incase the tag is of type 'member' AND the name is different than
                    // the typeref, we are actually dealing here with a 'using directive' entry
                    // in this case, the fully qualified name is tag->NameFromTyperef()
                    wxString dummy;
                    wxString typeRef = tag->NameFromTyperef(dummy);

                    type = NameFromPath(typeRef);
                    typeScope = ScopeFromPath(typeRef);
                    return true;
                }

                Variable var;
                if(VariableFromPattern(tag->GetPattern(), tag->GetName(), var)) {
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
        } else if(tags.size() > 1) {

            // if list contains more than one entry, check if all entries are of type 'function' or 'prototype'
            // (they can be mixed). If all entries are of one of these types, test their return value,
            // if all have the same return value, then we are ok
            clFunction foo;
            int classMatches(0);
            size_t classMatchIdx(0);

            for(size_t i = 0; i < tags.size(); i++) {
                TagEntryPtr tag(tags.at(i));
                if(!FunctionFromPattern(tag, foo)) { break; }

                type = _U(foo.m_returnValue.m_type.c_str());
                typeScope =
                    foo.m_returnValue.m_typeScope.empty() ? tag->GetScope() : _U(foo.m_returnValue.m_typeScope.c_str());
                if(type != wxT("void")) { return true; }
            }

            // Dont give up yet!
            // If in the list of matches there is a single entry of type class
            // use it as our match
            for(size_t i = 0; i < tags.size(); i++) {
                if(tags.at(i)->IsClass() || tags.at(i)->IsStruct()) {
                    classMatches++;
                    classMatchIdx = i;
                }
            }

            if(classMatches == 1) {
                TagEntryPtr tag = tags.at(classMatchIdx);
                tags.clear();
                tags.push_back(tag);

                type = tag->GetName();
                typeScope = tag->GetScopeName();
                return true;
            }

            return false;
        }
    }
    return false;
}

bool Language::VariableFromPattern(const wxString& in, const wxString& name, Variable& var)
{
    VariableList li;
    wxString pattern(in);
    // we need to extract the return value from the pattern
    pattern = pattern.BeforeLast(wxT('$'));
    pattern = pattern.AfterFirst(wxT('^'));

    const wxCharBuffer patbuf = _C(pattern);
    li.clear();

    TagsManager* mgr = GetTagsManager();
    std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

    get_variables(patbuf.data(), li, ignoreTokens, false);
    VariableList::iterator iter = li.begin();
    for(; iter != li.end(); iter++) {
        Variable v = *iter;
        if(name == _U(v.m_name.c_str())) {
            var = (*iter);
            return true;
        }
    } // if(li.size() == 1)
    return false;
}

bool Language::FunctionFromPattern(TagEntryPtr tag, clFunction& foo)
{
    FunctionList fooList;
    wxString pattern(tag->GetPattern());
    // we need to extract the return value from the pattern
    pattern = pattern.BeforeLast(wxT('$'));
    pattern = pattern.AfterFirst(wxT('^'));

    pattern = pattern.Trim();
    pattern = pattern.Trim(false);
    if(pattern.EndsWith(wxT(";"))) { pattern = pattern.RemoveLast(); }

    // remove any comments from the pattern
    wxString tmp_pattern(pattern);
    pattern.Empty();
    GetTagsManager()->StripComments(tmp_pattern, pattern);

    // a limitiation of the function parser...
    pattern << wxT(';');

    TagsManager* mgr = GetTagsManager();
    std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

    // use the replacement table on the pattern before processing it
    DoReplaceTokens(pattern, GetTagsManager()->GetCtagsOptions().GetTokensWxMap());

    const wxCharBuffer patbuf = _C(pattern);
    get_functions(patbuf.data(), fooList, ignoreTokens);
    if(fooList.size() == 1) {
        foo = (*fooList.begin());
        DoFixFunctionUsingCtagsReturnValue(foo, tag);
        return true;

    } else if(fooList.size() == 0) {
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
        if(fooList.size() == 1) {
            foo = (*fooList.begin());
            DoFixFunctionUsingCtagsReturnValue(foo, tag);
            return true;

        } else if(fooList.empty()) {
            // try a nasty hack:
            // the yacc cant find ctor declarations
            // so add a 'void ' infront of the function...
            wxString pat_tag(pattern);
            pat_tag = pat_tag.Trim(false).Trim();
            wxString pat3;
            bool dummyReturnValue(true);

            // failed to parse function.
            if(tag->GetReturnValue().IsEmpty() == false && !(tag->IsConstructor() || tag->IsDestructor())) {
                pat3 = pat_tag;
                pat3.Prepend(tag->GetReturnValue() + wxT(" "));
                dummyReturnValue = false;

            } else {
                // consider virtual methods as well
                bool virt(false);
                virt = pat_tag.StartsWith(wxT("virtual"), &pat3);
                if(virt) {
                    pat3.Prepend(wxT("void "));
                    pat3.Prepend(wxT("virtual "));
                } else {
                    pat3 = pat_tag;
                    pat3.Prepend(wxT("void "));
                }
            }
            const wxCharBuffer patbuf2 = _C(pat3);
            get_functions(patbuf2.data(), fooList, ignoreTokens);
            if(fooList.size() == 1) {
                foo = (*fooList.begin());

                if(dummyReturnValue) foo.m_returnValue.Reset(); // clear the dummy return value
                return true;
            }
        }
    }
    return false;
}

void Language::GetLocalVariables(const wxString& in, std::vector<TagEntryPtr>& tags, bool isFuncSignature,
                                 const wxString& name, size_t flags)
{
    wxString pattern(in);
    pattern = pattern.Trim().Trim(false);

    if(flags & ReplaceTokens) {
        // Apply ctags replcements table on the current input string
        pattern = ApplyCtagsReplacementTokens(in);
    }

    CxxVariableScanner scanner(pattern, eCxxStandard::kCxx11, GetTagsManager()->GetCtagsOptions().GetTokensWxMap(),
                               isFuncSignature);
    CxxVariable::Vec_t locals = scanner.GetVariables(false);

    std::for_each(locals.begin(), locals.end(), [&](CxxVariable::Ptr_t local) {
        wxString tagName = local->GetName();

        // if we have name, collect only tags that matches name
        if(!name.IsEmpty()) {
            // incase CaseSensitive is not required, make both string lower case
            wxString tmpName(name);
            wxString tmpTagName(tagName);
            if(flags & IgnoreCaseSensitive) {
                tmpName.MakeLower();
                tmpTagName.MakeLower();
            }

            if((flags & PartialMatch) && !tmpTagName.StartsWith(tmpName)) return;
            // Don't suggest what we have typed so far
            if((flags & PartialMatch) && tmpTagName == tmpName) return;
            if((flags & ExactMatch) && tmpTagName != tmpName) return;
        } // else no name is specified, collect all tags

        TagEntryPtr tag(new TagEntry());
        tag->SetName(tagName);
        tag->SetKind(wxT("variable"));
        tag->SetParent(wxT("<local>"));
        tag->SetScope(local->GetTypeAsCxxString());
        tag->SetAccess("public");
        tag->SetPattern(local->GetTypeAsCxxString() + " " + local->GetName());
        tags.push_back(tag);
    });
}

bool Language::OnArrowOperatorOverloading(ParsedToken* token)
{
    bool ret(false);
    // collect all functions of typename
    std::vector<TagEntryPtr> tags;
    wxString typeScope(token->GetTypeScope());
    wxString typeName(token->GetTypeName());

    // this function will retrieve the ineherited tags as well
    GetTagsManager()->GetDereferenceOperator(token->GetPath(), tags);

    if(tags.size() == 1) {
        // loop over the tags and scan for operator -> overloading
        // we found our overloading operator
        // extract the 'real' type from the pattern
        clFunction f;
        if(FunctionFromPattern(tags.at(0), f)) {

            typeName = _U(f.m_returnValue.m_type.c_str());
            typeScope =
                f.m_returnValue.m_typeScope.empty() ? token->GetPath() : _U(f.m_returnValue.m_typeScope.c_str());

            token->SetTypeName(typeName);
            token->SetTypeScope(typeScope);

            // Call the magic method that fixes typename/typescope
            DoIsTypeAndScopeExist(token);
            ret = true;
        }
    }
    return ret;
}

void Language::SetTagsManager(TagsManager* tm) { m_tm = tm; }

TagsManager* Language::GetTagsManager()
{
    if(!m_tm) {
        // for backward compatibility allows access to the tags manager using
        // the singleton call
        return TagsManagerST::Get();
    } else {
        return m_tm;
    }
}

void Language::DoRemoveTempalteInitialization(wxString& str, wxArrayString& tmplInitList)
{
    CppScanner sc;
    sc.SetText(_C(str));

    int type(0);
    int depth(0);

    wxString token;
    wxString outputString;
    str.Clear();

    while((type = sc.yylex()) != 0) {
        if(type == 0) return;

        token = _U(sc.YYText());
        switch(type) {
        case wxT('<'):
            if(depth == 0) outputString.Clear();
            outputString << token;
            depth++;
            break;

        case wxT('>'):
            outputString << token;
            depth--;
            break;

        default:
            if(depth > 0)
                outputString << token;
            else
                str << token;
            break;
        }
    }

    if(outputString.IsEmpty() == false) { ParseTemplateInitList(outputString, tmplInitList); }
}

void Language::DoFixFunctionUsingCtagsReturnValue(clFunction& foo, TagEntryPtr tag)
{
    if(foo.m_returnValue.m_type.empty()) {

        // Use the CTAGS return value
        wxString ctagsRetValue = tag->GetReturnValue();
        DoReplaceTokens(ctagsRetValue, GetTagsManager()->GetCtagsOptions().GetTokensWxMap());

        const wxCharBuffer cbuf = ctagsRetValue.mb_str(wxConvUTF8);
        std::map<std::string, std::string> ignoreTokens = GetTagsManager()->GetCtagsOptions().GetTokensMap();

        VariableList li;
        get_variables(cbuf.data(), li, ignoreTokens, false);
        if(li.size() == 1) { foo.m_returnValue = *li.begin(); }
    }
}

void Language::DoReplaceTokens(wxString& inStr, const wxStringTable_t& ignoreTokens)
{
    if(inStr.IsEmpty()) return;

    wxStringTable_t::const_iterator iter = ignoreTokens.begin();
    for(; iter != ignoreTokens.end(); iter++) {
        wxString findWhat = iter->first;
        wxString replaceWith = iter->second;

        if(findWhat.StartsWith(wxT("re:"))) {
            findWhat.Remove(0, 3);
            wxRegEx re(findWhat);
            if(re.IsValid() && re.Matches(inStr)) { re.ReplaceAll(&inStr, replaceWith); }
        } else {
            // Simple replacement
            int where = inStr.Find(findWhat);
            if(where >= 0) {
                if(inStr.Length() > static_cast<size_t>(where)) {
                    // Make sure that the next char is a non valid char otherwise this is not a complete word
                    if(inStr.Mid(where, 1).find_first_of(
                           wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890")) != wxString::npos) {
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

void Language::CheckForTemplateAndTypedef(ParsedToken* token)
{
    bool typedefMatch;
    bool templateMatch;
    int retry(0);

    do {
        typedefMatch = OnTypedef(token);
        if(typedefMatch) { RunUserTypes(token); }

        // Attempt to fix the result
        DoIsTypeAndScopeExist(token);

        if(typedefMatch) {
            DoExtractTemplateInitListFromInheritance(token);

            // The typeName was a typedef, so make sure we update the template declaration list
            // with the actual type
            std::vector<TagEntryPtr> tags;
            GetTagsManager()->FindByPath(token->GetPath(), tags);
            if(tags.size() == 1 && !tags.at(0)->IsTypedef()) {

                // Not a typedef
                token->SetTemplateArgList(DoExtractTemplateDeclarationArgs(tags.at(0)), m_templateArgs);
                token->SetIsTemplate(token->GetTemplateArgList().IsEmpty() == false);

            } else if(tags.size() == 1) {

                // Typedef
                TagEntryPtr t = tags.at(0);
                wxString pattern(t->GetPattern());
                wxArrayString tmpInitList;
                DoRemoveTempalteInitialization(pattern, tmpInitList);

                // Incase any of the template initialization list is a
                // typedef, resolve it as well
                DoResolveTemplateInitializationList(tmpInitList);
                token->SetTemplateInitialization(tmpInitList);
            }
        }

        templateMatch = OnTemplates(token);
        if(templateMatch) {
            if(!DoIsTypeAndScopeExist(token)) {
                std::vector<TagEntryPtr> dummyTags;
                DoCorrectUsingNamespaces(token, dummyTags);
            }
            token->SetIsTemplate(false);
            DoExtractTemplateInitListFromInheritance(token);
        }

        if(templateMatch) { RunUserTypes(token); }
        retry++;

    } while((typedefMatch || templateMatch) && retry < 15);
}

void Language::DoResolveTemplateInitializationList(wxArrayString& tmpInitList)
{
    for(size_t i = 0; i < tmpInitList.GetCount(); i++) {
        wxString fixedTemplateArg;
        wxString name = NameFromPath(tmpInitList.Item(i));

        wxString tmpScope = ScopeFromPath(tmpInitList.Item(i));
        wxString scope = tmpScope == wxT("<global>") ? m_templateHelper.GetPath() : tmpScope;

        ParsedToken tok;
        tok.SetTypeName(name);
        tok.SetTypeScope(scope);

        DoSimpleTypedef(&tok);

        name = tok.GetTypeName();
        scope = tok.GetTypeScope();

        if(GetTagsManager()->GetDatabase()->IsTypeAndScopeExistLimitOne(name, scope) == false) {
            // no match, assume template: NAME only
            tmpInitList.Item(i) = name;
        } else
            tmpInitList.Item(i) = PathFromNameAndScope(name, scope);
    }
}

wxArrayString Language::DoExtractTemplateDeclarationArgs(ParsedToken* token)
{
    // Find a tag in the database that matches this find and
    // extract the template declaration for it
    std::vector<TagEntryPtr> tags;
    GetTagsManager()->FindByPath(token->GetPath(), tags);
    if(tags.size() != 1) return wxArrayString();

    return DoExtractTemplateDeclarationArgs(tags.at(0));
}

wxArrayString Language::DoExtractTemplateDeclarationArgsFromScope()
{
    wxString tmpParentScope(m_templateHelper.GetTypeScope());
    wxString cuttedScope(tmpParentScope);

    tmpParentScope.Replace(wxT("::"), wxT("@"));
    std::vector<TagEntryPtr> tags;

    cuttedScope.Trim().Trim(false);
    while(!cuttedScope.IsEmpty()) {

        // try all the scopes of thse parent:
        // for example:
        // assuming the parent scope is A::B::C
        // try to match:
        // A::B::C
        // A::B
        // A
        tags.clear();
        GetTagsManager()->FindByPath(cuttedScope, tags);
        if(tags.size() == 1) {
            if(tags.at(0)->GetPattern().Contains(wxT("template"))) {
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

    // extract the template declartion list
    CppScanner declScanner;
    declScanner.ReturnWhite(1);
    declScanner.SetText(_C(pattern));
    bool foundTemplate(false);
    int type(0);
    while(true) {
        type = declScanner.yylex();
        if(type == 0) // eof
            break;

        wxString word = _U(declScanner.YYText());
        switch(type) {
        case IDENTIFIER:
            if(word == wxT("template")) {
                foundTemplate = true;

            } else if(foundTemplate) {
                templateString << word;
            }
            break;

        default:
            if(foundTemplate) { templateString << word; }
            break;
        }
    }

    if(foundTemplate) {
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

void TemplateHelper::SetTemplateDeclaration(const wxString& templateDeclaration)
{
    LanguageST::Get()->ParseTemplateArgs(templateDeclaration, this->templateDeclaration);
}

void TemplateHelper::SetTemplateInstantiation(const wxString& tempalteInstantiation)
{
    this->templateInstantiationVector.clear();
    wxArrayString l;
    LanguageST::Get()->ParseTemplateInitList(tempalteInstantiation, l);
    this->templateInstantiationVector.push_back(l);
}

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
    for(size_t i = 0; i < newInstantiationList.GetCount(); i++) {
        int where = this->templateDeclaration.Index(newInstantiationList.Item(i));
        if(where != wxNOT_FOUND) {
            wxString name = Substitute(newInstantiationList.Item(i));
            if(!name.IsEmpty()) newInstantiationList[i] = name;
        }
    }

    templateInstantiationVector.push_back(newInstantiationList);
}

wxString TemplateHelper::Substitute(const wxString& name)
{
    //	for(size_t i=0; i<templateInstantiationVector.size(); i++) {
    int count = static_cast<int>(templateInstantiationVector.size());
    for(int i = count - 1; i >= 0; i--) {
        int where = templateDeclaration.Index(name);
        if(where != wxNOT_FOUND) {
            // it exists, return the name in the templateInstantiation list
            if(templateInstantiationVector.at(i).GetCount() > (size_t)where &&
               templateInstantiationVector.at(i).Item(where) != name)
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
    if(typeScope != wxT("<global>")) path << typeScope << wxT("::");

    path << typeName;
    return path;
}

void Language::SetAdditionalScopes(const std::vector<wxString>& additionalScopes, const wxString& filename)
{
    if(!(GetTagsManager()->GetCtagsOptions().GetFlags() & CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING)) {
        this->m_additionalScopes = additionalScopes;

    } else {
        this->m_additionalScopes.clear();
        // Use the cache to get the list of using namespaces.
        // The cache is populated by the CodeCompletionManager worker
        // thread when the file is loaded
        std::map<wxString, std::vector<wxString> >::iterator iter = m_additionalScopesCache.find(filename);
        if(iter != m_additionalScopesCache.end()) { this->m_additionalScopes = iter->second; }

        // "using namespace" may not contains current namespace, so make sure we add it
        for(size_t i = 0; i < additionalScopes.size(); i++) {
            if(!(std::find(this->m_additionalScopes.begin(), this->m_additionalScopes.end(), additionalScopes.at(i)) !=
                 this->m_additionalScopes.end())) {
                this->m_additionalScopes.push_back(additionalScopes.at(i));
            }
        }
    }
}

const std::vector<wxString>& Language::GetAdditionalScopes() const { return m_additionalScopes; }

bool Language::OnSubscriptOperator(ParsedToken* token)
{
    bool ret(false);
    // collect all functions of typename
    std::vector<TagEntryPtr> tags;
    wxString scope;
    wxString typeName(token->GetTypeName());
    wxString typeScope(token->GetTypeScope());

    if(typeScope == wxT("<global>"))
        scope << token->GetTypeName();
    else
        scope << token->GetTypeScope() << wxT("::") << token->GetTypeName();

    // this function will retrieve the ineherited tags as well
    GetTagsManager()->GetSubscriptOperator(scope, tags);
    if(tags.size() == 1) {
        // we found our overloading operator
        // extract the 'real' type from the pattern
        clFunction f;
        if(FunctionFromPattern(tags.at(0), f)) {
            token->SetTypeName(_U(f.m_returnValue.m_type.c_str()));

            // first assume that the return value has the same scope like the parent (unless the return value has a
            // scope)
            token->SetTypeScope(f.m_returnValue.m_typeScope.empty() ? scope : _U(f.m_returnValue.m_typeScope.c_str()));

            // Call the magic method that fixes typename/typescope
            DoIsTypeAndScopeExist(token);
            ret = true;
        }
    }
    return ret;
}

bool Language::RunUserTypes(ParsedToken* token, const wxString& entryPath)
{
    wxStringTable_t typeMap = GetTagsManager()->GetCtagsOptions().GetTypesMap();
    // HACK1: Let the user override the parser decisions
    wxString path = entryPath.IsEmpty() ? token->GetPath() : entryPath;
    wxStringTable_t::const_iterator where = typeMap.find(path);
    if(where != typeMap.end()) {
        wxArrayString argList;

        // Split to name and scope
        wxString name, scope;

        scope = where->second.BeforeFirst(wxT('<'));
        name = scope.AfterLast(wxT(':'));
        scope = scope.BeforeLast(wxT(':'));
        if(scope.EndsWith(wxT(":"))) { scope.RemoveLast(); }
        token->SetTypeName(name);

        // Did we got a scope as well?
        if(!scope.IsEmpty()) token->SetTypeScope(scope);

        wxString argsString = where->second.AfterFirst(wxT('<'));
        argsString.Prepend(wxT("<"));

        DoRemoveTempalteInitialization(argsString, argList);
        if(argList.IsEmpty() == false) {
            // If we already got a concrete template initialization list
            // do not override it with the dummy one taken from the user
            // type definition
            if(token->GetTemplateInitialization().IsEmpty()) token->SetTemplateInitialization(argList);
            token->SetIsTemplate(true);
        }
        return true;
    }
    return false;
}

bool Language::DoIsTypeAndScopeExist(ParsedToken* token)
{
    // Check to see if this is a primitve type...
    if(is_primitive_type(token->GetTypeName().mb_str(wxConvUTF8).data())) { return true; }

    // Does the typename is happen to be a template argument?
    if(m_templateArgs.count(token->GetTypeName())) { return true; }

    wxString type(token->GetTypeName());
    wxString scope(token->GetTypeScope());
    bool res = GetTagsManager()->IsTypeAndScopeExists(type, scope);

    token->SetTypeName(type);
    token->SetTypeScope(scope);
    return res;
}

bool Language::DoCorrectUsingNamespaces(ParsedToken* token, std::vector<TagEntryPtr>& tags)
{
    wxString type(token->GetTypeName());
    wxString scope(token->GetTypeScope());
    bool res = CorrectUsingNamespace(type, scope, token->GetContextScope(), tags);

    token->SetTypeName(type);
    token->SetTypeScope(scope);

    return res;
}

void Language::DoExtractTemplateInitListFromInheritance(TagEntryPtr tag, ParsedToken* token)
{
    wxArrayString initList;
    wxString parent;
    wxString scope;

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
        wxArrayString inherits = tag->GetInheritsAsArrayWithTemplates();
        wxArrayString inheritsNoTemplate = tag->GetInheritsAsArrayNoTemplates();
        size_t i = 0;
        for(; i < inherits.size(); i++) {
            DoRemoveTempalteInitialization(inherits.Item(i), initList);
            if(initList.IsEmpty() == false) { break; }
        }

        if(initList.IsEmpty() == false) {
            token->SetIsTemplate(true);
            token->SetTemplateInitialization(initList);

            if(i < inheritsNoTemplate.GetCount()) {
                parent = inheritsNoTemplate.Item(i);
                scope = tag->GetScope();

                // Find this parent
                GetTagsManager()->IsTypeAndScopeExists(parent, scope);
                if(scope.IsEmpty() == false && scope != wxT("<global>")) { parent.Prepend(scope + wxT("::")); }

                std::vector<TagEntryPtr> tags;
                GetTagsManager()->FindByPath(parent, tags);
                if(tags.size() == 1) {
                    wxArrayString newArgList = DoExtractTemplateDeclarationArgs(tags.at(0));
                    if(newArgList.IsEmpty() == false) { token->SetTemplateArgList(newArgList, m_templateArgs); }
                }
            }
        }
    }
}

void Language::DoExtractTemplateInitListFromInheritance(ParsedToken* token)
{
    std::vector<TagEntryPtr> tags;
    GetTagsManager()->FindByPath(token->GetPath(), tags);
    if(tags.size() == 1) { DoExtractTemplateInitListFromInheritance(tags.at(0), token); }
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
    ParsedToken* token = tokeContainer->current;
    wxString scopeName = token->GetCurrentScopeName();
    wxString op = token->GetOperator();
    bool subscript = token->GetSubscriptOperator();

    wxString newTextToParse;
    newTextToParse << variableDecl << op;
    m_tokenScanner.Reset(newTextToParse);
    ParsedToken* newToken = ParseTokens(scopeName);
    if(newToken) {
        // copy the subscript operator from the local variable token to the
        // last token in the new parsed list
        ParsedToken* lastToken = newToken;
        while(lastToken && lastToken->GetNext()) {
            lastToken = lastToken->GetNext();
        }
        lastToken->SetSubscriptOperator(subscript);
        // If the local variable token has more tokens down the chain,
        // disconnect it from them while connecting the rest of the
        // tokens to the newly parsed list
        if(token->GetNext()) {
            lastToken->SetNext(token->GetNext());
            token->GetNext()->SetPrev(lastToken);
            token->SetNext(NULL);
        }
        // free the token
        ParsedToken::DeleteTokens(token);
        tokeContainer->head = newToken;
        tokeContainer->current = newToken;
        tokeContainer->SetRewind(true);
    }
}

void Language::DoExtractTemplateArgsFromSelf(ParsedToken* token)
{
    // if it is already marked as template, dont change it
    if(token->GetIsTemplate()) return;

    std::vector<TagEntryPtr> tags;
    GetTagsManager()->FindByPath(token->GetPath(), tags);
    if(tags.size() == 1 && !tags.at(0)->IsTypedef()) {
        // Not a typedef
        token->SetTemplateArgList(DoExtractTemplateDeclarationArgs(tags.at(0)), m_templateArgs);
        token->SetIsTemplate(token->GetTemplateArgList().IsEmpty() == false);
    }
}

// Adaptor to Language
static Language* gs_Language = NULL;
void LanguageST::Free()
{
    if(gs_Language) { delete gs_Language; }
    gs_Language = NULL;
}

Language* LanguageST::Get()
{
    if(gs_Language == NULL) gs_Language = new Language();
    return gs_Language;
}

wxString Language::ApplyCtagsReplacementTokens(const wxString& in)
{
    // First, get the replacement map
    CLReplacementList replacements;
    const wxStringTable_t& replacementMap = GetTagsManager()->GetCtagsOptions().GetTokensWxMap();
    wxStringTable_t::const_iterator iter = replacementMap.begin();
    for(; iter != replacementMap.end(); ++iter) {

        if(iter->second.IsEmpty()) continue;

        wxString pattern = iter->first;
        wxString replace = iter->second;
        pattern.Trim().Trim(false);
        replace.Trim().Trim(false);
        CLReplacement repl;
        repl.construct(pattern.To8BitData().data(), replace.To8BitData().data());
        if(repl.is_ok) { replacements.push_back(repl); }
    }

    if(replacements.empty()) return in;

    // Now, apply the replacements
    wxString outputStr;
    wxArrayString lines = ::wxStringTokenize(in, wxT("\r\n"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.GetCount(); i++) {
        std::string outStr = lines.Item(i).mb_str(wxConvUTF8).data();
        CLReplacementList::iterator iter = replacements.begin();
        for(; iter != replacements.end(); iter++) {
            ::CLReplacePatternA(outStr, *iter, outStr);
        }

        outputStr << wxString(outStr.c_str(), wxConvUTF8) << wxT("\n");
    }
    return outputStr;
}

int Language::DoReadClassName(CppScanner& scanner, wxString& clsname) const
{
    clsname.clear();
    int type = 0;

    while(true) {
        type = scanner.yylex();
        if(type == 0) return 0;

        if(type == IDENTIFIER) {
            clsname = scanner.YYText();

        } else if(type == '{' || type == ':') {
            return type;

        } else if(type == ';') {
            // we probably encountered a forward declaration or 'friend' statement
            clsname.Clear();
            return (int)';';
        }
    }
    return 0;
}

bool Language::InsertFunctionDecl(const wxString& clsname, const wxString& functionDecl, wxString& sourceContent,
                                  int visibility)
{
    // detemine the visibility requested
    int typeVisibility = lexPUBLIC;
    wxString strVisibility = wxT("public:\n");
    switch(visibility) {
    default:
    case 0:
        typeVisibility = lexPUBLIC;
        strVisibility = wxT("public:\n");
        break;

    case 1:
        typeVisibility = lexPROTECTED;
        strVisibility = wxT("protected:\n");
        break;

    case 2:
        typeVisibility = lexPRIVATE;
        strVisibility = wxT("private:\n");
        break;
    }

    // step 1: locate the class
    CppScanner scanner;
    scanner.SetText(sourceContent.mb_str(wxConvUTF8).data());

    bool success = false;
    int type = 0;
    while(true) {
        type = scanner.yylex();
        if(type == 0) {
            return false; // EOF
        }

        if(type == lexCLASS) {
            wxString name;
            type = DoReadClassName(scanner, name);
            if(type == 0) { return false; }

            if(name == clsname) {
                // We found the lex
                success = true;
                break;
            }
        }
    }

    if(!success) return false;

    // scanner is pointing on the class
    // We now need to find the first opening curly brace
    success = false;
    if(type == '{') {
        // DoReadClassName already consumed the '{' character
        // mark this as a success and continue
        success = true;

    } else {
        while(true) {
            type = scanner.yylex();
            if(type == 0) return false; // EOF

            if(type == '{') {
                success = true;
                break;
            }
        }
    }

    if(!success) return false;

    // search for requested visibility, if we could not locate it
    // locate the class ending curly brace
    success = false;
    int depth = 1;
    int visibilityLine = wxNOT_FOUND;
    int closingCurlyBraceLine = wxNOT_FOUND;
    while(true) {
        type = scanner.yylex();
        if(type == 0) break;

        if(type == typeVisibility) {
            visibilityLine = scanner.LineNo();
            break;
        }

        if(type == '{') {
            depth++;

        } else if(type == '}') {
            depth--;

            if(depth == 0) {
                // reached end of class
                closingCurlyBraceLine = scanner.LineNo();
                break;
            }
        }
    }

    wxString strToInsert;
    int insertLine = visibilityLine;
    if(visibilityLine == wxNOT_FOUND) {
        // could not locate the visibility line
        insertLine = closingCurlyBraceLine;
        strToInsert << strVisibility << functionDecl;
        insertLine--; // Place it one line on top of the curly brace

    } else {
        strToInsert << functionDecl;
    }

    if(insertLine == wxNOT_FOUND)
        // could not find any of the two
        return false;

    wxString newContent;
    wxArrayString lines = ::wxStringTokenize(sourceContent, wxT("\n"), wxTOKEN_RET_DELIMS);
    for(size_t i = 0; i < lines.GetCount(); i++) {
        if(insertLine == (int)i) { newContent << strToInsert; }
        newContent << lines.Item(i);
    }
    sourceContent = newContent;
    return true;
}

void Language::InsertFunctionImpl(const wxString& clsname, const wxString& functionImpl, const wxString& filename,
                                  wxString& sourceContent, int& insertedLine)
{
    insertedLine = wxNOT_FOUND;
    if(sourceContent.EndsWith(wxT("\n")) == false) sourceContent << wxT("\n");

    // What we want to do is to add our function as the last function in the scope
    ITagsStoragePtr storage = GetTagsManager()->GetDatabase();
    if(!storage) {
        // By default, append the file function to the end of the file
        sourceContent << functionImpl;
        return;
    }

    wxArrayString kinds;
    kinds.Add(wxT("function"));
    storage->SetUseCache(false);
    TagEntryPtrVector_t tags;
    storage->GetTagsByKindAndFile(kinds, filename, wxT("line"), ITagsStorage::OrderDesc, tags);
    storage->SetUseCache(true);

    if(tags.empty()) {
        // By default, append the file function to the end of the file
        sourceContent << functionImpl;
        return;
    }

    // Locate first tag from the requested scope
    TagEntryPtr tag = NULL;
    for(size_t i = 0; i < tags.size(); i++) {
        if(tags.at(i)->GetParent() == clsname) {
            tag = tags.at(i);
            break;
        }
    }

    if(!tag) {
        sourceContent << functionImpl;
        return;
    }

    int line = tag->GetLine();

    // Search for the end of this function
    // and place our function there...
    CppScanner scanner;
    scanner.SetText(sourceContent.mb_str(wxConvUTF8).data());

    int type = 0;

    // Fast forward the scanner to the line number
    while(true) {
        type = scanner.yylex();
        if(type == 0) {
            sourceContent << functionImpl;
            return;
        }

        if(scanner.LineNo() == line) break;
    }

    // search for the opening brace
    int depth = 0;
    while(true) {
        type = scanner.yylex();
        if(type == 0) {
            // EOF?
            sourceContent << functionImpl;
            return;
        }

        if(type == '{') {
            depth++;
            break;
        }
    }

    if(depth != 1) {
        // could locate the open brace
        sourceContent << functionImpl;
        return;
    }

    // now search for the closing one...
    int insertAtLine = wxNOT_FOUND;
    while(true) {
        type = scanner.yylex();
        if(type == 0) {
            // EOF?
            sourceContent << functionImpl;
            return;
        }

        if(type == '{') {
            depth++;

        } else if(type == '}') {
            depth--;

            if(depth == 0) {
                insertAtLine = scanner.lineno();
                break;
            }
        }
    }

    insertedLine = insertAtLine;

    // if we got here, it means we got a match
    wxString newContent;
    bool codeInjected = false;
    wxArrayString lines = ::wxStringTokenize(sourceContent, wxT("\n"), wxTOKEN_RET_DELIMS);
    for(size_t i = 0; i < lines.GetCount(); i++) {
        if(insertAtLine == (int)i) {
            codeInjected = true;
            newContent << functionImpl;
        }
        newContent << lines.Item(i);
    }

    if(!codeInjected) { newContent << functionImpl; }
    sourceContent = newContent;
}

int Language::GetBestLineForForwardDecl(const wxString& fileContent) const
{
    // Locating the place for adding forward declaration is one line on top of the first non comment/preprocessor
    // code. So basically we constrcut our lexer and call yylex() once (it will skip all whitespaces/comments/pp...
    // )
    CppLexer lexer(fileContent.mb_str(wxConvISO8859_1).data());

    while(true) {
        int type = lexer.lex();
        if(type == 0) {
            // EOF
            return wxNOT_FOUND;
        }
        break;
    }
    // stc is 0 based
    int line = lexer.line_number();
    if(line) --line;
    return line;
}

void Language::UpdateAdditionalScopesCache(const wxString& filename, const std::vector<wxString>& additionalScopes)
{
    if(m_additionalScopesCache.count(filename)) { m_additionalScopesCache.erase(filename); }
    m_additionalScopesCache.insert(std::make_pair(filename, additionalScopes));
}

void Language::ClearAdditionalScopesCache() { m_additionalScopesCache.clear(); }

CxxVariable::Ptr_t Language::FindLocalVariable(const wxString& name)
{
    if(m_locals.empty()) { return nullptr; }
    CxxVariable::Map_t::iterator iter = m_locals.find(name);
    if(iter == m_locals.end()) { return nullptr; }
    return iter->second;
}

CxxVariable::Ptr_t Language::FindVariableInScope(const wxString& scope, const wxString& name)
{
    CxxVariableScanner scanner(scope, eCxxStandard::kCxx11, GetTagsManager()->GetCtagsOptions().GetTokensWxMap(),
                               false);
    CxxVariable::Map_t M = scanner.GetVariablesMap();
    if(M.count(name) == 0) { return nullptr; }
    return M[name];
}
