#ifndef DIDSAVE_TEXTDOCUMENTREQUEST_H
#define DIDSAVE_TEXTDOCUMENTREQUEST_H

#include <wx/filename.h>
#include "LSP/RequestMessage.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidSaveTextDocumentRequest : public LSP::RequestMessage
{
public:
    DidSaveTextDocumentRequest(const wxFileName& filename, const wxString& fileContent);
    virtual ~DidSaveTextDocumentRequest();
    void BuildUID();
};

}; // namespace LSP

#endif // DIDCHANGE_TEXTDOCUMENTREQUEST_H
