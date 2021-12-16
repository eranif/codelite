#include "Scanner.hpp"
#include "CxxLexerAPI.h"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
#include "fileutils.h"

Scanner::Scanner() {}

Scanner::~Scanner() {}

void Scanner::scan(const wxFileName& current_file, const wxArrayString& search_path, wxStringSet_t* includes_set,
                   wxStringSet_t* using_ns_set)
{
    CxxTokenizer tokenizer;
    wxString content;
    if(!FileUtils::ReadFileContent(current_file, content)) {
        return;
    }

    wxStringSet_t seen_includes;
    tokenizer.Reset(content);
    CxxLexerToken token;
    while(tokenizer.NextToken(token)) {
        switch(token.GetType()) {
        case T_PP_INCLUDE_FILENAME: {
            wxFileName fixed_path;
            if(seen_includes.count(token.GetWXString()) == 0 &&
               IsFileExists(current_file, token.GetWXString(), search_path, &fixed_path)) {
                includes_set->insert(fixed_path.GetFullPath());
            }
        } break;
        case T_USING:
            ParseUsingNamespace(tokenizer, using_ns_set);
            break;
        default:
            break;
        }
    }
}

bool Scanner::IsFileExists(const wxFileName& current_file, const wxString& name, const wxArrayString& search_path,
                           wxFileName* fixed_path) const
{

    wxFileName fullpath(current_file.GetPath() + "/" + name);

    if(fullpath.FileExists()) {
        if(fullpath.IsRelative()) {
            fullpath.MakeAbsolute(current_file.GetPath());
        }
        *fixed_path = fullpath;
        return true;
    }

    // try the search paths
    for(const wxString& path : search_path) {
        fullpath = wxFileName(path + "/" + name);
        if(fullpath.FileExists()) {
            if(fullpath.IsRelative()) {
                fullpath.MakeAbsolute(current_file.GetPath());
            }
            *fixed_path = fullpath;
            return true;
        }
    }
    return false;
}

void Scanner::ParseUsingNamespace(CxxTokenizer& tokenizer, wxStringSet_t* using_ns_set)
{
    // Get the next token
    CxxLexerToken token;
    tokenizer.NextToken(token);

    if(token.GetType() != T_NAMESPACE)
        return;

    // Read everything until we find the ';'
    wxString usingNamespace;
    while(tokenizer.NextToken(token)) {
        if(token.GetType() == ';') {
            break;
        }
        usingNamespace << token.GetText();
    }

    if(!usingNamespace.empty() && using_ns_set->count(usingNamespace) == 0) {
        using_ns_set->insert(usingNamespace);
    }
}
