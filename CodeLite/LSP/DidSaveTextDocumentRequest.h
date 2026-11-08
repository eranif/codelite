#pragma once

#include "LSP/Notification.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidSaveTextDocumentRequest : public LSP::Notification
{
public:
    explicit DidSaveTextDocumentRequest(const wxString& filename, const wxString& fileContent);
    ~DidSaveTextDocumentRequest() override = default;
};
} // namespace LSP
