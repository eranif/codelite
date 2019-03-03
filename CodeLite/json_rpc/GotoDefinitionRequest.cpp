#include "GotoDefinitionRequest.h"

json_rpc::GotoDefinitionRequest::GotoDefinitionRequest(const wxFileName& filename, size_t line, size_t column)
{
    SetMethod("textDocument/definition");
    m_params.reset(new TextDocumentPositionParams());
    m_params->As<TextDocumentPositionParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<TextDocumentPositionParams>()->SetPosition(Position(line, column));
}

json_rpc::GotoDefinitionRequest::~GotoDefinitionRequest() {}
