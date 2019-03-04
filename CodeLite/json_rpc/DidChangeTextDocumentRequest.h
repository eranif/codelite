#ifndef DIDCHANGE_TEXTDOCUMENTREQUEST_H
#define DIDCHANGE_TEXTDOCUMENTREQUEST_H

#include <wx/filename.h>
#include "json_rpc/RequestMessage.h"

namespace json_rpc
{

class WXDLLIMPEXP_CL DidChangeTextDocumentRequest : public json_rpc::RequestMessage
{
public:
    DidChangeTextDocumentRequest(const wxFileName& filename, const wxString& fileContent);
    virtual ~DidChangeTextDocumentRequest();
};

}; // namespace json_rpc

#endif // DIDCHANGE_TEXTDOCUMENTREQUEST_H
