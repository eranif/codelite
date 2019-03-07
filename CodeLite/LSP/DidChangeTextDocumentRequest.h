#ifndef DIDCHANGE_TEXTDOCUMENTREQUEST_H
#define DIDCHANGE_TEXTDOCUMENTREQUEST_H

#include <wx/filename.h>
#include "LSP/RequestMessage.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidChangeTextDocumentRequest : public LSP::RequestMessage
{
public:
    DidChangeTextDocumentRequest(const wxFileName& filename, const wxString& fileContent);
    virtual ~DidChangeTextDocumentRequest();
};

}; // namespace LSP

#endif // DIDCHANGE_TEXTDOCUMENTREQUEST_H
