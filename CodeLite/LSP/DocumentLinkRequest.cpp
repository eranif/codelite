#include "DocumentLinkRequest.hpp"

#include "LSP/LSPEvent.h"

LSP::DocumentLinkRequest::DocumentLinkRequest(const wxString& filepath,
                                              size_t textHash,
                                              const std::optional<LSP::Position>& openAt)
    : m_filepath(filepath)
    , m_textHash(textHash)
    , m_openAt(openAt)
{
    SetMethod("textDocument/documentLink");
    m_params.reset(new DocumentLinkParams());
    m_params->As<DocumentLinkParams>()->SetTextDocument(TextDocumentIdentifier(filepath));
}

std::optional<LSPEvent> LSP::DocumentLinkRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    LSP_DEBUG() << "LSP::DocumentLinkRequest::OnResponse()" << endl;

    // the result is `DocumentLink[]`, or `null` when there are no links
    std::vector<LSP::DocumentLink> links;
    auto result = response.Get("result");
    if (result.isArray()) {
        int count = result.arraySize();
        links.reserve(count);
        for (int i = 0; i < count; ++i) {
            LSP::DocumentLink link;
            link.FromJSON(result[i]);
            if (link.IsOk()) {
                links.push_back(link);
            }
        }
    }

    LSPEvent event{wxEVT_LSP_DOCUMENT_LINKS};
    event.SetFileName(m_filepath);
    event.SetServerName(GetServerName());
    event.SetDocumentLinks(links);
    event.SetTextHash(m_textHash);
    event.SetOpenAt(m_openAt);
    owner->AddPendingEvent(event);
    return event;
}
