#ifndef DIDOPENTEXTDOCUMENTREQUEST_H
#define DIDOPENTEXTDOCUMENTREQUEST_H

#include "MessageWithParams.h"
#include <wx/filename.h>
#include "LSP/Notification.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidOpenTextDocumentRequest : public LSP::Notification
{
public:
    DidOpenTextDocumentRequest(const wxFileName& filename, const std::string& text, const wxString& langugage);
    virtual ~DidOpenTextDocumentRequest();
};

}; // namespace LSP

#endif // DIDOPENTEXTDOCUMENTREQUEST_H
