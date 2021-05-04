#ifndef DIDSAVE_TEXTDOCUMENTREQUEST_H
#define DIDSAVE_TEXTDOCUMENTREQUEST_H

#include "LSP/Notification.h"
#include <wx/filename.h>

namespace LSP
{

class WXDLLIMPEXP_CL DidSaveTextDocumentRequest : public LSP::Notification
{
public:
    explicit DidSaveTextDocumentRequest(const wxString& filename, const wxString& fileContent);
    virtual ~DidSaveTextDocumentRequest();
};
}; // namespace LSP

#endif // DIDCHANGE_TEXTDOCUMENTREQUEST_H
