#include "CompletionHelper.hpp"

CompletionHelper::CompletionHelper() {}

CompletionHelper::~CompletionHelper() {}

wxString CompletionHelper::get_expression_from_location(const wxString& file_content, size_t line, size_t character,
                                                        ITagsStoragePtr db)
{
    return wxEmptyString;
}
