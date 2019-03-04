#ifndef DIDOPENTEXTDOCUMENTREQUEST_H
#define DIDOPENTEXTDOCUMENTREQUEST_H

#include "RequestMessage.h"
#include <wx/filename.h>

namespace json_rpc
{

class WXDLLIMPEXP_CL DidOpenTextDocumentRequest : public json_rpc::RequestMessage
{
public:
    DidOpenTextDocumentRequest(const wxFileName& filename, const wxString& text, const wxString& langugage);
    virtual ~DidOpenTextDocumentRequest();
};

} // namespace json_rpc

#endif // DIDOPENTEXTDOCUMENTREQUEST_H
