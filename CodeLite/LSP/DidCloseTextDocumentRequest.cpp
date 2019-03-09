#include "DidCloseTextDocumentRequest.h"

LSP::DidCloseTextDocumentRequest::DidCloseTextDocumentRequest(const wxFileName& filename)
{
    SetMethod("textDocument/didClose");
    m_params.reset(new DidCloseTextDocumentParams());
    m_params->As<DidCloseTextDocumentParams>()->SetTextDocument(TextDocumentIdentifier(filename));
}

LSP::DidCloseTextDocumentRequest::~DidCloseTextDocumentRequest() {}

void LSP::DidCloseTextDocumentRequest::BuildUID()
{
    if(!m_uuid.IsEmpty()) { return; }
    m_uuid << GetMethod() << ":"
           << m_params->As<DidCloseTextDocumentParams>()->GetTextDocument().GetFilename().GetFullPath();
}
