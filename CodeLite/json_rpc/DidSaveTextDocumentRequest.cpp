#include "json_rpc/DidSaveTextDocumentRequest.h"

json_rpc::DidSaveTextDocumentRequest::DidSaveTextDocumentRequest(const wxFileName& filename,
                                                                 const wxString& fileContent)
{
    SetMethod("textDocument/didSave");
    m_params.reset(new DidSaveTextDocumentParams());
    m_params->As<DidSaveTextDocumentParams>()->SetTextDocument(TextDocumentIdentifier(filename));
    m_params->As<DidSaveTextDocumentParams>()->SetText(fileContent);
}

json_rpc::DidSaveTextDocumentRequest::~DidSaveTextDocumentRequest() {}
