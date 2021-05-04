#include "DidCloseTextDocumentRequest.h"

LSP::DidCloseTextDocumentRequest::DidCloseTextDocumentRequest(const wxString& filename)
{
    SetMethod("textDocument/didClose");
    m_params.reset(new DidCloseTextDocumentParams());
    m_params->As<DidCloseTextDocumentParams>()->SetTextDocument(TextDocumentIdentifier(filename));
}

LSP::DidCloseTextDocumentRequest::~DidCloseTextDocumentRequest() {}
