#ifndef DIDOPENTEXTDOCUMENTREQUEST_H
#define DIDOPENTEXTDOCUMENTREQUEST_H

#include "MessageWithParams.h"
#include <wx/filename.h>
#include "LSP/ResponseMessage.h"
#include "LSP/Request.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidOpenTextDocumentRequest : public LSP::Request
{
public:
    DidOpenTextDocumentRequest(const wxFileName& filename, const wxString& text, const wxString& langugage);
    virtual ~DidOpenTextDocumentRequest();
};

}; // namespace LSP

#endif // DIDOPENTEXTDOCUMENTREQUEST_H
