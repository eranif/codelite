#include "LSP/DidChangeTextDocumentRequest.h"

static int counter = 0;

LSP::DidChangeTextDocumentRequest::DidChangeTextDocumentRequest(const wxString& filename, const wxString& fileContent)
{
    SetMethod("textDocument/didChange");
    m_params.reset(new DidChangeTextDocumentParams());

    VersionedTextDocumentIdentifier id;
    id.SetVersion(++counter);
    id.SetFilename(filename);
    m_params->As<DidChangeTextDocumentParams>()->SetTextDocument(id);

    TextDocumentContentChangeEvent changeEvent;
    changeEvent.SetText(fileContent);
    m_params->As<DidChangeTextDocumentParams>()->SetContentChanges({ changeEvent });
}

LSP::DidChangeTextDocumentRequest::~DidChangeTextDocumentRequest() {}
