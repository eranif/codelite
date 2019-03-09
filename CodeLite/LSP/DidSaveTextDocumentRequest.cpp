#include "LSP/DidSaveTextDocumentRequest.h"

LSP::DidSaveTextDocumentRequest::DidSaveTextDocumentRequest(const wxFileName& filename,
                                                                 const wxString& fileContent)
{
    SetMethod("textDocument/didSave");
    m_params.reset(new DidSaveTextDocumentParams());
    m_params->As<DidSaveTextDocumentParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<DidSaveTextDocumentParams>()->SetText(fileContent);
}

LSP::DidSaveTextDocumentRequest::~DidSaveTextDocumentRequest() {}

void LSP::DidSaveTextDocumentRequest::BuildUID()
{
    if(!m_uuid.IsEmpty()) { return; }
    m_uuid << GetMethod() << ":" << m_params->As<DidOpenTextDocumentParams>()->GetTextDocument().GetUri().GetFullPath();
}
