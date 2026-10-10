#ifndef DOCUMENTLINKREQUEST_HPP
#define DOCUMENTLINKREQUEST_HPP

#include "LSP/Request.h"

#include <optional>

namespace LSP
{

/// Ask the server for the links in a document (`textDocument/documentLink`), for example the file of an `#include`
/// or a `require` statement. The reply fires `wxEVT_LSP_DOCUMENT_LINKS`
class WXDLLIMPEXP_CL DocumentLinkRequest : public Request
{
public:
    /// @param textHash the hash of the text that was sent to the server, returned in the event
    /// @param openAt open the link at this position when the reply arrives (returned in the event)
    DocumentLinkRequest(const wxString& filepath, size_t textHash, const std::optional<LSP::Position>& openAt);
    ~DocumentLinkRequest() override = default;

    std::optional<LSPEvent> OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner) override;

private:
    wxString m_filepath;
    size_t m_textHash = 0;
    std::optional<LSP::Position> m_openAt;
};

} // namespace LSP

#endif // DOCUMENTLINKREQUEST_HPP
