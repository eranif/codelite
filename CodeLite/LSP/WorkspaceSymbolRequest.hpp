#ifndef WORKSPACESYMBOLREQUEST_HPP
#define WORKSPACESYMBOLREQUEST_HPP

#include "LSP/Request.h"
#include "basic_types.h"
#include <wx/filename.h>

namespace LSP
{
class WXDLLIMPEXP_CL WorkspaceSymbolRequest : public LSP::Request
{
public:
    explicit WorkspaceSymbolRequest(const wxString& query);
    virtual ~WorkspaceSymbolRequest();
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
};
} // namespace LSP
#endif // WORKSPACESYMBOLREQUEST_HPP
