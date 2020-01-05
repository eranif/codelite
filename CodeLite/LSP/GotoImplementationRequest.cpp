#include "GotoImplementationRequest.h"
#include "LSP/LSPEvent.h"
#include "LSP/json_rpc_params.h"

LSP::GotoImplementationRequest::GotoImplementationRequest(const wxFileName& filename, size_t line, size_t column)
{
    SetMethod("textDocument/implementation");
    m_params.reset(new TextDocumentPositionParams());
    m_params->As<TextDocumentPositionParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<TextDocumentPositionParams>()->SetPosition(Position(line, column));
}

LSP::GotoImplementationRequest::~GotoImplementationRequest() {}

void LSP::GotoImplementationRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner,
                                                IPathConverter::Ptr_t pathConverter)
{
    JSONItem result = response.Get("result");
    if(!result.isOk()) { return; }
    LSP::Location loc;
    if(result.isArray()) {
        loc.FromJSON(result.arrayItem(0), pathConverter);
    } else {
        loc.FromJSON(result, pathConverter);
    }

    // We send the same event for declaraion as we do for definition
    if(!loc.GetUri().IsEmpty()) {
        LSPEvent definitionEvent(wxEVT_LSP_DEFINITION);
        definitionEvent.SetLocation(loc);
        owner->AddPendingEvent(definitionEvent);
    }
}

bool LSP::GotoImplementationRequest::IsValidAt(const wxFileName& filename, size_t line, size_t col) const
{
    return (m_filename == filename) && (m_line == line) && (m_column == col);
}
