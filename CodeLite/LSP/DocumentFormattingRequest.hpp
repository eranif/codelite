#ifndef DOCUMENTFORMATTINGREQUEST_HPP
#define DOCUMENTFORMATTINGREQUEST_HPP

#include "LSP/Request.h"

namespace LSP
{

/// Ask the server to format a whole document (`textDocument/formatting`), or only `range` of it
/// (`textDocument/rangeFormatting`). The reply fires `wxEVT_LSP_DOCUMENT_FORMATTED` with the text edits
class WXDLLIMPEXP_CL DocumentFormattingRequest : public Request
{
public:
    DocumentFormattingRequest(const wxString& filepath,
                              size_t tabSize,
                              bool insertSpaces,
                              const std::optional<LSP::Range>& range = std::nullopt);
    ~DocumentFormattingRequest() override = default;

    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;
    void HandleError(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;

private:
    wxString m_filepath;
};

} // namespace LSP

#endif // DOCUMENTFORMATTINGREQUEST_HPP
