#ifndef DIDSAVE_TEXTDOCUMENTREQUEST_H
#define DIDSAVE_TEXTDOCUMENTREQUEST_H

#include <wx/filename.h>
#include "LSP/Notification.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidSaveTextDocumentRequest : public LSP::Notification
{
public:
    DidSaveTextDocumentRequest(const wxFileName& filename, const wxString& fileContent);
    virtual ~DidSaveTextDocumentRequest();
};
}; // namespace LSP

#endif // DIDCHANGE_TEXTDOCUMENTREQUEST_H
