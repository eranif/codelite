#ifndef DIDSAVE_TEXTDOCUMENTREQUEST_H
#define DIDSAVE_TEXTDOCUMENTREQUEST_H

#include <wx/filename.h>
#include "json_rpc/RequestMessage.h"

namespace json_rpc
{

class WXDLLIMPEXP_CL DidSaveTextDocumentRequest : public json_rpc::RequestMessage
{
public:
    DidSaveTextDocumentRequest(const wxFileName& filename, const wxString& fileContent);
    virtual ~DidSaveTextDocumentRequest();
};

}; // namespace json_rpc

#endif // DIDCHANGE_TEXTDOCUMENTREQUEST_H
