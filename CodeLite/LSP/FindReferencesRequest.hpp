#ifndef FINDREFERENCESREQUEST_HPP
#define FINDREFERENCESREQUEST_HPP

#include "LSP/Request.h"
#include "basic_types.h"

#include <wx/filename.h>
#include <wx/string.h>
namespace LSP
{

class WXDLLIMPEXP_CL FindReferencesRequest : public LSP::Request
{
public:
    FindReferencesRequest(const wxString& filename, size_t line, size_t column, bool includeDeclaration);
    ~FindReferencesRequest();
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
};

} // namespace LSP

#endif // FINDREFERENCESREQUEST_HPP
