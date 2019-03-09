#ifndef COMPLETIONREQUEST_H
#define COMPLETIONREQUEST_H

#include "LSP/RequestMessage.h"
#include "LSP/basic_types.h"

namespace LSP
{

class WXDLLIMPEXP_CL CompletionRequest : public LSP::RequestMessage
{
public:
    CompletionRequest(const LSP::TextDocumentIdentifier& textDocument, const LSP::Position& position);
    virtual ~CompletionRequest();
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
    void BuildUID();
};

}; // namespace LSP
#endif // COMPLETIONREQUEST_H
