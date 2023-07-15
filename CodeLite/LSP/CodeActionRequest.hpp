#ifndef CODEACTIONREQUEST_HPP
#define CODEACTIONREQUEST_HPP

#include "LSP/Request.h"

namespace LSP
{

class WXDLLIMPEXP_CL CodeActionRequest : public Request
{
public:
    CodeActionRequest(const LSP::TextDocumentIdentifier& textDocument, const LSP::Range& range,
                      const std::vector<LSP::Diagnostic>& diags = {});
    virtual ~CodeActionRequest();

    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
};

} // namespace LSP

#endif // CODEACTIONREQUEST_HPP
