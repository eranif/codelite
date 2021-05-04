#ifndef DIDCLOSETEXTDOCUMENTREQUEST_H
#define DIDCLOSETEXTDOCUMENTREQUEST_H

#include "LSP/MessageWithParams.h"
#include "LSP/Notification.h"
#include <wx/filename.h>

namespace LSP
{

class WXDLLIMPEXP_CL DidCloseTextDocumentRequest : public LSP::Notification
{
public:
    explicit DidCloseTextDocumentRequest(const wxString& filename);
    virtual ~DidCloseTextDocumentRequest();
};
};     // namespace LSP
#endif // DIDCLOSETEXTDOCUMENTREQUEST_H
