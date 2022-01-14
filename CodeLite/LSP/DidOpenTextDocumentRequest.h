#ifndef DIDOPENTEXTDOCUMENTREQUEST_H
#define DIDOPENTEXTDOCUMENTREQUEST_H

#include "LSP/Notification.h"
#include "MessageWithParams.h"

#include <wx/filename.h>

namespace LSP
{

class WXDLLIMPEXP_CL DidOpenTextDocumentRequest : public LSP::Notification
{
public:
    explicit DidOpenTextDocumentRequest(const wxString& filename, const wxString& text, const wxString& langugage);
    virtual ~DidOpenTextDocumentRequest();
};

}; // namespace LSP

#endif // DIDOPENTEXTDOCUMENTREQUEST_H
