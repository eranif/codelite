#ifndef COMPLETIONHELPER_HPP
#define COMPLETIONHELPER_HPP

#include <wx/filename.h>
#include <wx/string.h>

#include <istorage.h>

class CompletionHelper
{
public:
    CompletionHelper();
    ~CompletionHelper();

    wxString get_expression(const wxString& file_content);
    wxString truncate_file_to_location(const wxString& file_content, size_t line, size_t column);
};

#endif // COMPLETIONHELPER_HPP
