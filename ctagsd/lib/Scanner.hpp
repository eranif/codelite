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
    bool IsFileExists(const wxFileName& current_file, const wxString& name, const wxArrayString& search_path,
                      wxFileName* fixed_path) const;
    void ParseUsingNamespace(CxxTokenizer& tokenizer, wxStringSet_t* using_ns_set);

public:
    Scanner();
    ~Scanner();

    void scan(const wxFileName& current_file, const wxArrayString& search_path, wxStringSet_t* includes_set,
              wxStringSet_t* using_ns_set);
};

#endif // SCANNER_HPP
