#include "DidOpenTextDocumentRequest.h"
#include "json_rpc_params.h"

LSP::DidOpenTextDocumentRequest::DidOpenTextDocumentRequest(const wxString& filename, const wxString& text,
                                                            const wxString& langugage)
{
    SetMethod("textDocument/didOpen");
    m_params.reset(new DidOpenTextDocumentParams());
    m_params->As<DidOpenTextDocumentParams>()->SetTextDocument(TextDocumentItem(filename, langugage, text));
}

LSP::DidOpenTextDocumentRequest::~DidOpenTextDocumentRequest() {}
