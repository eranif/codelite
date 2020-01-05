#include "GotoDeclarationRequest.h"
#include "LSP/LSPEvent.h"

LSP::GotoDeclarationRequest::GotoDeclarationRequest(const wxFileName& filename, size_t line, size_t column)
    : m_filename(filename)
    , m_line(line)
    , m_column(column)
{
    SetMethod("textDocument/declaration");
    m_params.reset(new TextDocumentPositionParams());
    m_params->As<TextDocumentPositionParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<TextDocumentPositionParams>()->SetPosition(Position(line, column));
}

LSP::GotoDeclarationRequest::~GotoDeclarationRequest() {}

void LSP::GotoDeclarationRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner,
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

bool LSP::GotoDeclarationRequest::IsValidAt(const wxFileName& filename, size_t line, size_t col) const
{
    return (m_filename == filename) && (m_line == line) && (m_column == col);
}
