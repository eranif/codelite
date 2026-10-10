#ifndef DOCUMENTONTYPEFORMATTINGREQUEST_HPP
#define DOCUMENTONTYPEFORMATTINGREQUEST_HPP

#include "LSP/Request.h"

namespace LSP
{

/// Ask the server for the edits to make after `ch` was typed at `position` (`textDocument/onTypeFormatting`). The
/// reply fires `wxEVT_LSP_ON_TYPE_FORMATTED` with the text edits
class WXDLLIMPEXP_CL DocumentOnTypeFormattingRequest : public Request
{
public:
    DocumentOnTypeFormattingRequest(
        const wxString& filepath, const LSP::Position& position, const wxString& ch, size_t tabSize, bool insertSpaces);
    ~DocumentOnTypeFormattingRequest() override = default;

    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;

private:
    wxString m_filepath;
};

} // namespace LSP

#endif // DOCUMENTONTYPEFORMATTINGREQUEST_HPP
