#pragma once

#include "LSP/Request.h"

namespace LSP
{
class WXDLLIMPEXP_CL WorkspaceSymbolRequest : public LSP::Request
{
public:
    explicit WorkspaceSymbolRequest(const wxString& query);
    ~WorkspaceSymbolRequest() override = default;
    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
};
} // namespace LSP
