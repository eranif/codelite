#ifndef COMPLETIONHELPER_HPP
#define COMPLETIONHELPER_HPP

#include "CxxTokenizer.h"
#include "codelite_exports.h"
#include "entry.h"

#include <functional>
#include <istorage.h>
#include <vector>
#include <wx/filename.h>
#include <wx/string.h>

using namespace std;
class WXDLLIMPEXP_CL CompletionHelper
{
public:
    CompletionHelper();
    ~CompletionHelper();

    wxString get_expression(const wxString& file_content, bool for_calltip, wxString* last_word = nullptr) const;
    wxString truncate_file_to_location(const wxString& file_content, size_t line, size_t column,
                                       bool only_complete_words) const;
    vector<wxString> split_function_signature(const wxString& signature, wxString* return_value) const;

    static bool is_cxx_keyword(const wxString& word);
    /**
     * @brief format a tag comment
     * @param tag the tag which owns the comment
     * @param input_comment raw comment
     * @return formatted comment
     */
    wxString format_comment(TagEntryPtr tag, const wxString& input_comment) const;
    /**
     * @brief same as the above, but accepts raw pointer
     */
    wxString format_comment(TagEntry* tag, const wxString& input_comment) const;

    /**
     * @brief go backward and check if the caret line is an include statement
     * @param file_content the current file. The caret is assumed at the end of the string
     * @param file_name [output]
     * @param suffix [output] does the completion entry should end with '>' or with '"'?
     */
    bool is_include_statement(const wxString& file_content, wxString *file_name, wxString* suffix) const;
};

#endif // COMPLETIONHELPER_HPP
