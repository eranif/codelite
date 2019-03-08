#ifndef DIDOPENTEXTDOCUMENTREQUEST_H
#define DIDOPENTEXTDOCUMENTREQUEST_H

#include "RequestMessage.h"
#include <wx/filename.h>
#include "LSP/ResponseMessage.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidOpenTextDocumentRequest : public LSP::RequestMessage
{
public:
    DidOpenTextDocumentRequest(const wxFileName& filename, const wxString& text, const wxString& langugage);
    virtual ~DidOpenTextDocumentRequest();
    void OnReponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
};

} // namespace LSP

#endif // DIDOPENTEXTDOCUMENTREQUEST_H
