#ifndef COMPLETIONREQUEST_H
#define COMPLETIONREQUEST_H

#include "LSP/RequestMessage.h"
#include "LSP/basic_types.h"

namespace LSP
{

class WXDLLIMPEXP_CL CompletionRequest : public RequestMessage
{
public:
    CompletionRequest(const LSP::TextDocumentIdentifier& textDocument, const LSP::Position& position);
    virtual ~CompletionRequest();
};

}; // namespace LSP

#endif // COMPLETIONREQUEST_H
