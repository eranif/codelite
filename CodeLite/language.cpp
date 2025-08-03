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
#include "language.h"

#include "Cxx/CxxScannerTokens.h"
#include "Cxx/CxxVariableScanner.h"
#include "Cxx/y.tab.h"
#include "code_completion_api.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "precompiled_header.h"

#include <algorithm>

Language::Language()
    : m_expression(wxEmptyString)
    , m_tm(NULL)
{
}

void Language::ParseTemplateArgs(const wxString& argListStr, wxArrayString& argsList)
{
    CppScanner scanner;
    scanner.SetText(_C(argListStr));
    int type = scanner.yylex();
    wxString word = _U(scanner.YYText());

    // Eof?
    if(type == 0) {
        return;
    }
    if(type != (int)'<') {
        return;
    }

    bool nextIsArg(false);
    bool cont(true);
    while(cont) {
        type = scanner.yylex();
        if(type == 0) {
            break;
        }

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
    if(type == 0) {
        return;
    }
    if(type != (int)'<') {
        return;
    }

    int depth(1);
    wxString typeName;
    while(depth > 0) {
        type = scanner.yylex();
        if(type == 0) {
            break;
        }

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
            if(depth == 1) {
                typeName << _U(scanner.YYText());
            }
            break;
        }
    }

    if(typeName.Trim().Trim(false).IsEmpty() == false) {
        argsList.Add(typeName.Trim().Trim(false));
    }
    typeName.Empty();
}

wxString Language::GetScopeName(const wxString& in)
{
    std::vector<std::string> moreNS;

    const wxCharBuffer buf = _C(in);

    TagsManager* mgr = GetTagsManager();
    std::map<std::string, std::string> ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

    std::string scope_name = get_scope_name(buf.data(), moreNS, ignoreTokens);
    wxString scope = _U(scope_name.c_str());
    if(scope.IsEmpty()) {
        scope = wxT("<global>");
    }
    return scope;
}

bool Language::VariableFromPattern(const wxString& in, const wxString& name, Variable& var)
{
    VariableList li;
    wxString pattern(in);
    // we need to extract the return value from the pattern
    pattern = pattern.BeforeLast(wxT('$'));
    pattern = pattern.AfterFirst(wxT('^'));

    // remove C++11 angle bracket to use C++98
    wxString fixed_pattern;
    for(const wxChar& ch : pattern) {
        switch(ch) {
        case '>':
            fixed_pattern << " >";
            break;
        case '<':
            fixed_pattern << "< ";
            break;
        default:
            fixed_pattern << ch;
            break;
        }
    }
    pattern.swap(fixed_pattern);

    const wxCharBuffer patbuf = _C(pattern);
    li.clear();

    TagsManager* mgr = GetTagsManager();
    auto ignoreTokens = mgr->GetCtagsOptions().GetTokensMap();

    get_variables(patbuf.data(), li, ignoreTokens, false);

    for (Variable v : li) {
        if(name == _U(v.m_name.c_str())) {
            var = v;
            var.m_pattern = pattern.mb_str(wxConvUTF8).data();
            return true;
        }
    }
    return false;
}

std::vector<TagEntryPtr> Language::GetLocalVariables(const wxString& in)
{
    wxString pattern(in);
    pattern = pattern.Trim().Trim(false);

    CxxVariableScanner scanner(pattern, eCxxStandard::kCxx11, GetTagsManager()->GetCtagsOptions().GetTokensWxMap(),
                               true);
    CxxVariable::Vec_t locals = scanner.GetVariables(false);

    std::vector<TagEntryPtr> tags;
    for(CxxVariable::Ptr_t local : locals) {
        const wxString& tagName = local->GetName();

        TagEntryPtr tag(new TagEntry());
        tag->SetName(tagName);
        tag->SetKind(wxT("variable"));
        tag->SetParent(wxT("<local>"));
        tag->SetScope(local->GetTypeAsCxxString());
        tag->SetAccess("public");
        tag->SetPattern(local->ToString());
        tags.push_back(tag);
    }
    return tags;
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

wxArrayString Language::DoExtractTemplateDeclarationArgs(TagEntryPtr tag)
{
    wxString pattern = tag->GetPattern();
    wxString templateString;

    // extract the template declaration list
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
            if(foundTemplate) {
                templateString << word;
            }
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

void TemplateHelper::SetTemplateInstantiation(const wxString& templateInstantiation)
{
    this->templateInstantiationVector.clear();
    wxArrayString l;
    LanguageST::Get()->ParseTemplateInitList(templateInstantiation, l);
    this->templateInstantiationVector.push_back(l);
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
    if(typeScope != wxT("<global>"))
        path << typeScope << wxT("::");

    path << typeName;
    return path;
}

// Adaptor to Language
static Language* gs_Language = NULL;
void LanguageST::Free()
{
    if(gs_Language) {
        delete gs_Language;
    }
    gs_Language = NULL;
}

Language* LanguageST::Get()
{
    if(gs_Language == NULL)
        gs_Language = new Language();
    return gs_Language;
}

int Language::DoReadClassName(CppScanner& scanner, wxString& clsname) const
{
    clsname.clear();
    int type = 0;

    while(true) {
        type = scanner.yylex();
        if(type == 0)
            return 0;

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
    // determine the visibility requested
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
            if(type == 0) {
                return false;
            }

            if(name == clsname) {
                // We found the lex
                success = true;
                break;
            }
        }
    }

    if(!success)
        return false;

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
            if(type == 0)
                return false; // EOF

            if(type == '{') {
                success = true;
                break;
            }
        }
    }

    if(!success)
        return false;

    // search for requested visibility, if we could not locate it
    // locate the class ending curly brace
    success = false;
    int depth = 1;
    int visibilityLine = wxNOT_FOUND;
    int closingCurlyBraceLine = wxNOT_FOUND;
    while(true) {
        type = scanner.yylex();
        if(type == 0)
            break;

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
        if(insertLine == (int)i) {
            newContent << strToInsert;
        }
        newContent << lines.Item(i);
    }
    sourceContent = newContent;
    return true;
}

int Language::GetBestLineForForwardDecl(const wxString& fileContent) const
{
    // Locating the place for adding forward declaration is one line on top of the first non comment/preprocessor
    // code. So basically we construct our lexer and call yylex() once (it will skip all whitespaces/comments/pp...
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
    if(line)
        --line;
    return line;
}
