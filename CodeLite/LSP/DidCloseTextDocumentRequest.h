#pragma once

#include "LSP/Notification.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidCloseTextDocumentRequest : public LSP::Notification
{
public:
    explicit DidCloseTextDocumentRequest(const wxString& filename);
    ~DidCloseTextDocumentRequest() override = default;
};
} // namespace LSP
