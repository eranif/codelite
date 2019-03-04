#ifndef DIDCLOSETEXTDOCUMENTREQUEST_H
#define DIDCLOSETEXTDOCUMENTREQUEST_H

#include <wx/filename.h>
#include "json_rpc/RequestMessage.h"

namespace json_rpc
{

class WXDLLIMPEXP_CL DidCloseTextDocumentRequest : public json_rpc::RequestMessage
{
public:
    DidCloseTextDocumentRequest(const wxFileName& filename);
    virtual ~DidCloseTextDocumentRequest();
};

}; // namespace json_rpc

#endif // DIDCLOSETEXTDOCUMENTREQUEST_H
