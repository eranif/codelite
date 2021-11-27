#include "HoverRequest.hpp"
#include "LSP/LSPEvent.h"

LSP::HoverRequest::HoverRequest(const wxString& filename, size_t line, size_t column)
{
    SetMethod("textDocument/hover");
    m_params.reset(new TextDocumentPositionParams());
    m_params->As<TextDocumentPositionParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<TextDocumentPositionParams>()->SetPosition(Position(line, column));
}

LSP::HoverRequest::~HoverRequest() {}

void LSP::HoverRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    if(!response.Has("result")) {
        return;
    }
    JSONItem res = response.Get("result");
    LSP::Hover h;
    h.FromJSON(res);

    LSPEvent event(wxEVT_LSP_HOVER);
    event.SetHover(h);
    owner->AddPendingEvent(event);
}
