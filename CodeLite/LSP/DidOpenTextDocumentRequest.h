#pragma once

#include "LSP/Notification.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidOpenTextDocumentRequest : public LSP::Notification
{
public:
    explicit DidOpenTextDocumentRequest(const wxString& filename, const wxString& text, const wxString& langusage);
    ~DidOpenTextDocumentRequest() override = default;
};

} // namespace LSP
