#include "LSP/DidSaveTextDocumentRequest.h"

LSP::DidSaveTextDocumentRequest::DidSaveTextDocumentRequest(const wxString& filename, const wxString& fileContent)
{
    SetMethod("textDocument/didSave");
    m_params.reset(new DidSaveTextDocumentParams());
    m_params->As<DidSaveTextDocumentParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<DidSaveTextDocumentParams>()->SetText(fileContent);
}

LSP::DidSaveTextDocumentRequest::~DidSaveTextDocumentRequest() {}
