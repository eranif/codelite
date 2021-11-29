#ifndef COMPLETIONHELPER_HPP
#define COMPLETIONHELPER_HPP
#include "CxxTokenizer.h"

#include <functional>
#include <istorage.h>
#include <vector>
#include <wx/filename.h>
#include <wx/string.h>

using namespace std;
class CompletionHelper
{
public:
    CompletionHelper();
    ~CompletionHelper();

    wxString get_expression(const wxString& file_content, bool for_calltip, wxString* last_word = nullptr) const;
    wxString truncate_file_to_location(const wxString& file_content, size_t line, size_t column,
                                       bool only_complete_words) const;
    vector<wxString> split_function_signature(const wxString& signature, wxString* return_value) const;

    static bool is_cxx_keyword(const wxString& word);
};

#endif // COMPLETIONHELPER_HPP
