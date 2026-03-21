#ifndef HOVERREQUEST_HPP
#define HOVERREQUEST_HPP

#include "LSP/Request.h"

namespace LSP
{
class WXDLLIMPEXP_CL HoverRequest : public LSP::Request
{
public:
    explicit HoverRequest(const wxString& filename, size_t line, size_t column);
    virtual ~HoverRequest() = default;
    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
};
} // namespace LSP
#endif // HOVERREQUEST_HPP
