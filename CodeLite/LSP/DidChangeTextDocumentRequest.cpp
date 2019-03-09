#include "LSP/DidChangeTextDocumentRequest.h"

LSP::DidChangeTextDocumentRequest::DidChangeTextDocumentRequest(const wxFileName& filename, const wxString& fileContent)
{
    static int counter = 0;
    SetMethod("textDocument/didChange");
    m_params.reset(new DidChangeTextDocumentParams());

    VersionedTextDocumentIdentifier id;
    id.SetVersion(++counter);
    id.SetFilename(filename);
    m_params->As<DidChangeTextDocumentParams>()->SetTextDocument(id);

    TextDocumentContentChangeEvent changeEvent;
    changeEvent.SetText(fileContent);
    m_params->As<DidChangeTextDocumentParams>()->SetContentChanges({ changeEvent });

    m_uuid << GetMethod() << ":" << filename.GetFullPath();
}

LSP::DidChangeTextDocumentRequest::~DidChangeTextDocumentRequest() {}

void LSP::DidChangeTextDocumentRequest::BuildUID()
{
    if(!m_uuid.IsEmpty()) { return; }
    m_uuid << GetMethod() << ":"
           << m_params->As<DidChangeTextDocumentParams>()->GetTextDocument().GetFilename().GetFullPath();
}
