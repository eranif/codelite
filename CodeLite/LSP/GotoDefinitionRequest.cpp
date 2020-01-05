#include "GotoDefinitionRequest.h"
#include "LSP/LSPEvent.h"

LSP::GotoDefinitionRequest::GotoDefinitionRequest(const wxFileName& filename, size_t line, size_t column)
    : m_filename(filename)
    , m_line(line)
    , m_column(column)
{
    SetMethod("textDocument/definition");
    m_params.reset(new TextDocumentPositionParams());
    m_params->As<TextDocumentPositionParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<TextDocumentPositionParams>()->SetPosition(Position(line, column));
}

LSP::GotoDefinitionRequest::~GotoDefinitionRequest() {}

void LSP::GotoDefinitionRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner,
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

    if(!loc.GetUri().IsEmpty()) {
        // Fire an event with the matching location
        LSPEvent definitionEvent(wxEVT_LSP_DEFINITION);
        definitionEvent.SetLocation(loc);
        owner->AddPendingEvent(definitionEvent);
    }
}

bool LSP::GotoDefinitionRequest::IsValidAt(const wxFileName& filename, size_t line, size_t col) const
{
    return (m_filename == filename) && (m_line == line) && (m_column == col);
}
