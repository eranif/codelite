#ifndef COMPLETIONHELPER_HPP
#define COMPLETIONHELPER_HPP

#include "codelite_exports.h"
#include "database/entry.h"

#include <vector>
#include <wx/string.h>

class WXDLLIMPEXP_CL CompletionHelper
{
public:
    enum eSplitFlags {
        STRIP_DEFAULT = 0,
        STRIP_NO_DEFAULT_VALUES = (1 << 0), // do not include the default value
        STRIP_NO_NAME = (1 << 1),           // do not include the function argument name
    };

public:
    CompletionHelper() = default;
    ~CompletionHelper() = default;

    /**
     * @brief normalize function signature by removing from its signature all
     * default values and removing the arguments name (both is configurable via `flags`)
     */
    wxString normalize_function(TagEntryPtr tag,
                                size_t flags = CompletionHelper::STRIP_NO_DEFAULT_VALUES |
                                               CompletionHelper::STRIP_NO_NAME);
    /**
     * @brief same as above
     */
    wxString normalize_function(const TagEntry* tag,
                                size_t flags = CompletionHelper::STRIP_NO_DEFAULT_VALUES |
                                               CompletionHelper::STRIP_NO_NAME);
    std::vector<wxString> split_function_signature(const wxString& signature,
                                                   wxString* return_value,
                                                   size_t flags = CompletionHelper::STRIP_DEFAULT) const;

    /**
     * @brief go backward and check if the caret line is an include statement
     * @param file_content the current file. The caret is assumed at the end of the string
     * @param file_name [output]
     * @param suffix [output] does the completion entry should end with '>' or with '"'?
     */
    bool is_include_statement(const wxString& file_content, wxString* file_name, wxString* suffix) const;
    /**
     * @brief Similar to the above, but for the current line check if the current line matches an include statement
     */
    bool is_line_include_statement(const wxString& line, wxString* file_name, wxString* suffix) const;
};

#endif // COMPLETIONHELPER_HPP
