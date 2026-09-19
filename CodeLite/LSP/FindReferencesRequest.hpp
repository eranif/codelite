#ifndef FINDREFERENCESREQUEST_HPP
#define FINDREFERENCESREQUEST_HPP

#include "LSP/Request.h"

#include <wx/string.h>

namespace LSP
{

class WXDLLIMPEXP_CL FindReferencesRequest : public LSP::Request
{
public:
    FindReferencesRequest(const wxString& filename, size_t line, size_t column, bool includeDeclaration);
    ~FindReferencesRequest() override = default;
    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
};

} // namespace LSP

#endif // FINDREFERENCESREQUEST_HPP
