#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "CxxTokenizer.h"
#include "macros.h"

#include <unordered_map>
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/string.h>

using namespace std;
class Scanner
{
    wxStringSet_t m_missing_includes;
    wxStringMap_t m_matches;

private:
    bool IsFileExists(const wxString& current_dir, const wxString& name, const wxArrayString& search_path,
                      wxFileName* fixed_path);
    void ParseUsingNamespace(CxxTokenizer& tokenizer, wxStringSet_t* using_ns_set);

    wxString fix_include_line(const wxString& include_line);

public:
    Scanner();
    ~Scanner();

    void scan(const wxFileName& current_file, const wxArrayString& search_path, wxStringSet_t* includes_set,
              wxStringSet_t* using_ns_set);
    void scan_buffer(const wxFileName& current_file, const wxString& content, const wxArrayString& search_path,
                     wxStringSet_t* includes_set, wxStringSet_t* using_ns_set);
};

#endif // SCANNER_HPP
