#ifndef DIDCLOSETEXTDOCUMENTREQUEST_H
#define DIDCLOSETEXTDOCUMENTREQUEST_H

#include <wx/filename.h>
#include "LSP/MessageWithParams.h"
#include "LSP/Request.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidCloseTextDocumentRequest : public LSP::Request
{
public:
    DidCloseTextDocumentRequest(const wxFileName& filename);
    virtual ~DidCloseTextDocumentRequest();
};

}; // namespace LSP

#endif // DIDCLOSETEXTDOCUMENTREQUEST_H
