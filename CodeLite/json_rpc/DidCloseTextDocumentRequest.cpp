#include "DidCloseTextDocumentRequest.h"

json_rpc::DidCloseTextDocumentRequest::DidCloseTextDocumentRequest(const wxFileName& filename)
{
    SetMethod("textDocument/didClose");
    m_params.reset(new DidCloseTextDocumentParams());
    m_params->As<DidCloseTextDocumentParams>()->SetTextDocument(TextDocumentIdentifier(filename));
}

json_rpc::DidCloseTextDocumentRequest::~DidCloseTextDocumentRequest() {}
