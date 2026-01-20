#ifndef DIDCLOSETEXTDOCUMENTREQUEST_H
#define DIDCLOSETEXTDOCUMENTREQUEST_H

#include "LSP/Notification.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidCloseTextDocumentRequest : public LSP::Notification
{
public:
    explicit DidCloseTextDocumentRequest(const wxString& filename);
    virtual ~DidCloseTextDocumentRequest() = default;
};
} // namespace LSP
#endif // DIDCLOSETEXTDOCUMENTREQUEST_H
