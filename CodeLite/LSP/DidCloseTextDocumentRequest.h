#ifndef DIDCLOSETEXTDOCUMENTREQUEST_H
#define DIDCLOSETEXTDOCUMENTREQUEST_H

#include <wx/filename.h>
#include "LSP/RequestMessage.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidCloseTextDocumentRequest : public LSP::RequestMessage
{
public:
    DidCloseTextDocumentRequest(const wxFileName& filename);
    virtual ~DidCloseTextDocumentRequest();
    void BuildUID();
};

}; // namespace LSP

#endif // DIDCLOSETEXTDOCUMENTREQUEST_H
