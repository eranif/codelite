#include "Scanner.hpp"

#include "CxxLexerAPI.h"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
#include "file_logger.h"
#include "fileutils.h"

Scanner::Scanner() {}

Scanner::~Scanner() {}

void Scanner::scan(const wxFileName& current_file, const wxArrayString& search_path, wxStringSet_t* includes_set,
                   wxStringSet_t* using_ns_set)
{
    wxString content;
    if(!FileUtils::ReadFileContent(current_file, content)) {
        return;
    }
    scan_buffer(current_file, content, search_path, includes_set, using_ns_set);
}

void Scanner::scan_buffer(const wxFileName& current_file, const wxString& content, const wxArrayString& search_path,
                          wxStringSet_t* includes_set, wxStringSet_t* using_ns_set)
{
    CxxTokenizer tokenizer;

    wxString cur_file_dir = current_file.GetPath();
    wxStringSet_t seen_includes;
    tokenizer.Reset(content);
    CxxLexerToken token;
    while(tokenizer.NextToken(token)) {
        switch(token.GetType()) {
        case T_PP_INCLUDE_FILENAME: {
            std::set<wxString> fixed_path;
            wxString include_line = fix_include_line(token.GetWXString());
            if(!include_line.empty() // valid include line
               && seen_includes.count(include_line) == 0 &&
               IsFileExists(cur_file_dir, include_line, search_path, fixed_path)) {
                seen_includes.insert(include_line);
                for (const auto& path: fixed_path) {
                    includes_set->insert(path);
                }
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

#ifdef __WXMSW__
#define DIR_SEP "\\"
#else
#define DIR_SEP "/"
#endif

static wxString fix_separators_to_platform(const wxString& str)
{
    wxString s = str;
#ifdef __WXMSW__
    s.Replace("/", "\\");
#else
    s.Replace("\\", "/");
#endif
    return s;
}

bool Scanner::IsFileExists(const wxString& current_dir, const wxString& name, const wxArrayString& search_path,
                           std::set<wxString>& fixed_path)
{
    if(m_missing_includes.count(name))
        return false;

    if(m_matches.count(name)) {
        fixed_path = m_matches[name];
        return true;
    }

    wxStructStat buf;
    wxString fullpath = current_dir + DIR_SEP + name;
    fullpath = fix_separators_to_platform(fullpath);

    if(wxStat(fullpath, &buf) == 0) {
        wxFileName fn(fullpath);
        fn.MakeAbsolute();
        fixed_path.insert(fn.GetFullPath());
    }

    // try the search paths
    for(const wxString& path : search_path) {

        fullpath = path + DIR_SEP + name;
        fullpath = fix_separators_to_platform(fullpath);
        if(wxStat(fullpath, &buf) == 0) {
            fixed_path.insert(fullpath);
        }
    }
    if (!fixed_path.empty()) {
        m_matches.insert({ name, fixed_path });
        return true;
    }
    
    m_missing_includes.insert(name);
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
        if(!usingNamespace.StartsWith("std::_") && !usingNamespace.Contains("GLIBCXX") && usingNamespace != "__pstl" &&
           usingNamespace != "__detail") {
            using_ns_set->insert(usingNamespace);
        }
    }
}

wxString Scanner::fix_include_line(const wxString& include_line)
{
    // skip STL debug folders
    if(include_line.StartsWith("<debug/")) {
        return "";
    }

    wxString includeName = include_line;
    includeName.Replace("\"", "");
    includeName.Replace("<", "");
    includeName.Replace(">", "");
    return includeName;
}
