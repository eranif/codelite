#include "DidCloseTextDocumentRequest.h"

LSP::DidCloseTextDocumentRequest::DidCloseTextDocumentRequest(const wxFileName& filename)
{
    SetMethod("textDocument/didClose");
    m_params.reset(new DidCloseTextDocumentParams());
    m_params->As<DidCloseTextDocumentParams>()->SetTextDocument(TextDocumentIdentifier(filename.GetFullPath()));
}

LSP::DidCloseTextDocumentRequest::~DidCloseTextDocumentRequest() {}
