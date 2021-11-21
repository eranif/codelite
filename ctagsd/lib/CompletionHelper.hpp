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

    wxString get_expression_from_location(const wxString& file_content, size_t line, size_t character,
                                          ITagsStoragePtr db);
};

#endif // COMPLETIONHELPER_HPP
