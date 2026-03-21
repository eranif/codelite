#ifndef RENAMEREQUEST_HPP
#define RENAMEREQUEST_HPP

#include "LSP/Request.h"
namespace LSP
{

class WXDLLIMPEXP_CL RenameRequest : public Request
{
public:
    RenameRequest(const wxString& new_name, const wxString& filename, size_t line, size_t column);
    virtual ~RenameRequest() = default;

    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
    void HandleError(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
};

} // namespace LSP

#endif // RENAMEREQUEST_HPP
