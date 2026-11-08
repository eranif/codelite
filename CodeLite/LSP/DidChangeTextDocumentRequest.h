#pragma once

#include "LSP/Notification.h"

namespace LSP
{

class WXDLLIMPEXP_CL DidChangeTextDocumentRequest : public LSP::Notification
{
public:
    explicit DidChangeTextDocumentRequest(const wxString& filename, const wxString& fileContent);
    ~DidChangeTextDocumentRequest() override = default;
};

} // namespace LSP
