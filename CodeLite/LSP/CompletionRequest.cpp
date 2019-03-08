#include "CompletionRequest.h"

LSP::CompletionRequest::CompletionRequest(const LSP::TextDocumentIdentifier& textDocument,
                                          const LSP::Position& position)
{
    SetMethod("textDocument/completion");
    m_params.reset(new CompletionParams());
    m_params->As<CompletionParams>()->SetPosition(position);
    m_params->As<CompletionParams>()->SetTextDocument(textDocument);
}

LSP::CompletionRequest::~CompletionRequest() {}
